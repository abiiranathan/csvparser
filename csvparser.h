#ifndef __CSV_PARSER_H__
#define __CSV_PARSER_H__
// Macro ensures strdup is available on linux
#define _DEFAULT_SOURCE 1


#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Structure to represent a CSV parser
typedef struct CsvParser CsvParser;

// Structure to represent a CSV row
typedef struct CsvRow {
  char** fields;     // array of fields in each row
  size_t numFields;  // number of fields in each row
} CsvRow;

// Initialize a new csv parser. CSV parser is an opaque type.
// Returns NULL on failure.
CsvParser* csv_new_parser(int fd);

// Parses the csv data and populates number of rows and row data.
CsvRow* csv_parse(CsvParser* self);

// return the number of rows. skips empty lines.
size_t csv_get_numrows(const CsvParser* self);

// Free memory used by csv rows and parser.
void csv_free_parser(CsvParser* self);

// Set delimiter. default is ','
void csv_set_delim(CsvParser* self, char delim);
// Set quote character, default '"'
void csv_set_quote(CsvParser* self, char quote);

// Set comment to be ignored. Default '#'
void csv_set_comment(CsvParser* self, char comment);

// Indicate that the file has headers. default true.
void csv_set_has_header(CsvParser* self, bool has_header);

// If skip_header && has_header are true, the first row will be skipped.
void csv_set_skip_header(CsvParser* self, bool skip_header);

#endif /* __CSV_PARSER_H__ */
