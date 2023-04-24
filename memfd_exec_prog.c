#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

int main(int argc, char* argv[]) {
  pid_t pid = getpid();

  if(unlinkat(AT_FDCWD, argv[1], 0))
    err(1, "unlink %s failed", argv[0]);

  for(;;) {
    sleep(1);
    printf("Hello from %d\n", pid);
  }
}
