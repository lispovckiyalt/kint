#pragma once

#include <sys/types.h>

#include <unistd.h>

pid_t
spawn(const char* exe, char *const argv[]);

