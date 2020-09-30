#include "nortan_test.h"

/**
 * @brief         An idiomatic read/write impl across files
 * @todo -       - use I/O vector system calls
 *               - read from stdin in chunk of vectors
 *               - write using lsek64, keeping track
 *               - using io_submit or io_uring system calls
 *               - just use mmap or mmap64, just read the file like
 *               - linear address space
 *               - add unit tests, by seeing /proc/$PID/fd/0
 *
 */
const int BUF_SIZE = 1024;

#define errExit(msg)        \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

/**
 * @brief              Idiom to read and write data across files
 *
 * @param name_input  represents the input file as char buffer
 * @param name_output represents the output as char buffer
 * @return int        if the reads and writes went through
 */
static int read_write_loop(char *name_input, char *name_output)
{

    int lenR = 0;
    int lenW = 0;

    char *buf[BUF_SIZE];
    int fin;
    int fout;
    struct stat st;

    fin = open(name_input, O_RDONLY);

    if (fstat(fin, &st) == -1 || (fin == -1))
    {
        errExit("\ncannot query the file location supplied  reading\n");
        exit(0);
    }

    fout = open(name_output, O_WRONLY | O_CREAT | O_TRUNC, st.st_mode & 0777);

    if (fstat(fout, &st) == -1 || fout == -1)
    {
        errExit("\ncannot query the  file for write\n");
        close(fin);
        exit(0);
    }

    while ((lenR = read(fin, buf, BUF_SIZE)) >= 0)
    {
        if (lenR == 0)
        {
            printf("\nclosing the reading\n");
            fdatasync(fout);
            close(fout);
            break;
        }
        else
        {
            lenW = write(fout, buf, lenR);

            if (lenW == -1 || errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // there is no more disk space to write
                // or the output file's limitations have reached
                // see man page of write system call
                close(fout);
                perror("cannot transfer anymore data");
                break;
            }
            else if (lenW < lenR)
            {
                // this will happen just once
                // see man page of write system call
                continue;
            }
            else
            {
                fdatasync(fout);
            }
        }
    }

    // the last lenR if < 0 will indicate the error in the read
    // or the last read size

    return lenR;
}

int main(int argc, char **argv)
{

    char name[32];
    
    printf("\n\t Please enter a file name to write to\n");
    scanf("%31s", name);
    printf("\n\t You want to write to %s - ", name);

    read_write_loop("/dev/stdin", name);
    
    return 0;
}
