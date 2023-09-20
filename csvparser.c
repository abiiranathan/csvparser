#include "csvparser.h"
#include <limits.h>
#include <stdarg.h>

typedef struct CsvParser {
  size_t num_rows;   // Number of rows in csv, excluding empty lines
  FILE* stream;      // File stream
  int fd;            // File descriptor
  CsvRow** rows;     // Array of row pointers
  char delim;        // Delimiter character
  char quote;        // Quote character
  char comment;      // Comment character
  bool has_header;   // Whether the CSV file has a header
  bool skip_header;  // Whether to skip the header when parsing

  // Internal
  bool parsed;         // whether csv_parse has been called.
  bool lines_counted;  // whether lines already counted.
} CsvParser;

// Function to duplicate a string.
// A custom implementation of strdup.
static char* dupstr(const char* source) {
  if (source == NULL)
    return NULL;

  size_t length = strlen(source) + 1;
  char* duplicate = (char*)malloc(length);
  if (duplicate != NULL) {
    strcpy(duplicate, source);
  }
  return duplicate;
}

// Print an error message and exit
static void fatalf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  exit(EXIT_FAILURE);
}

// Function to parse a CSV line and split it into fields
static void parse_csv_line(const char* line, size_t rowIndex, CsvRow* row, char delim, char quote) {
  char field[MAX_FIELD_SIZE];
  int fieldIndex = 0;
  int insideQuotes = 0;

  row->fields = NULL;
  row->numFields = 0;

  for (int i = 0; line[i] != '\0'; i++) {
    if (line[i] == quote) {
      insideQuotes = !insideQuotes;  // Toggle insideQuotes flag
    } else if (line[i] == delim && !insideQuotes) {
      // End of field, process it
      field[fieldIndex] = '\0';

      // Allocate memory for the field and add it to the row
      char* fieldCopy = dupstr(field);
      row->fields = realloc(row->fields, (row->numFields + 1) * sizeof(char*));

      if (!row->fields) {
        fatalf("ERROR: unable to realloc() memory for row->fields");
      }

      row->fields[row->numFields] = fieldCopy;
      row->numFields++;

      fieldIndex = 0;
    } else {
      // Add character to the current field
      field[fieldIndex++] = line[i];
    }
  }

  // Process the last field
  field[fieldIndex] = '\0';

  // Allocate memory for the last field and add it to the row
  char* fieldCopy = dupstr(field);
  row->fields = realloc(row->fields, (row->numFields + 1) * sizeof(char*));
  if (!row->fields) {
    fatalf("ERROR: unable to realloc() memory for row->fields");
  }

  row->fields[row->numFields] = fieldCopy;
  row->numFields++;

  if (row->numFields < 1) {
    fatalf("row %zu has no fields\n", rowIndex);
  }
}

// count the number of lines in a csv file.
// ignore comments. Optionally skip header.
static size_t line_count(CsvParser* self) {
  size_t lines = 0;
  char prevChar = '\n';
  bool headerSkipped = false;

  while (!feof(self->stream)) {
    char c = fgetc(self->stream);
    if (c == EOF) {
      break;
    }

    // Ignore comment lines
    if (c == self->comment) {
      while ((c = fgetc(self->stream)) != EOF && c != '\n')
        ;

      // read the new line character at end of comment line
      c = fgetc(self->stream);
      continue;
    }

    // Skip the header line if it exists and hasn't been skipped yet
    if (self->has_header && self->skip_header && !headerSkipped && lines == 0) {
      while ((c = fgetc(self->stream)) != EOF && c != '\n')
        ;
      headerSkipped = true;
    }

    if (c == '\n' && !isspace(prevChar)) {
      char nextChar = fgetc(self->stream);
      if (nextChar == EOF) {
        break;
      }

      if (isspace(nextChar)) {
        continue;
      }
      lines++;

      // Put back the character if it is not EOF
      ungetc(nextChar, self->stream);
    } else if (!isspace(c)) {
      prevChar = c;
    }
  }

  // Count last line if it doesn't end with a newline character and is not empty
  if (prevChar != '\n' && !isspace(prevChar)) {
    lines++;
  }

  self->lines_counted = true;
  return lines;
}

CsvParser* csv_new_parser(int fd) {
  CsvParser* parser = malloc(sizeof(CsvParser));

  if (parser) {
    // The file descriptor is not dup'ed,
    // and will be closed when the stream created  by  fdopen()  is
    //  closed(man fdopen).
    FILE* stream = fdopen(fd, "r");
    if (!stream) {
      fprintf(stderr, "error opening file stream\n");
      return NULL;
    }

    parser->num_rows = 0;
    parser->fd = fd;
    parser->stream = stream;

    // set default values
    parser->delim = ',';
    parser->quote = '"';
    parser->comment = '#';
    parser->has_header = true;
    parser->skip_header = false;
    parser->lines_counted = false;
    parser->parsed = false;
    parser->rows = NULL;
  }

  return parser;
}

// Allocate memory for rows and set num_rows.
static void csv_allocate_rows(CsvParser* self, size_t alloc_max) {
  CsvRow** rows = malloc(self->num_rows * sizeof(CsvRow*));
  if (!rows) {
    fatalf("error allocating memory for %zu rows\n", self->num_rows);
  }

  for (size_t i = 0; i < self->num_rows; i++) {
    rows[i] = malloc(sizeof(CsvRow));
    if (!rows[i]) {
      fatalf("error allocating memory for row %zu\n", i);
    }
  }
  self->rows = rows;
}

CsvRow** csv_parse(CsvParser* self) {
  if (self->parsed) {
    fatalf("can not call csv_parse_async() or csv_parse() more than once\n");
  }

  // read num_rows and allocate them on heap.
  if (!self->lines_counted) {
    self->num_rows = line_count(self);
    rewind(self->stream);  // reset file pointer to beginning of file
    csv_allocate_rows(self, self->num_rows);
  }

  char line[MAX_FIELD_SIZE];
  size_t num_rows = self->num_rows;

  size_t rowIndex = 0;
  bool headerSkipped = false;

  while (fgets(line, MAX_FIELD_SIZE, self->stream) && rowIndex < num_rows) {
    // trim white space from end of line and skip empty lines
    char* end = line + strlen(line) - 1;
    while (end > line && isspace(*end)) {
      end--;
    }

    // If the line is empty, skip it
    if (end == line) {
      continue;
    }

    // Terminate the line with a null character
    end[1] = '\0';

    // skip comment lines
    if (line[0] == self->comment) {
      continue;
    }

    if (self->has_header && self->skip_header && rowIndex == 0 && !headerSkipped) {
      headerSkipped = true;
      continue;
    }

    parse_csv_line(line, rowIndex, self->rows[rowIndex], self->delim, self->quote);
    rowIndex++;
  }

  if (self->fd > STDERR_FILENO) {
    fclose(self->stream);  // This will close the underlying file descriptor.
  }

  self->parsed = true;
  return self->rows;
}

void csv_parse_async(CsvParser* self, RowCallback callback, size_t alloc_max) {
  if (self->parsed) {
    fatalf("can not call csv_parse_async() or csv_parse() more than once\n");
  }

  if (!self->lines_counted) {
    self->num_rows = line_count(self);
    rewind(self->stream);
  }

  size_t rowIndex = 0;
  bool headerSkipped = false;
  char line[MAX_FIELD_SIZE];

  self->num_rows = (alloc_max > 0 && alloc_max < self->num_rows) ? alloc_max : self->num_rows;
  csv_allocate_rows(self, self->num_rows);

  while (fgets(line, MAX_FIELD_SIZE, self->stream) && rowIndex < self->num_rows) {
    // trim white space from end of line and skip empty lines
    char* end = line + strlen(line) - 1;
    while (end > line && isspace(*end)) {
      end--;
    }

    // If the line is empty, skip it
    if (end == line) {
      continue;
    }

    // Terminate the line with a null character
    end[1] = '\0';

    // skip comment lines
    if (line[0] == self->comment) {
      continue;
    }

    if (self->has_header && self->skip_header && rowIndex == 0 && !headerSkipped) {
      headerSkipped = true;
      continue;
    }

    parse_csv_line(line, rowIndex, self->rows[rowIndex], self->delim, self->quote);
    // Pass the processed row to the caller.
    callback(rowIndex, self->rows[rowIndex]);
    rowIndex++;
  }

  if (self->fd > STDERR_FILENO) {
    fclose(self->stream);  // This will close the underlying file descriptor.
  }
  self->parsed = true;
}

size_t csv_get_numrows(const CsvParser* self) {
  if (!self->lines_counted) {
    fatalf("called csv_get_numrows() before csv_parse() or csv_parse_async()\n");
  }
  return self->num_rows;
}

void static csv_free_row(CsvRow* row) {
  if (row == NULL)
    return;


  for (size_t i = 0; i < row->numFields; i++) {
    free(row->fields[i]);
  }

  free(row->fields);
  free(row);
  row = NULL;
}

void csv_parser_free(CsvParser* self) {
  if (!self)
    return;

  if (self->rows) {
    for (size_t i = 0; i < self->num_rows; i++) {
      csv_free_row(self->rows[i]);
    }
    free(self->rows);
    self->rows = NULL;
  }

  free(self);
  self = NULL;
}

void csv_set_delim(CsvParser* self, char delim) {
  self->delim = delim;
}

void csv_set_quote(CsvParser* self, char quote) {
  self->quote = quote;
}

void csv_set_comment(CsvParser* self, char comment) {
  self->comment = comment;
}

void csv_set_has_header(CsvParser* self, bool has_header) {
  self->has_header = has_header;
}

void csv_set_skip_header(CsvParser* self, bool skip_header) {
  self->skip_header = skip_header;
}

int csv_fdopen(const char* filename) {
  int fd = -1;
#if defined(__linux__)
  fd = open(filename, O_RDONLY | O_EXCL);
#elif defined(_WIN32)
  fd = _open(filename, _O_RDONLY | _O_EXCL);
#endif
  return fd;
}

// Cross-platform helper to close a file descriptor.
// Ignores stdin, stdout and stderr file descriptors.
void csv_fdclose(int fd) {
  // do not close stdin, stdout or stderr.
  if (fd > STDERR_FILENO) {
#if defined(__linux__)
    close(fd);
#elif defined(_WIN32)
    _close(fd);
#endif
  }
}