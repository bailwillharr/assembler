cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall)

project(assembler VERSION 0.2)
add_executable(assembler src/parseline.c src/symtable.c src/util.c src/assemble.c src/main.c)

install(TARGETS assembler DESTINATION bin)
