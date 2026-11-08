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

#include <sys/mount.h>

#include "mount.h"

void
mount_allvfs()
{
    mount_vfs("proc", "/proc", "proc", MS_NOSUID | MS_NODEV | MS_NOEXEC);
    mount_vfs("sysfs", "/sys", "sysfs", MS_NOSUID | MS_NODEV | MS_NOEXEC);
    mount_vfs("devtmpfs", "/dev", "devtmpfs", MS_NOSUID);
    mount_vfs("devpts","/dev/pts","devpts",MS_NOSUID | MS_NOEXEC);
    mount_vfs("tmpfs","/run","tmpfs",MS_NOSUID | MS_NODEV);
    mount_vfs("tmpfs","/tmp","tmpfs",MS_NOSUID | MS_NODEV);
}
