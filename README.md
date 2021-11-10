# opt.h
Minimal single-header library to add command line flags to a C program using `getopt(3)`.

To use, include `opt.h` in all files that will use this library. In exactly ONE of these files, use `#define
OPT_H_IMPLEMENTATION` to generate the implementation:

```c
#define OPT_H_IMPLEMENTATION
#include "opt.h"
```

To start a flag block, use `START_OPTIONS(argc, argv)` using the `argc` and `argv` you want to pass to `getopt(3)`.

To add flags, use the `ADD_OPTION` macro:

```c
ADD_OPTION(type, flag, varname);
```

where:

- `type` is the type of flag (one of `FLAG` (no value), `INT` (integer value), or `STR` (string value)),
- `flag` is a character literal for the flag (e.g., `'v'`),
- `varname` is the name of the variable to create and store
the flag value in. Two things to note:
  - For strings, the memory for the variable is allocated with `malloc(3)` and so the user must use
    `free(3)` to free the memory when they are done.
  - An additional variable is created of the form `OPT_has_##varname` that indicates whether
    a flag was seen.

After adding options, use `END_OPTIONS()` to process the input given to `START_OPTIONS()`.

## Example
This minimal example shows the complete usage of the library:

```c
#include <stdio.h>

#define OPT_H_IMPLEMENTATION
#include "opt.h"

int
main(int argc, char** argv)
{
    START_OPTIONS(argc, argv);
    ADD_OPTION(FLAG, 'v', is_verbose);
    ADD_OPTION(INT, 't', time_to_live);
    ADD_OPTION(STR, 'i', input_filename);
    END_OPTIONS();

    printf("Verbose: %d\n", is_verbose);

    if (OPT_has_time_to_live) {
      printf("Time To Live: %d\n", time_to_live);
    }

    if (OPT_has_input_filename) {
        printf("Input Filename: %s\n", input_filename);
        free(input_filename); // free memory malloced by opt.h
    }

    return 0;
}
```
