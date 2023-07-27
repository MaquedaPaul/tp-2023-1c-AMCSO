# Libraries
LIBS=commons shared pthread cspecs

# Custom libraries' paths
SHARED_LIBPATHS=../shared
STATIC_LIBPATHS=

# Compiler flags
CDEBUG=-g -Wno-unused-function -DDEBUG
CRELEASE=-O3 -Wall -DNDEBUG

# Arguments when executing with start, memcheck or helgrind
ARGS= ../pruebas/configs/BASE/cpu.config 0.0.0.0

# Valgrind flags
MEMCHECK_FLAGS=--track-origins=yes --log-file="memcheck.log"
HELGRIND_FLAGS=--log-file="helgrind.log"

# Source files (*.c) to be excluded from tests compilation
TEST_EXCLUDE=src/main.c
