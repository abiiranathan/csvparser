/**
 * @file csv_parser.h
 * @brief CSV Parser Library
 *
 * This library provides functions for parsing CSV (Comma-Separated Values) data.
 * It allows you to read and manipulate CSV files with ease.
 */
#ifndef __CSV_PARSER_H__
#define __CSV_PARSER_H__

// This macro ensures fdopen strdup are available
#define _DEFAULT_SOURCE 1

#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifndef MAX_FIELD_SIZE
#define MAX_FIELD_SIZE 1024  // Maximum size of the csv line.
#endif

/**
 * @brief Opaque structure representing a CSV parser.
 */
typedef struct CsvParser CsvParser;

/**
 * @brief Structure representing a CSV row.
 */
typedef struct CsvRow {
  char** fields;     ///< Array of fields in each row.
  size_t numFields;  ///< Number of fields in each row.
} CsvRow;

/**
 * @brief Create a new CSV parser associated with a file descriptor.
 *
 * This function initializes a new CSV parser and associates it with the given file descriptor.
 *
 * @param fd The file descriptor of the CSV file.
 * @return A pointer to the created CsvParser, or NULL on failure.
 */
CsvParser* csv_new_parser(int fd);

/**
 * @brief Parse the CSV data and retrieve the next row.
 *
 * This function parses the CSV data and returns the next row as a CsvRow structure.
 *
 * @param self A pointer to the CsvParser.
 * @return A pointer to the next CsvRow, or NULL if there are no more rows or an error occurs.
 */
CsvRow* csv_parse(CsvParser* self);

/**
 * @brief Get the number of rows in the CSV data.
 *
 * This function returns the total number of rows in the CSV data,
 * excluding empty lines and comments.
 *
 * @param self A pointer to the CsvParser.
 * @return The number of rows.
 */
size_t csv_get_numrows(const CsvParser* self);

/**
 * @brief Free memory used by the CsvParser and CsvRow structures.
 *
 * This function releases the memory used by the CsvParser and any CsvRow structures created with it.
 *
 * @param self A pointer to the CsvParser.
 */
void csv_free_parser(CsvParser* self);

/**
 * @brief Set the delimiter character for CSV fields.
 *
 * @param self A pointer to the CsvParser.
 * @param delim The delimiter character (default is ',').
 */
void csv_set_delim(CsvParser* self, char delim);

/**
 * @brief Set the quote character for CSV fields.
 *
 * @param self A pointer to the CsvParser.
 * @param quote The quote character (default is '"').
 */
void csv_set_quote(CsvParser* self, char quote);

/**
 * @brief Set the comment character to be ignored.
 *
 * @param self A pointer to the CsvParser.
 * @param comment The comment character (default is '#').
 */
void csv_set_comment(CsvParser* self, char comment);

/**
 * @brief Indicate whether the CSV data has a header row.
 *
 * @param self A pointer to the CsvParser.
 * @param has_header True if the CSV data has a header row, false otherwise (default is true).
 */
void csv_set_has_header(CsvParser* self, bool has_header);

/**
 * @brief Skip the header row if it exists.
 *
 * If both `skip_header` and `has_header` are true, the first row (header) will be skipped during parsing.
 *
 * @param self A pointer to the CsvParser.
 * @param skip_header True to skip the header row, false otherwise (default is true).
 */
void csv_set_skip_header(CsvParser* self, bool skip_header);

#endif /* __CSV_PARSER_H__ */
