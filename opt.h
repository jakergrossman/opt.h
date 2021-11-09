/*
 * Jake Grossman <jake.r.grossman@gmail.com>
 * This is free software released in the public domain.
 * See LICENSE.txt for more information.
 */
#ifndef opt_h
#define opt_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

// start an options block with a given argc and argv
#define START_OPTIONS(argc, argv) OPT_start_options(argc, argv);

// add a flag option
#define ADD_FLAG_OPT(flag, varname) \
    int varname = 0; \
    OPT_add_option(OPT_H_FLAG, flag, no_argument, &varname);

// add an integer option
#define ADD_INT_OPT(flag, varname) \
    int varname = 0; \
    OPT_add_option(OPT_H_INT, flag, required_argument, &varname);

// add a string option
#define ADD_STR_OPT(flag, varname) \
    char* varname = NULL; \
    OPT_add_option(OPT_H_STR, flag, required_argument, &varname);

// end the option block
#define END_OPTIONS() OPT_process_options();

#define OPT_H_NONE 0 // flag is not present
#define OPT_H_FLAG 1 // flag has no argument
#define OPT_H_INT  2 // flag has an integer argument
#define OPT_H_STR  3 // flag has a string argument

void OPT_start_options(int argc, char** argv);
void OPT_add_option(int type, char flag, int is_required, void* data);
void OPT_process_options();

#endif // end header file

#ifdef OPT_H_IMPLEMENTATION

// list of found flags (letters only)
char  OPT_flags[26] = { OPT_H_NONE };
int   OPT_num_flags = 0;

// TODO: too many lists
char   OPT_is_required[26] = { 0 };
int*   OPT_flag_int_values[26] = { NULL }; // int and flag-only values share space
char** OPT_flag_str_values[26] = { NULL };

// what argc/argv to use for getopt
// NOTE: this has all the side
// effects for argv that getopt
// normally does
int OPT_argc;
char** OPT_argv;

// flag indicating whether we are in a START_OPTIONS block
int OPT_in_progress = 0;

// cleanup any leftover data and prepare to start
// reading options
void OPT_start_options(int argc, char** argv) {
    // clear all existing data
    for (int i; i < 26; i++) {
        OPT_flags[i] = OPT_H_NONE;
        OPT_flag_int_values[i] = NULL;
        OPT_flag_str_values[i] = NULL;
        OPT_in_progress = 0;
    }

    // save argument count and string array pointer
    OPT_argc = argc;
    OPT_argv = argv;

    OPT_in_progress = 1;
}

// read and process arguments that the user has specified
void OPT_process_options() {
    int opt;
    char* arg_string;

    // eagerly allocate the maximum size
    // (2 characters for each flag, 'a:')
    arg_string = malloc(OPT_num_flags * 2 + 1);
    if (!arg_string) {
        perror("OPT_process_options");
        exit(EXIT_FAILURE);
    }
    memset(arg_string, 0, OPT_num_flags*2+1);

    char* ptr = arg_string;
    for (size_t i = 0; i < sizeof(OPT_flags); i++) {
        if (OPT_flags[i] == OPT_H_NONE) {
            // flag not present
            continue;
        }

        *(ptr++) = i + 96;
        switch(OPT_is_required[i]) {
            case no_argument:
                break;
            case required_argument:
                *(ptr++) = ':';
                break;
            default:
                fprintf(stderr, "Unexpected value for OPT_is_required[%d] (%c)\n", i, i+96);
                exit(EXIT_FAILURE);
        }
    }

    while ((opt = getopt(OPT_argc, OPT_argv, arg_string)) != -1) {
        int index = (int)opt % 32;
        switch (OPT_flags[index]) {
            case OPT_H_FLAG:
                // flag only
                *(OPT_flag_int_values[index]) = 1;
                break;
            case OPT_H_INT:
                // integer
                *(OPT_flag_int_values[index]) = atoi(optarg);
                break;
            case OPT_H_STR:
                // string (mallocced, freed by user)
                *(OPT_flag_str_values[index]) = malloc(strlen(optarg)+1);
                if (!*(OPT_flag_str_values[index])) {
                    perror("OPT_process_options");
                    exit(EXIT_FAILURE);
                }
                strcpy(*(OPT_flag_str_values[index]), optarg);
                break;
        }
    }

    free(arg_string);
    OPT_in_progress = 0;
}

// backbone of the ADD_xxx_OPT macros
void OPT_add_option(int type, char flag, int is_required, void* data) {
    if (!OPT_in_progress) {
        fprintf(stderr, "OPT_add_option(): called outside of an options block\n");
        exit(EXIT_FAILURE);
    }

    // get index into flags
    size_t index = (int)flag % 32;

    // check that this flag has not been seen before
    if (OPT_flags[index] == -1) {
        fprintf(stderr, "OPT_add_option(): Duplicate declaration -- '%c'\n", flag);
        exit(EXIT_FAILURE);
    }

    switch(type) {
        case OPT_H_FLAG:
            OPT_flag_int_values[index] = (int*) data;
            break;
        case OPT_H_INT:
            OPT_flag_int_values[index] = (int*) data;
            break;
        case OPT_H_STR:
            OPT_flag_str_values[index] = (char**) data;
    }

    // increment number of flags
    OPT_num_flags++;

    // assign appropriate value to flags field
    OPT_flags[index] = type;

    OPT_is_required[index] = is_required;
}

#endif
