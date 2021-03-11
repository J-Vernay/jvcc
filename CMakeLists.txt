cmake_minimum_required(VERSION 3.14) # needed for WINDOWS_EXPORT_ALL_SYMBOLS

project(jvcc)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

set(ALL_SOURCES
    src/StrView.c
    
    src/general/DynArray.c
    src/general/IO.c
    
    src/cli/jvcmd.c
    src/cli/main.c
)

add_executable(jvcc ${ALL_SOURCES})
target_include_directories(jvcc PUBLIC src)
target_compile_features(jvcc PUBLIC c_std_11)

# jvcc is also compiled as a shared library to be tested with Python
add_library(jvcc-shared SHARED ${ALL_SOURCES})
target_include_directories(jvcc-shared PUBLIC src)
target_compile_features(jvcc-shared PUBLIC c_std_11)
set_target_properties(jvcc-shared PROPERTIES
    ENABLE_EXPORTS ON
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)
