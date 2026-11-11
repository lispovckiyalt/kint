/*
 * Copyright (C) 2026 lispovckiyalt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>

#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/reboot.h>
#include <sys/wait.h>

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include "hostname.h"
#include "mount.h"
#include "spawn.h"

int 
main(void)
{
    if (getpid() != 1) { fprintf(stderr, "init must be PID 1\n"); 
    return 1; }

    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigaddset(&set, SIGTERM);   
    sigaddset(&set, SIGINT);    
    sigaddset(&set, SIGUSR1);   
    sigaddset(&set, SIGUSR2);   
    sigprocmask(SIG_BLOCK, &set, NULL);

    mount_allvfs();

    if (change_hostname() != 0)
    {
        perror("kint: something went wrong");
        sethostname("localhost", 9);
    }

    reboot(RB_DISABLE_CAD);

    char *shell_argv[] = { "/bin/bash", NULL };
    pid_t openshell = spawn("/bin/bash", shell_argv);

    int cmd = 0;
    for (;;)
    {
        int mask;
        sigwait(&set, &mask);
        if (mask == SIGCHLD)
        {
            pid_t p;
            while ((p = waitpid(-1, 0x0, WNOHANG)) > 0) 
            {
                if (p == openshell)
                {
                    sleep(1);
                    openshell = spawn("/bin/bash", shell_argv);
                }
            }
        } else {
            cmd = (mask == SIGTERM || mask == SIGUSR1) ? RB_POWER_OFF
                : (mask == SIGUSR2 || mask == SIGINT)  ? RB_AUTOBOOT : 0;
            break;
        }
    }

    kill(-1, SIGTERM);
    sleep(2);
    kill(-1, SIGKILL);
    while (waitpid(-1, NULL, WNOHANG) > 0);
    sync();
    umount2("/tmp", MNT_DETACH);
    mount(NULL, "/", NULL, MS_REMOUNT | MS_RDONLY, NULL);
    reboot(cmd ? cmd : RB_POWER_OFF);
    for (;;) pause();
}
