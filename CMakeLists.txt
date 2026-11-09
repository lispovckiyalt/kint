cmake_minimum_required(VERSION 3.10)
project(kint C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
include_directories(inc)
find_package(Threads REQUIRED)

set(SOURCES
    src/main.c

    src/hostname/change_hostname.c


    src/mount/mount_vfs.c
    src/mount/mount_allvfs.c

    src/spawn/spawn.c
)

add_executable(kint ${SOURCES})

target_compile_options(kint PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wconversion
    -fsanitize=address,undefined
)

target_link_libraries(kint PRIVATE
    Threads::Threads
    -fsanitize=address,undefined
)


install(TARGETS kint RUNTIME DESTINATION bin)
