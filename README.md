# opt.h
Small single-header library to add command line flags to a C program using `getopt(3)`.

To use, include `opt.h` in all files that will use this library. In exactly ONE of these files, include `#define
OPT_H_IMPLEMENTATION` to generate the implementation:

```c
#define OPT_H_IMPLEMENTATION
#include "opt.h"
```

To start a flag block, use `START_OPTIONS(argc, argv)`. `argc` and `argv` can be any values, not necessarily the values
passed to main.

To add flags, use one of the following macros:

```c
// add a flag-only option (no value)
ADD_FLAG_OPT(flag, varname);

// add a flag with an integer value
ADD_INT_OPT(flag, varname);

// add a flag with a string value
ADD_STR_OPT(flag, varname);
```

These take the form `ADD_XXX_OPT(flag, varname)` where:
- `XXX` is the type of flag (one of `FLAG`, `OPT`, or `STR`),
- `flag` is a character literal for the flag (e.g., `'v'`),
- `varname` is the name of the variable to create and store
the flag value in. Two things to note:
  - For strings, the space for the variable is allocated with `malloc(3)` and the user must use
    `free(3)` to free the memory when they are done.
  - For `INT` and `STR` type flags, an additional variable is created of the form `OPT_has_##varname` that indicates whether
    a flag was seen. This is not done for `FLAG` type flags, since the variable itself is an indication of the presence of
    a flag (0 or 1).

After adding options, use `END_OPTIONS()` to process all the options that have been added using `getopt(3)`.

## Example
This trivial example shows the general usage:

```c
#include <stdio.h>

#define OPT_H_IMPLEMENTATION
#include "opt.h"

int main(int argc, char* argv[])
{
    START_OPTIONS(argc, argv);
    ADD_FLAG_OPT('v', is_verbose);
    ADD_INT_OPT('t', time_to_live);
    ADD_STR_OPT('i', input_filename);
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
