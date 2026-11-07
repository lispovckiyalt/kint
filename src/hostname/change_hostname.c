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
#include <unistd.h>
#include <limits.h>

#include "hostname.h"

int
change_hostname()
{
    char buffer[HOST_NAME_MAX];

    FILE * etchostname = fopen("/etc/hostname", "r");

    if (etchostname == 0x0) { return 1; }

    if (fgets(buffer, sizeof(buffer), etchostname) == NULL) { fclose(etchostname); return 1;}

    fclose(etchostname);

    size_t leng = strlen(buffer);
    
    if (leng > 0 && buffer[leng - 1] == '\n') 
    {
        buffer[leng - 1] = '\0';
        leng--;
    }

    if (leng == 0) { return 1; }

    if (sethostname(buffer, leng) == 0) { return 0; }
    else { return 1; }
}
