cmake_minimum_required(VERSION 3.10)

project(ASSEMBLER VERSION 1.0)
add_executable(assembler src/parseline.c src/symtable.c src/util.c src/assemble.c src/main.c)

target_compile_options(assembler PRIVATE -Wall)

set_target_properties(assembler PROPERTIES
	C_STANDARD 99
	C_STANDARD_REQUIRED ON)

install(TARGETS assembler DESTINATION bin)
