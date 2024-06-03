// Define CSV_ARENA_BLOCK_SIZE to 200MB for the CSV parser.
#define CSV_ARENA_BLOCK_SIZE 200 * 1024 * 1024
#include "csvparser.h"

/*
Year,Industry_aggregation_NZSIOC,Industry_code_NZSIOC,Industry_name_NZSIOC,Units,Variable_code,Variable_name,Variable_category,Value,Industry_code_ANZSIC06
*/

unsigned long to_number(const char* str) {
  char* endptr;
  unsigned long n;
  n = strtoul(str, &endptr, 10);
  // check possible overflow or underflow and set cash to 0
  // if endptr is equal to str: no conversion was performed
  // if *endptr is not equal to '\0': there are trailing characters
  // if n is 0: the conversion failed
  if (endptr == str || *endptr != '\0' || n == 0) {
    n = 0;
  }
  return n;
}

file_t* output = NULL;

void row_callback(size_t rowIndex, CsvRow* row) {
  if (row->numFields == 10) {
    fprintf(file_fp(output), "%ld,%s,%ld,%s,%s,%s,%s,%s,%ld,%ld\n", to_number(row->fields[0]), row->fields[1],
            to_number(row->fields[2]), row->fields[3], row->fields[4], row->fields[5], row->fields[6], row->fields[7],
            to_number(row->fields[8]), to_number(row->fields[9]));
  } else {
    fprintf(stderr, "[%ld]: Invalid number of fields: %ld\n", rowIndex, row->numFields);
  }
}

int main(void) {
  output = file_open("output.csv", "w");
  if (!output) {
    fprintf(stderr, "Error opening output file\n");
    return EXIT_FAILURE;
  }

  // Parse CSV of 6.3MB
  CsvParser* parser = csvparser_new("large_csv.csv");
  if (!parser) {
    fprintf(stderr, "Error creating CSV parser\n");
    return EXIT_FAILURE;
  }

  CSV_SETCONFIG(parser, .skip_header = true);
  csvparser_parse_async(parser, row_callback, 0);

  csvparser_free(parser);
  file_close(output);
  return EXIT_SUCCESS;
}