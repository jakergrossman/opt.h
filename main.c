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
    OPT_PROCESS(argc, argv);

    printf("Verbose: %d\n", is_verbose);

    if (OPT_has_time_to_live)
      printf("Time To Live: %d\n", time_to_live);

    if (OPT_has_input_filename) {
        printf("Input Filename: %s\n", input_filename);
        free(input_filename); // free memory malloced by opt.h
    }

    return 0;
}
