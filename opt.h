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

// typedefs to help emulate template-like behavior in ADD_OPTION
typedef int OPT_INT;
typedef int OPT_FLAG;
typedef char* OPT_STR;

// start an options block with a given argc and argv
#define START_OPTIONS(argc, argv) OPT_start_options(argc, argv);

// add an option
// type is one of FLAG, INT, or STR
#define ADD_OPTION(type, flag, varname) \
    OPT_##type varname = 0; \
    int OPT_has_##varname = 0; \
    OPT_add_option(OPT_H_##type, flag, &OPT_has_##varname, &varname);

// end the option block
#define END_OPTIONS() OPT_process_options();

#define OPT_H_NONE 0 // flag is not defined
#define OPT_H_FLAG 1 // flag has no argument
#define OPT_H_INT  2 // flag has an integer argument
#define OPT_H_STR  3 // flag has a string argument

void
OPT_start_options(int argc, char** argv);

void
OPT_add_option(int type, char flag, int* has_data_flag, void* data);

void
OPT_process_options();

#endif // end header file

#ifdef OPT_H_IMPLEMENTATION

char   OPT_flags[26] = { OPT_H_NONE }; // list of declared flags (a-z only)
int    OPT_num_flags = 0; // number of flags currently added in option block
void*  OPT_flag_values[26] = { NULL }; // pointers to flag value variables
int*   OPT_found_flags[26] = { NULL }; // pointers to `OPT_has_varname` flags

// which argc/argv to use for getopt
// NOTE: this has all the side
// effects for argv that getopt
// normally does
int OPT_argc;
char** OPT_argv;

// flag indicating whether we are in an options block
int OPT_in_progress = 0;

// cleanup any leftover data and prepare to start reading options
void
OPT_start_options(int argc, char** argv)
{
    // clear all existing data
    for (int i = 0; i < 26; i++) {
        OPT_flags[i] = OPT_H_NONE;
        OPT_flag_values[i] = NULL;
        OPT_in_progress = 0;
    }

    // save argument count and string array pointer
    OPT_argc = argc;
    OPT_argv = argv;

    OPT_in_progress = 1;
}

// read and process arguments that the user has specified
void
OPT_process_options()
{
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
        int type = OPT_flags[i];

        if (type == OPT_H_NONE) // flag not defined
            continue;

        *(ptr++) = i + 'a' - 1; // translate alphabet index to lowercase letter

        if (type == OPT_H_INT || type == OPT_H_STR)
            *(ptr++) = ':'; // indicate required value
    }

    // process argument string
    while ((opt = getopt(OPT_argc, OPT_argv, arg_string)) != -1) {
        int index = (int)opt % 32; // convert opt character to alphabet index
        int type = OPT_flags[index];
        switch (type) {
            case OPT_H_INT: {
                // integer flag
                int* ptr = (int*)OPT_flag_values[index];

                *ptr = atoi(optarg);
                break;
            }
            case OPT_H_STR: {
                // string flag
                char** ptr = (char**)OPT_flag_values[index];

                *ptr = malloc(strlen(optarg)+1);

                if (!(*ptr)) {
                    perror("OPT_process_options");
                    exit(EXIT_FAILURE);
                }

                strcpy(*ptr, optarg);
                break;
            }
            case OPT_H_FLAG: {
                // regular flag (no data)
                int* ptr = (int*)OPT_flag_values[index];

                *ptr = 1;
                break;
            }
            default: {
                // getopt will have already printed an error on stderr
                exit(EXIT_FAILURE);
            }
        }

        // successfully processed flag
        if (OPT_found_flags[index])
            *(OPT_found_flags[index]) = 1;
    }

    free(arg_string);
    OPT_in_progress = 0;
}

void
OPT_add_option(int type, char flag, int* has_data_flag, void* data)
{
    if (!OPT_in_progress) {
        fprintf(stderr, "OPT_add_option(): called outside of an options block\n");
        exit(EXIT_FAILURE);
    }

    // convert opt character to alphabet index
    int index = (int)flag % 32;

    // check that this flag has not been seen before
    if (OPT_flags[index] != OPT_H_NONE) {
        fprintf(stderr, "OPT_add_option(): Duplicate declaration -- '%c'\n", flag);
        exit(EXIT_FAILURE);
    }

    // save pointer to data variable
    if (data) {
        if (!type == OPT_H_INT && !type == OPT_H_STR && !type == OPT_H_FLAG) {
            fprintf(stderr, "OPT_add_option(): Unexpected type value: %d\n", type);
            exit(EXIT_FAILURE);
        }

        // save pointer to data variable
        OPT_flag_values[index] = data;
    }

    // store pointer to indicator flag variable
    OPT_found_flags[index] = has_data_flag;
    OPT_num_flags++;

    // assign appropriate value to flags field
    OPT_flags[index] = type;
}

#endif
