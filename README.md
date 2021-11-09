# opt.h
Small single-header library to add command line flags to a C program using `getopt(3)`.

To use, include `opt.h` in all files that will use this library. In exactly ONE of these files, include `#define
OPT_H_IMPLEMENTATION` to generate the implementation:

    #define OPT_H_IMPLEMENTATION
    #include "opt.h"

To add flags, use one of the following macros:

    // add a flag-only option (no value)
    ADD_FLAG_OPT(flag, varname);

    // add a flag with an integer value
    ADD_INT_OPT(flag, varname);

    // add a flag with a string value
    ADD_STR_OPT(flag, varname);

These take the form `ADD_XXX_OPT(flag, varname)` where `XXX` is the type of flag (one of `FLAG`, `OPT`, or `STR`),
`flag` is a character literal for the flag (e.g., 'v'), and `varname` is the name of the variable to create and store
the flag value in. Note that for strings, the space for the variable is allocated with `malloc(3)` and the user must use
`free(3)` to free the memory when they are done.

## Example
This trivial example shows the general usage:

    #include <stdio.h>

    #define OPT_H_IMPLEMENTATION
    #include "opt.h"

    int main(int argc, char* argv[])
    {
        START_OPTIONS(argc, argv);
        ADD_FLAG_OPT('v', is_verbose);
        ADD_STR_OPT('i', input_filename);
        ADD_INT_OPT('t', time_to_live);
        END_OPTIONS();

        printf("Verbose: %d\n", is_verbose);
        if (input_filename) {
            printf("Input Filename: %s\n", input_filename);
        }
        if (time_to_live) {
            printf("Time To Live: %d\n", time_to_live);
        }

        return 0;
    }

