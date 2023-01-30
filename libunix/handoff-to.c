#include <unistd.h>
#include <fcntl.h>
#include "libunix.h"

int is_fd_open(int fd) {
    return fcntl(fd, F_GETFL) >= 0;
}

void handoff_to(int our_fd, int child_fd, char *argv[]) {
    // don't dup if we are already running with <TRACE_FD> open.
    if(our_fd != child_fd)
        dup2(our_fd, child_fd);

    int pid = fork();
    if(pid < 0)
        sys_die("fork", cannot fork process?);
    if(pid == 0) {
        argv_print("about to handoff to", argv);
        if(execvp(argv[0], argv) < 0)
            sys_die(execvp, bogus);
    }
    int status;
    if(!child_clean_exit(pid, &status))
        die("child: <%s> crashed\n", argv[0]);
    output("my-install: <%s> exited with: %d\n", argv[0], status);
}


