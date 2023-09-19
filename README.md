# CSV Parser Library

This library provides functions for parsing CSV (Comma-Separated Values) data. It allows you to read and manipulate CSV files with ease.

## Features

- Parse CSV data and retrieve rows and fields.
- Configure delimiter, quote character, and comment character.
- Support for skipping header rows.
- Lightweight and easy to use.

## Usage

To use this library, follow these steps:

1. Include the `csvparser.h` header in your C code.
2. Create a `CsvParser` object using `csv_new_parser(int fd)` and associate it with a file descriptor.
3. Configure parser settings (delimiter, quote character, etc.) as needed.
4. Use `csv_parse(CsvParser* self)` to parse CSV data and retrieve rows.
5. Manage memory with `csv_get_numrows`, `csv_free_parser`, and other functions.
6. Use _csv_set.._ set of functions to configure the parser settings.

## Example

```c
#include "csvparser.h"

int main() {
    int fd = /* your file descriptor here */;
    CsvParser* parser = csv_new_parser(fd);

    // Configure parser settings if needed
    csv_set_delim(parser, ',');
    csv_set_quote(parser, '"');

    CsvRow* rows = csv_parse(parser);
    int numrows = csv_get_numrows(parser);

    // Print the first row field
    CsvRow* row = rows[0];
    for (int i = 0; i < row->numfields; i++) {
        printf("%s\n", row->fields[i][0]);
    }

    // Clean up
    csv_free_parser(parser);
    return 0;
}
```
