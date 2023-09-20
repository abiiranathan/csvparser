#include <stdio.h>
#include "csvparser.h"

// Sample Inventory item.
typedef struct InventoryItem {
  char name[256];
  size_t price;
} Item;

void parseItem(CsvRow* row, Item* item) {
  strncpy(item->name, row->fields[0], sizeof(item->name) - 1);
  char* endptr;
  if (strcmp(row->fields[1], "") == 0) {
    item->price = 0;
  } else {
    item->price = strtoul(row->fields[1], &endptr, 10);
    // check possible overflow or underflow and set cash to 0
    if (endptr == row->fields[1] || *endptr != '\0' || item->price == 0) {
      item->price = 0;
    }
  }
}

void handle_row(size_t rowIndex, CsvRow* row) {
  Item item = {0};
  parseItem(row, &item);
  printf("Async Item %zu: \"%s\", %zu\n", rowIndex + 1, item.name, item.price);
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s [CSV file]\n", argv[0]);
    return EXIT_FAILURE;
  }

  int fd = csv_fdopen(argv[1]);
  if (fd == -1) {
    perror("error opening file");
    return EXIT_FAILURE;
  }

  CsvParser* parser = csv_new_parser(fd);
  if (!parser) {
    fprintf(stderr, "Error creating CSV parser\n");
    return EXIT_FAILURE;
  }

  csv_set_skip_header(parser, true);

  // // parse the csv data.
  // CsvRow** rows = csv_parse(parser);
  // if (!rows) {
  //   fprintf(stderr, "Error parsing CSV file\n");
  //   return 1;
  // }

  // // get number of rows
  // size_t num_rows = csv_get_numrows(parser);
  // Item items[num_rows];
  // for (size_t i = 0; i < num_rows; i++) {
  //   parseItem(rows[i], &items[i]);
  // }

  // for (size_t i = 0; i < num_rows; i++) {
  //   printf("Item %zu: \"%s\", %zu\n", i + 1, items[i].name, items[i].cash);
  // }

  // pass max_alloc > 0 to read up to max_alloc.
  csv_parse_async(parser, handle_row, 0);
  csv_parser_free(parser);
  return 0;
}
