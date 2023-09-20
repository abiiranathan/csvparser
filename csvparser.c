#include "csvparser.h"

typedef struct CsvParser {
  size_t num_rows;   // Number of rows in csv, excluding empty lines
  FILE* stream;      // File stream
  int fd;            // File descriptor
  CsvRow* rows;      // Array of rows
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
char* dupstr(const char* source) {
  if (source == NULL)
    return NULL;

  size_t length = strlen(source) + 1;
  char* duplicate = (char*)malloc(length);
  if (duplicate != NULL) {
    strcpy(duplicate, source);
  }
  return duplicate;
}

// Function to parse a CSV line and split it into fields
static void parseCSVLine(const char* line, size_t rowIndex, CsvRow* row, char delim, char quote) {
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
  row->fields[row->numFields] = fieldCopy;

  row->numFields++;

  if (row->numFields < 1) {
    fprintf(stderr, "row %zu has no fields\n", rowIndex);
    exit(EXIT_FAILURE);
  }
}

static size_t line_count(CsvParser* self) {
  // count number of lines in csv file(ignore empty lines)
  size_t lines = 0;

  // Initialize prevChar to '\n' to handle the first line
  char prevChar = '\n';

  // Flag to indicate whether the header has been skipped
  bool headerSkipped = false;

  while (!feof(self->stream)) {
    char c = fgetc(self->stream);

    if (c == EOF) {
      break;  // Exit the loop when EOF is reached
    }

    // Ignore comment lines
    if (c == self->comment) {
      while ((c = fgetc(self->stream)) != EOF && c != '\n') {
        // Read and discard characters until a newline
        // or EOF is encountered
      }

      // read the new line character at end of comment line
      c = fgetc(self->stream);
      // will terminate the loop if EOF is encountered
      continue;
    }

    // Skip the header line if it exists and hasn't been skipped yet
    if (self->has_header && self->skip_header && !headerSkipped && lines == 0) {
      while ((c = fgetc(self->stream)) != EOF && c != '\n') {
        // Read and discard characters until a newline or EOF is encountered
      }
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
    FILE* stream = fdopen(fd, "r");  // convert fd to file stream with fdopen
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
  }

  return parser;
}

static void csv_allocate_rows(CsvParser* self) {
  self->num_rows = line_count(self);
  rewind(self->stream);  // reset file pointer to beginning of file

  CsvRow* rows = malloc(sizeof(CsvRow) * self->num_rows);
  if (!rows) {
    fprintf(stderr, "error allocating memory for %zu rows\n", self->num_rows);
    exit(EXIT_FAILURE);
  }
  self->rows = rows;
}

CsvRow* csv_parse(CsvParser* self) {
  // if we have already parsed the file, return the rows
  if (self->parsed) {
    return self->rows;
  }

  // read num_rows and allocate them on heap.
  if (!self->lines_counted) {
    csv_allocate_rows(self);
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

    parseCSVLine(line, rowIndex, &self->rows[rowIndex], self->delim, self->quote);
    rowIndex++;
  }

  // don't close file stream if fd is stdin
  if (self->fd > 0) {
    fclose(self->stream);
  }

  self->parsed = true;
  return self->rows;
}

size_t csv_get_numrows(const CsvParser* self) {
  if (!self->lines_counted) {
    fprintf(stderr, "called csv_get_numrows() before csv_parse()\n");
    exit(EXIT_FAILURE);
  }
  return self->num_rows;
}

static void csv_free_rows(CsvRow* rows) {
  if (!rows)
    return;

  for (size_t i = 0; i < rows->numFields; i++) {
    free(rows->fields[i]);
  }

  free(rows->fields);
  free(rows);
}

void csv_free_parser(CsvParser* self) {
  if (!self)
    return;

  if (self->rows) {
    csv_free_rows(self->rows);
  }
  free(self);
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
