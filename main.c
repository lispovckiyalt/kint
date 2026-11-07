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

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/reboot.h>
#include <sys/wait.h>

#include <unistd.h>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_HN_LEN 64

int
change_hn()
{
    FILE *ehn = fopen("/etc/hostname", "r");
    if (ehn == 0x0)
    {
        return 1;
    }

    char buffer[MAX_HN_LEN];

    if (fgets(buffer, sizeof(buffer), ehn) == NULL)
    {
        fclose(ehn);
        return 1;
    }
    fclose(ehn);

    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') 
    {
        buffer[length - 1] = '\0';
        length--;
    }

    if(length==0)
    {
        return 1;
    }

    if (sethostname(buffer, length) == 0)
    {
        return 0;
    } else { return 1; }
}

static void 
mount_vfs(const char * name, const char * dir, const char * filesystem, unsigned long rights)
{
    mkdir(dir, 0755);

    if (mount(name, dir, filesystem, rights, (char *)0x0) < 0 && errno != EBUSY)
    {
        fprintf(stderr, "kint: mount: %s: %s\n", 
                dir, strerror(errno));
    }
}

static pid_t spawn(const char* exe, char *const argv[])
{
    pid_t pid = fork();

    if (pid == 0)
    {
        setsid();
        int fd = open("/dev/console", O_RDWR);

        if (fd >= 0)
        {
            ioctl(fd, TIOCSCTTY, 1);

            for (int i = 0; i < 3; i++)
            {
                dup2(fd, i);
            }

            if (fd > 2) close(fd);
        }

        sigset_t empty; 
        sigemptyset(&empty);
        sigprocmask(SIG_SETMASK, &empty, NULL);
        execv(exe, argv);
        _exit(127);
    }

    return pid;
}

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

    mount_vfs("proc", "/proc", "proc", MS_NOSUID | MS_NODEV | MS_NOEXEC);
    mount_vfs("sysfs", "/sys", "sysfs", MS_NOSUID | MS_NODEV | MS_NOEXEC);
    mount_vfs("devtmpfs", "/dev", "devtmpfs", MS_NOSUID);
    mount_vfs("devpts","/dev/pts","devpts",MS_NOSUID | MS_NOEXEC);
    mount_vfs("tmpfs","/run","tmpfs",MS_NOSUID | MS_NODEV);
    mount_vfs("tmpfs","/tmp","tmpfs",MS_NOSUID | MS_NODEV);

    if (change_hn() != 0)
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
