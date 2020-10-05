
#ifndef __NORTAN_TEST_H
#define __NORTAN_TEST_H
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const int BUF_SIZE = 1024;

#define errExit(msg)        \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

/**
 * @brief Check if a string provided is valid and regular file name
 *
 * @param file_name
 * @return true
 * @return false
 */
bool check_valid_file_name(const char *file_name);

/**
 * @brief The idiomatic [ NON Vectored, NON aio, NON  mmaped ]
 *
 * @param name_input
 * @param name_output
 * @return int
 */
static int read_write_loop(char *name_input, char *name_output);

/**
 * @brief Tries to prepare the file for write operations
 *
 * @return true
 * @return false
 */
bool make_file_for_writing(const char *);

#endif /*  NORTAN_TEST_H */
