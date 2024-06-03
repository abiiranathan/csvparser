# csvparser

A simple CSV parser library for C.

## Features

- Parse CSV data and retrieve rows and fields.
- Configure delimiter, quote character, and comment character.
- Support for skipping header rows.
- Lightweight and easy to use.
- Memory efficiency using arena allocation.
- Cross-platform support.

# Installation
Install solidc dependencies
```bash
git clone https://github.com/abiiranathan/solidc.git
cd solidc
mkdir -p build
cd build
cmake ..
make
sudo make install
```

Install csvparser
```bash
git clone https://github.com/abiiranathan/csvparser.git
cd csvparser
mkdir -p build
cd build
cmake ..
make
sudo make install
```

To turn off tests
```bash
cmake -DBUILD_TESTING=OFF ..
```

To turn off examples
```bash
cmake -DBUILD_EXAMPLES=OFF ..
```

## API

To use this library, follow these steps:

1. Include the `csvparser.h` header in your C code.
2. Create a `CsvParser` object using `csvparser_new(const char *)` and associate it with a file descriptor.
3. Configure parser settings (delimiter, quote character, etc.) as needed.
4. Use `csvparser_parse(CsvParser* self)` to parse CSV data and retrieve rows.
5. Use `csvparser_parse_async(CsvParser* self, RowCallback callback, size_t alloc_max)` to process each row at a time using a callback function. If `max_alloc` is set to 0, it will read all rows, otherwise, it will read up to `max_alloc` rows.
6. Get the number of rows using `csvparser_get_numrows(CsvParser* self)`.
7. Access the rows and fields using the `CsvRow` structure.
8. Free the parser and rows using `csvparser_free(CsvParser* self)`.

## symbols
- `CsvParser` - The main parser object.
- `CsvRow` - Represents a row in the CSV data.
- `CsvConfig` - Represents the configuration settings for the parser. The default values are:
  - `delimiter` = ','
  - `quote` = '"'
  - `comment` = '#'
  - `skip_header` = true
  - `has_header` = true

You can change these settings using the macro provided in the header file. This must be done before calling `csvparser_parse`.

```c
CSV_SETCONFIG(parser, .delimiter = '\t', .comment = '!', .skip_header = false, .has_header = false);
```

Or you can set the full config using the `csvparser_setconfig` function.

```c
CsvConfig config = {
    .delimiter = '\t',
    .comment = '!',
    .skip_header = false,
    .has_header = false
};
csvparser_setconfig(parser, &config);
```

### Configurable macros before including the header file
- `MAX_FIELD_SIZE` - The maximum size of a field(line) in bytes. Default is 1024.
- `CSV_ARENA_BLOCK_SIZE` - The size of the memory block for arena allocation. Default is 4096.

Pass -D option to the compiler to set these values.

```bash
gcc -D MAX_FIELD_SIZE=2048 -D CSV_ARENA_BLOCK_SIZE=8192 -o myprogram myprogram.c
```

## Example

See the [examples](./examples) directory for more examples.

```c
#include <stdio.h>  
#include "csvparser.h"

int main() {
    CsvParser* parser = csvparser_new("data.csv");
    if (parser == NULL) {
        fprintf(stderr, "Error: Unable to create parser\n");
        return 1;
    }

    CSV_SETCONFIG(parser, .delimiter = '\t', .comment = '!', .skip_header = false, .has_header = false);

    CsvRow ** rows = csvparser_parse(parser);
    if (rows == NULL) {
        fprintf(stderr, "Error: Unable to parse CSV data\n");
        csvparser_free(parser);
        return 1;
    }

    size_t numrows = csvparser_get_numrows(parser);
    printf("Number of rows: %zu\n", numrows);

    // Process the rows and fields
    for (size_t i = 0; i < numrows; i++) {
        CsvRow* row = rows[i];
        size_t numfields = row->numFields;
        printf("Row %zu: ", i);
        for (size_t j = 0; j < numfields; j++) {
            printf("%s ", row->fields[j]);
        }
        printf("\n");
    }

    // free the parser and rows
    // Do not use memory pointed by fields after freeing the parser
    csvparser_free(parser);
    return 0;
}

```

## License

This project is licensed under the MIT License.

See the [LICENSE](LICENSE) file for details.

