#!/bin/sh
./assembler tests/test2.z80
hexdump -C file.rom
rm file.rom
