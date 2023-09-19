#include "../csvparser.h"
// include our header first

#include <stdio.h>
#include <string.h>

// Function to compare two CsvRow objects
static bool compareCsvRows(const CsvRow* expected, const CsvRow* actual) {
  if (expected->numFields != actual->numFields) {
    return false;
  }
  for (size_t i = 0; i < expected->numFields; i++) {
    if (strcmp(expected->fields[i], actual->fields[i]) != 0) {
      return false;
    }
  }
  return true;
}

// Function to run a CSV parser test case
static void runCsvParserTestCase(const char* csvData, CsvRow* expectedRows, size_t numExpectedRows,
                                 bool skipHeader) {
  // Create a temporary file and write the CSV data
  FILE* file = tmpfile();
  fwrite(csvData, 1, strlen(csvData), file);
  rewind(file);

  // Get the file descriptor
  int fd = fileno(file);

  // Create a CSV parser from the file descriptor
  CsvParser* parser = csv_new_parser(fd);
  csv_set_skip_header(parser, skipHeader);

  // Parse the CSV data
  CsvRow* actualRows = csv_parse(parser);

  // Verify the number of rows
  size_t numActualRows = csv_get_numrows(parser);
  if (numActualRows != numExpectedRows) {
    printf("Test failed: Expected %zu rows, but got %zu rows\n", numExpectedRows, numActualRows);
  } else {
    // Compare each row
    for (size_t i = 0; i < numExpectedRows; i++) {
      if (!compareCsvRows(&expectedRows[i], &actualRows[i])) {
        printf("Test failed: Row %zu mismatch\n", i + 1);
      }
    }
    printf("Test passed\n");
  }

  // Free resources
  csv_free_parser(parser);
  fclose(file);
}

int main() {
  // Define test data and expected results
  const char* csvData =
    "name,age\n"
    "Alice,25\n"
    "Bob,30\n"
    "Charlie,35\n";

  CsvRow expectedRows[] = {
    {.fields = (char*[]){"name", "age"}, .numFields = 2},
    {.fields = (char*[]){"Alice", "25"}, .numFields = 2},
    {.fields = (char*[]){"Bob", "30"}, .numFields = 2},
    {.fields = (char*[]){"Charlie", "35"}, .numFields = 2},
  };

  // Run the CSV parser test case
  runCsvParserTestCase(csvData, expectedRows, sizeof(expectedRows) / sizeof(expectedRows[0]),
                       false);


  // test with skip header
  CsvRow expectedRows2[] = {
    {.fields = (char*[]){"Alice", "25"}, .numFields = 2},
    {.fields = (char*[]){"Bob", "30"}, .numFields = 2},
    {.fields = (char*[]){"Charlie", "35"}, .numFields = 2},
  };
  runCsvParserTestCase(csvData, expectedRows2, sizeof(expectedRows2) / sizeof(expectedRows2[0]),
                       true);
  return 0;
}
