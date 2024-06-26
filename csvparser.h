/**
 * @file csv_parser.h
 * @brief CSV Parser Library
 *
 * This library provides functions for parsing CSV (Comma-Separated Values) data.
 * It allows you to read and manipulate CSV files with ease.
 */
#ifndef __CSV_PARSER_H__
#define __CSV_PARSER_H__

// If clang, ignore the initializer-overrides warning
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winitializer-overrides"
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverride-init"
#endif

// C++ compatibility
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <solidc/arena.h>
#include <solidc/cstr.h>
#include <solidc/file.h>

#include <stdbool.h>
#include <stddef.h>

#ifndef CSV_ARENA_BLOCK_SIZE
#define CSV_ARENA_BLOCK_SIZE 4096
#endif

#ifndef MAX_FIELD_SIZE
// Maximum size of the csv line.
#define MAX_FIELD_SIZE 1024
#endif

/**
 * @brief Opaque structure representing a CSV parser.
 * Create a new CSV parser with csvparser_new and free it with csvparser_free.
 * Use csvparser_parse to parse the CSV data and retrieve all the rows at once.
 * Use csvparser_parse_async to parse the CSV data and pass each processed row back in a callback.
 * Use csvparser_getnumrows to get the number of rows in the CSV data.
 * Use csvparser_setdelim to set the delimiter character for CSV fields.
 * 
 * You can redefine before including header the MAX_FIELD_SIZE macro to change the maximum size of the csv line
 * and the CSV_ARENA_BLOCK_SIZE macro to change the size of the arena block.
 */
typedef struct CsvParser CsvParser;

/**
 * @brief Structure representing a CSV row.
 */
typedef struct CsvRow {
  char** fields;     ///< Array of fields in each row.
  size_t numFields;  ///< Number of fields in each row.
} CsvRow;

// callback to process every row as its parsed.
typedef void (*RowCallback)(size_t rowIndex, CsvRow* row);

/**
 * @brief Create a new CSV parser associated with a filename.
 *
 * This function initializes a new CSV parser and associates it with the given filename.
 *
 * @param filename The filename of the CSV file to parse.
 * @return A pointer to the created CsvParser, or NULL on failure.
 */
CsvParser* csvparser_new(const char* filename);


/**
 * @brief Parse the CSV data and retrieve all the rows at once.
 *
 * This function parses the CSV data and returns all the rows as an array of 
 * CsvRow structure.
 *
 * The parser file descriptor and stream will automatically be closed.
 * Note that this function allocates an array of all items on the heap
 * that you must free with csv_parser_free.
 *
 * @param self A pointer to the CsvParser.
 * @return A pointer to the next CsvRow, or NULL if there are no more rows or an error occurs.
 */
CsvRow** csvparser_parse(CsvParser* self);

/**
 * @brief Parse the CSV data and pass each processed row back in a callback.
 * Return true from the callback to stop early.
 * The parser file descriptor and stream will automatically be closed.
 *
 * If alloc_max is 0, the parser will allocate all rows at once;
 * otherwise it will allocate alloc_max rows.
 * 
 * @param self A pointer to the CsvParser.
 * @param alloc_max The maximum number of rows to allocate at once.
 * @return void.
 */
void csvparser_parse_async(CsvParser* self, RowCallback callback, size_t alloc_max);

/**
 * @brief Get the number of rows in the CSV data.
 *
 * This function returns the total number of rows in the CSV data,
 * excluding empty lines and comments.
 *
 * @param self A pointer to the CsvParser.
 * @return The number of rows.
 */
size_t csvparser_numrows(const CsvParser* self);

/**
 * @brief Free memory used by the CsvParser and CsvRow structures.
 *
 * This function releases the memory used by the CsvParser and any CsvRow structures created with it.
 *
 * @param self A pointer to the CsvParser.
 */
void csvparser_free(CsvParser* self);

struct CsvConfig {
  char delim;
  char quote;
  char comment;
  bool has_header;
  bool skip_header;
};

typedef struct CsvConfig CsvConfig;

void csvparser_setconfig(CsvParser* parser, CsvConfig config);

#define CSV_SETCONFIG(parser, ...)                                                                                     \
  csvparser_setconfig(                                                                                                 \
    parser,                                                                                                            \
    (CsvConfig){.delim = ',', .quote = '"', .comment = '#', .has_header = true, .skip_header = true, __VA_ARGS__})

#ifdef __cplusplus
}
#endif

#endif /* __CSV_PARSER_H__ */
