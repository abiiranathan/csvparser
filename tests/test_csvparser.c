#include <solidc/filepath.h>
#include "../csvparser.h"

#include <stdio.h>
#include <string.h>

// Function to compare two CsvRow objects
static bool compareCsvRows(const CsvRow* expected, const CsvRow* actual) {
  if (expected->numFields != actual->numFields) {
    return false;
  }

  for (size_t i = 0; i < expected->numFields; i++) {
    if (strcmp(expected->fields[i], actual->fields[i]) != 0) {
      printf("Expected: %s, Actual: %s\n", expected->fields[i], actual->fields[i]);
      return false;
    }
  }
  return true;
}

// Function to run a CSV parser test case
static void runCsvParserTestCase(const char* csvData, CsvRow* expectedRows, size_t numExpectedRows, bool skipHeader,
                                 bool hasHeader) {
  // Create a temporary file and write the CSV data
  char* tmpfile = make_tempfile();
  if (!tmpfile) {
    printf("Error creating temporary file\n");
    return;
  }

  FILE* file = fopen(tmpfile, "w");
  if (!file) {
    printf("Error creating temporary file\n");
    return;
  }

  fwrite(csvData, 1, strlen(csvData), file);
  fclose(file);

  // Create a CSV parser from the file descriptor
  CsvParser* parser = csvparser_new(tmpfile);
  if (!parser) {
    printf("Error creating CSV parser\n");
    return;
  }

  CSV_SETCONFIG(parser, .skip_header = skipHeader, .has_header = hasHeader);

  // Parse the CSV data
  CsvRow** rows = csvparser_parse(parser);
  if (!rows) {
    printf("Error parsing CSV file\n");
    return;
  }

  // Verify the number of rows
  size_t num_rows = csvparser_numrows(parser);
  if (num_rows != numExpectedRows) {
    printf("Test failed: Expected %zu rows, but got %zu rows\n", numExpectedRows, num_rows);
    for (size_t i = 0; i < numExpectedRows; i++) {
      compareCsvRows(&expectedRows[i], rows[i]);
    }
  } else {
    // Compare each row
    for (size_t i = 0; i < numExpectedRows; i++) {
      if (!compareCsvRows(&expectedRows[i], rows[i])) {
        printf("Test failed: Row %zu mismatch\n", i + 1);
      }
    }
    printf("Test passed\n");
  }

  // Free resources
  csvparser_free(parser);
  remove(tmpfile);
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

  // we are not skipping the header
  runCsvParserTestCase(csvData, expectedRows, 4, false, true);

  // test with skip header
  CsvRow expectedRows2[] = {
    {.fields = (char*[]){"Alice", "25"}, .numFields = 2},
    {.fields = (char*[]){"Bob", "30"}, .numFields = 2},
    {.fields = (char*[]){"Charlie", "35"}, .numFields = 2},
  };
  runCsvParserTestCase(csvData, expectedRows2, 3, true, true);
  return 0;
}
