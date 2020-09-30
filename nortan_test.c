#include "common.h"

const int BUF_SIZE = 1024;

#define errExit(msg)    \
  do                    \
  {                     \
    perror(msg);        \
    exit(EXIT_FAILURE); \
  } while (0)

/**
 * @brief Idiom to read and write data across files
 * @param name_input  represents the input file as char buffer
 * @param name_output represents the output as char buffer
 * @return int        if the reads and writes went through
 */
static int read_write_loop(char *name_input, char *name_output) {

  int len = 0;
  char *buf[BUF_SIZE];
  int fin;
  int fout;
  struct stat st;

  fin = open(name_input, O_RDONLY);

  if (fstat(fin, &st) == -1 || (fin == -1)) {
    errExit("\ncannot query the file location supplied  reading\n");
    exit(0);
  }

  fout = open(name_output, O_WRONLY | O_CREAT | O_TRUNC, st.st_mode & 0777);

  if (fstat(fout, &st) == -1 || fout == -1) {
    errExit("\ncannot query the  file for write\n");
    close(fin);
    exit(0);
  }

  while ((len = read(fin, buf, BUF_SIZE)) >= 0) {
    if (len == 0) {
      printf("\nclosing the reading\n");
      close(fout);
      break;
    } else {
      write(fout, buf, len);
    }
  }

  return 0;
}

int main(int argc, char **argv) {

  char name[32];
  printf("\n\t Please enter a file name to write to\n");
  scanf("%31s", name);
  printf("\n\t You want to write to %s - ", name);

  read_write_loop("/dev/stdin", name);

  return 0;
}
