#include "nortan_test.h"
#include <sys/stat.h>

// USAGE :  "\n\tUsage: prog-name  < Output  FileName Location > \n";


/**
 * @brief entry point
 *
 * @param argc how Many
 * @param argv enviornment supplied to the program
 * @return int return code to runtime lib
 */

int main(int argc, char **argv)
{

    /**
   * @Production Comments
   *                          I will use the getopt loop to parse the input
   * arguments Defined in getopt.h from GNU or boost's command line arguments
   *                          or, windows command line parsing from win32
   *
   */

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <  Output  FileName Location >\n", argv[0]);
        exit(0);
    }

    /**
   * @PRODUCTION Comments
   *                    - an execution like ./a.out /dev/urandom WILL FAIL ! 
   *                     -->  I will sanitize the input
   *                      But this is subjective
   *                      If a binary code, is provided by a malicious user, and
   * If we  choose to use mmap implementation, even without without the PAGE
   * EXECUTE,the mapping of the write file descriptor/handle will(not just) be
   * bad.
   *
   *                      Example : Bad User provides compiled disass,
   * relocatable  code The user also knows which file descriptos are there for
   * the process Using lsof, he has essentially inserted a binary in the os.
   *                      Queston : This might sound naive, because the
   * permissions [ user, group ] of executer can be limited ( CAP_SYSADMIN -- )
   *
   *    --> the flpic/ aslr must be enabled
   *    --> we must make sure if some other thread has locked the stdin already 
   *  
   */

    if (false == check_valid_file_name(argv[1]))
    {
        errExit("\n\t Please provide valid, regular, file name \n");
    }

    if (false == make_file_for_writing(argv[1]))
    {
        errExit("Cannot open file to write to");
    }

    while (true)
    {
        read_write_loop("/dev/stdin", argv[1]);
    }

    // this line will never be executed
    return 0;
}

/**
 * @brief
 *      I am aware that there are redundant checks going on in the checks for fds
 * @param name_input
 * @param name_output
 * @return int
 */
int read_write_loop(char *name_input, char *name_output)
{

    int lenR = 0;
    int lenW = 0;

    char *buf[BUF_SIZE];
    int fin;
    int fout;
    struct stat stI;
    struct stat stO;

    fin = open(name_input, O_RDONLY);

    if (fstat(fin, &stI) == -1 || (fin == -1))
    {
        errExit("\ncannot query the file location supplied  reading\n");
        exit(0);
    }

    fout = open(name_output, O_WRONLY | O_CREAT | O_TRUNC, stO.st_mode & 0777);

    if (fstat(fout, &stO) == -1 || fout == -1)
    {
        errExit("\ncannot query the  file for write\n");
        close(fin);
        exit(0);
    }

    /**
     * @Production Comments 
     *      The following loop can be done faster 
     *          using mmap on the output fd ( not stdin)
     *              I tried this, but ran into a wired issue on my virtual machine 
     *              Also, the calculation will be tricky to make sure the offsets 
     *                  as last argument to mmap is PAGE ALIGNED 
     *                  - However, unlike a file, we do not know the offset ranges 
     *     
     *  It can also be made faster using vectored writes ( not reads )
     *  Using io_submit system call [ not glibc ] - I have done this work in past, 
     *  If asked to exactly implement one of these, I will be able to it 
     * 
     * 
     */

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

bool check_valid_file_name(const char *file_name)
{
    bool res = false;

    struct stat st;

    if (stat(file_name, &st) == -1)
    {
        errExit("\n\tCannot Query the file location\n");
    }
    else if (S_ISREG(st.st_mode) != 0)
    {
        res = true;
    }
    else
    {
        res = false;
    }

    return res;
}

// Please see carefully, all fds will be closed at end of program
// It might seem there is an fd leak, but there isn't
bool make_file_for_writing(const char *file_name)
{
    bool res = false;

    struct stat fs;

    stat(file_name, &fs);

    int fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, fs.st_mode & 0777);

    if (fd == -1)
    {
        return res;
    }
    res = true;
    return res;
}
