# opt.h
Minimal single-header library to add command line flags to a C program using `getopt(3)`.

## Usage

To use, include `opt.h` in all files that will use this library. In exactly ONE of these files, use `#define
OPT_H_IMPLEMENTATION` to generate the implementation:

```c
#define OPT_H_IMPLEMENTATION
#include "opt.h"
```

Before starting a new set of options, call `OPT_CLEAR()` to reset the internal option state.

Then, to add options, use:

```c
OPT_FLAG(option, var_name);
OPT_INT(option, var_name);
OPT_STR(option, var_name);
```

where:

- `option` is a character literal representing the flag to use

- `var_name` is the name of the variable to create and store the flag data into.

For strings, the memory for the variable is allocated with `malloc(3)` and so the user must use `free(3)` to free the
memory when they are done.  An additional variable is created of the form `OPT_has_##varname` that indicates whether a
flag was seen.

After adding options, use `OPT_PROCESS(argc, argv)` to process options over a given `argc` and `argv`. Note that this
has the same side effects as `getopt` on `argv`.

---

## Example
This minimal example shows the complete usage of the library:

```c
#include <stdio.h>

#define OPT_H_IMPLEMENTATION
#include "opt.h"

int
main(int argc, char** argv)
{
    OPT_CLEAR();
    OPT_FLAG('v', is_verbose);
    OPT_INT('t', time_to_live);
    OPT_STR('i', input_filename);
    OPT_PROCESS(argv, argv);

    printf("Verbose: %d\n", is_verbose);

    if (OPT_has_time_to_live)
      printf("Time To Live: %d\n", time_to_live);

    if (OPT_has_input_filename) {
        printf("Input Filename: %s\n", input_filename);
        free(input_filename); // free memory malloced by opt.h
    }

    return 0;
}
```
