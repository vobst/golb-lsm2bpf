#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <sys/mman.h>
#include <sys/types.h>

#include <stdio.h>
#include <unistd.h>

#include <err.h>
#include <errno.h>

#include "memfd_exec_prog.h"

static const unsigned char memfd_exec_script[] = "#!/usr/bin/env bash\necho $1\nrm $1\nwhile true\ndo\necho $$\nsleep 1\ndone";

int main(int argc, char* argv[])
{
  unsigned int flags = 0;
  const unsigned char* code;
  size_t size;
  int fd;
  char* new_argv[] = { "1337", argv[0], NULL };
  char* new_envp[] = { NULL };

  if (argc < 2)
    goto usage;

  if (!__builtin_strncmp(argv[1], "script", 6)) {
    code = memfd_exec_script;
    size = sizeof(memfd_exec_script);
  } else if (!__builtin_strncmp(argv[1], "elf", 3)) {
    code = memfd_exec_prog;
    size = sizeof(memfd_exec_prog);
    flags |= MFD_CLOEXEC;
  } else
    goto usage;

  fd = memfd_create("foobar", flags);
  if (fd == -1)
    err(1, "%s failed", "memfd_create");

  write(fd, code, size);

  fexecve(fd, new_argv, new_envp);

  err(1, "%s failed", "fexecve");

usage:
  fprintf(stderr, "Usage: %s <script|elf>\n", argv[0]);
  return 1;
}
