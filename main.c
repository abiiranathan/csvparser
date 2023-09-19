#include <stdio.h>
#include "csvparser.h"

#if defined(__linux__)
#include <unistd.h>
#elif defined(_WIN32)
#include <io.h>
#endif

// Inventory item.
typedef struct InventoryItem {
  char name[256];  // 255 characters + null terminator
  size_t cash;     // price in cash
} Item;

void readFromCSVRow(CsvRow row, Item* item) {
  strncpy(item->name, row.fields[0], sizeof(item->name) - 1);
  char* endptr;
  if (strcmp(row.fields[1], "") == 0) {
    item->cash = 0;
  } else {
    item->cash = strtoul(row.fields[1], &endptr, 10);
    // check possible overflow or underflow and set cash to 0
    if (endptr == row.fields[1] || *endptr != '\0' || item->cash == 0) {
      item->cash = 0;
    }
  }
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s [CSV file]\n", argv[0]);
    return 1;
  }

  int fd = -1;
  const char* filename = argv[1];
#if defined(__linux__)
  fd = open(filename, O_RDONLY | O_EXCL);
#elif defined(_WIN32)
  fd = _open(filename, _O_RDONLY | _O_EXCL);
#endif

  if (fd == -1) {
    perror("Error opening CSV file %s for reading\n");
    return 1;
  }

  CsvParser* parser = csv_new_parser(fd);
  if (!parser) {
    fprintf(stderr, "Error creating CSV parser\n");
    return 1;
  }

  csv_set_skip_header(parser, true);

  // parse the csv data.
  CsvRow* rows = csv_parse(parser);
  if (!rows) {
    fprintf(stderr, "Error parsing CSV file\n");
    return 1;
  }

  // get number of rows
  size_t numRows = csv_get_numrows(parser);
  printf("Number of rows: %zu\n", numRows);

  // close file descriptors
  if (fd > 0) {
#if defined(__linux__)
    close(fd);
#elif defined(_WIN32)
    _close(fd);
#endif
  }

  // Allocate stack memory for the items
  Item items[numRows];

  // Read the items
  for (size_t i = 0; i < numRows; i++) {
    readFromCSVRow(rows[i], &items[i]);
  }

  // Print the items
  for (size_t i = 0; i < numRows; i++) {
    printf("Item %zu: \"%s\", %zu\n", i + 1, items[i].name, items[i].cash);
  }

  // Free the CSV rows and parser
  csv_free_parser(parser);
  return 0;
}
