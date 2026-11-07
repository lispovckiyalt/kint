# kint

A minimal init (PID 1) for Linux, written in plain C.

## Features

- Mounts `/proc`, `/sys`, `/dev`, `/dev/pts`, `/run` and `/tmp`
- Reads the hostname from `/etc/hostname` (falls back to `localhost`)
- Disables instant reboot on Ctrl-Alt-Del; the kernel sends `SIGINT` instead
- Starts `/bin/bash` on `/dev/console` with a controlling tty and respawns it if it exits
- Reaps zombie processes
- Shuts down or reboots cleanly on signals

## Signals

| Signal | Action |
|---|---|
| `SIGTERM`, `SIGUSR1` | Power off |
| `SIGUSR2`, `SIGINT` | Reboot |
| `SIGCHLD` | Reap terminated children |

For example, from a shell: `kill -TERM 1`.

## Building

```sh
cmake -S . -B build
cmake --build build
```

## Testing in QEMU

```sh
mkdir -p rootfs/{bin,dev,proc,sys,run,tmp,etc}
cp init rootfs/init
cp /usr/bin/busybox rootfs/bin/        # must be a static busybox
(cd rootfs/bin && ./busybox --install -s . && ln -s busybox bash)
(cd rootfs && find . | cpio -o -H newc | gzip > ../initramfs.gz)

qemu-system-x86_64 -kernel /boot/vmlinuz-linux -initrd initramfs.gz \
    -append "console=ttyS0 rdinit=/int" -nographic -m 512
```

Use `rdinit=` when booting from an initramfs and `init=` when booting from a disk.

## Limitations

- No service configuration; only a single shell is supervised
- No getty or login
- Mount errors printed before the console is opened may be lost
- `/bin/bash` must exist together with its libraries, otherwise init will keep respawning it

## License

GPL-2.0-or-later. See [LICENSE](LICENSE).
