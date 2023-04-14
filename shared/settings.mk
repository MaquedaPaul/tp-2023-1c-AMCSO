# Libraries
LIBS=commons pthread

# Custom libraries' paths
SHARED_LIBPATHS=
STATIC_LIBPATHS=

# Compiler flags
CDEBUG=-g -w -Wno-unused-function -DDEBUG
CRELEASE=-O3 -w -Wno-unused-function -DNDEBUG