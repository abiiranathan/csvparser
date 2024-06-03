#include <csvparser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Sample Inventory item.
typedef struct InventoryItem {
  char name[256];
  size_t price;
} Item;

unsigned long to_number(const char* str) {
  char* endptr;
  unsigned long n;
  n = strtoul(str, &endptr, 10);
  // check possible overflow or underflow and set cash to 0
  if (endptr == str || *endptr != '\0' || n == 0) {
    n = 0;
  }
  return n;
}

void parseItem(CsvRow* row, Item* item) {
  if (row->numFields == 2) {
    strncpy(item->name, row->fields[0], sizeof(item->name) - 1);
    item->price = to_number(row->fields[1]);
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

  // open the file
  const char* filename = argv[1];
  CsvParser* parser = csvparser_new(filename);
  if (!parser) {
    fprintf(stderr, "Error creating CSV parser\n");
    return EXIT_FAILURE;
  }

  CsvRow** rows = csvparser_parse(parser);
  if (!rows) {
    fprintf(stderr, "Error parsing CSV file\n");
    return EXIT_FAILURE;
  }

  size_t num_rows = csvparser_numrows(parser);
  Item* items = calloc(num_rows, sizeof(Item));
  if (!items) {
    fprintf(stderr, "Error allocating memory for items\n");
    return EXIT_FAILURE;
  }

  for (size_t i = 0; i < num_rows; i++) {
    parseItem(rows[i], &items[i]);
  }

  for (size_t i = 0; i < num_rows; i++) {
    printf("Item %zu: \"%s\", %zu\n", i + 1, items[i].name, items[i].price);
  }

  // csvparser_parse_async(parser, handle_row, 0);
  csvparser_free(parser);
  free(items);
  return 0;
}
