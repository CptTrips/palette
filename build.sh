#!/bin/bash

# compile object files
# -c prevents linking (n/a here) and builds to [fname].o
g++ -c palette.cc

# Add object code to archive
ar rcs libpalette.a palette.o

# compile main object
g++ -c main.cc

# link library to main
g++ -o palette -L./ -lpalette main.o

