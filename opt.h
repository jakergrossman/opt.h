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

// ================ BEGIN PUBLIC API ==================

// flag only, no value, e.g.: `-h`
#define OPT_FLAG(option, var_name) \
    OPT_INTERNAL_ADD_OPTION(char,  OPT_FLAG, option, var_name)

// flag and integer value, e.g.: `-i 100`
#define OPT_INT(option, var_name) \
    OPT_INTERNAL_ADD_OPTION(int,   OPT_INT,  option, var_name)

// flag and string value, e.g.: `-o output.txt`
#define OPT_STR(option, var_name) \
    OPT_INTERNAL_ADD_OPTION(char*, OPT_STR,  option, var_name)

// process declared options over a given argc/argv
#define OPT_PROCESS(argc, argv) OPT_process_options(argc, argv)

// reset option state
// no memory is freed, the user is responsible for freeing
// the any memory allocated to them
#define OPT_CLEAR() \
do { \
    for (int i = 0; i < 26; i++) { \
        OPT_options[i] = OPT_NONE; \
        OPT_option_values[i] = NULL; \
        OPT_has_data_flags[i] = NULL; \
        OPT_opt_string_len = 0; \
    } \
} while (0)

// ================ END PUBLIC API ====================

#define OPT_INTERNAL_ADD_OPTION(var_type, option_type, flag, var_name) \
    var_type var_name = 0; \
    char OPT_has_##var_name = 0; \
    OPT_add_option(option_type, flag, &OPT_has_##var_name, &var_name)

enum OPT_TYPE {
    OPT_NONE = 0, // option is not declared
    OPT_FLAG,     // option has no argument
    OPT_INT,      // option has an integer argument
    OPT_STR,      // option has a string argument

    OPT_SIZE      // must be last
};

void
OPT_add_option(enum OPT_TYPE type, char option, char* has_data_flag, void* data);

void
OPT_process_options(int argc, char** argv);

#endif // end header file

// =============== BEGIN IMPLEMENTATION ===============

#ifdef OPT_H_IMPLEMENTATION

char  OPT_options[26] = { OPT_NONE }; // list of declared options (a-z only)
int   OPT_opt_string_len = 0; // length of opt_string for currently declared options
void* OPT_option_values[26] = { NULL }; // pointers to option value variables
char* OPT_has_data_flags[26] = { NULL }; // pointers to `OPT_has_var_name` flags

// which argc/argv to use for getopt
// this has all the side effects for argv that getopt normally does
int    OPT_argc;
char** OPT_argv;

void
OPT_add_option(enum OPT_TYPE type, char option, char* has_data_flag, void* data)
{
    if (type >= OPT_SIZE) {
        fprintf(stderr,
                "OPT_add_option: Unexpected type value: %d; OPT_SIZE is %d\n",
                type, OPT_SIZE);

        exit(EXIT_FAILURE);
    }

    // convert opt character to alphabet index
    int index = (int)option % 32;

    // check that this option has not been seen before
    if (OPT_options[index] != OPT_NONE) {
        fprintf(stderr,
                "OPT_add_option(): Duplicate declaration -- '%c'\n",
                option);

        exit(EXIT_FAILURE);
    }

    // save pointer to data variable
    if (data)
        OPT_option_values[index] = data;

    // store pointer to indicator option variable
    OPT_has_data_flags[index] = has_data_flag;

    // increment size of opt_string
    if (type == OPT_INT || type == OPT_STR)
        OPT_opt_string_len += 2; // required argument "a:"
    else /* OPT_FLAG */
        OPT_opt_string_len += 1; // no argument "a"

    // assign appropriate value to flags field
    OPT_options[index] = type;
}

// read and process arguments that the user has specified
void
OPT_process_options(int argc, char** argv)
{
    // save argument count and string array pointer
    OPT_argc = argc;
    OPT_argv = argv;

    int opt;
    char* opt_string;

    // allocate memory for opt_string
    opt_string = malloc(OPT_opt_string_len+1);
    if (!opt_string) {
        perror("OPT_process_options");
        exit(EXIT_FAILURE);
    }
    memset(opt_string, 0, OPT_opt_string_len+1);

    // construct opstring
    char* ptr = opt_string;
    for (size_t i = 0; i < sizeof(OPT_options); i++) {
        int type = OPT_options[i];

        if (type == OPT_NONE) // flag not defined
            continue;

        *(ptr++) = i + 'a' - 1; // translate alphabet index to lowercase letter
        if (type == OPT_INT || type == OPT_STR)
            *(ptr++) = ':'; // indicate required value
    }

    // process argument string
    while ((opt = getopt(OPT_argc, OPT_argv, opt_string)) != -1) {
        int index = (int)opt % 32; // convert opt character to alphabet index
        int type = OPT_options[index];
        switch (type) {
            case OPT_INT: {
                // integer flag
                int* ptr = (int*)OPT_option_values[index];
                *ptr = atoi(optarg);
                break;
            }
            case OPT_STR: {
                // string flag
                char** ptr = (char**)OPT_option_values[index];

                *ptr = malloc(strlen(optarg)+1);
                if (!(*ptr)) {
                    perror("OPT_process_options");
                    exit(EXIT_FAILURE);
                }

                strcpy(*ptr, optarg);
                break;
            }
            case OPT_FLAG: {
                // regular flag (no data)
                char* ptr = (char*)OPT_option_values[index];
                *ptr = 1;
                break;
            }
            default: {
                // getopt will have already printed an error on stderr
                exit(EXIT_FAILURE);
            }
        }

        // successfully processed flag
        *(OPT_has_data_flags[index]) = 1;
    }

    free(opt_string);
}

#endif
// =============== END IMPLEMENTATION ===============
