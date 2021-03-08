cmake_minimum_required(VERSION 3.1)

project(jvcc)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall -Wextra")

add_executable(jvcc
    src/StrView.c
    
    src/general/DynArray.c
    src/general/IO.c
    
    src/cli/jvcmd.c
    src/cli/main.c
)
target_include_directories(jvcc PUBLIC src)
