#include <asm-generic/ioctls.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "spawn.h"

pid_t
spawn(const char * exe, char * const argv[])
{
    pid_t pid = fork();
    if (pid == 0)
    {
        setsid();
        int filed = open("/dev/console", O_RDWR);

        if (filed >= 0)
        {
            ioctl(filed, TIOCSCTTY, 1);

            for (int i = 0; i < 3; i++) { dup2(filed, i); }

            if (filed > 2) { close(filed); }
        }

        sigset_t empty;
        sigemptyset(&empty);
        sigprocmask(SIG_SETMASK, &empty, NULL);
        execv(exe, argv);
        _exit(127);
    }


    return pid;
}



