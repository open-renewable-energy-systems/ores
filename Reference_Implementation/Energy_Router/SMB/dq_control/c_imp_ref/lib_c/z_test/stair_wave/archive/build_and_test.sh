#!/bin/bash

# Exit on any error
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

echo "Building and testing the lookup table implementation..."

# Create build directory if it doesn't exist
mkdir -p build

# Compilation flags
CC=gcc
CFLAGS="-Wall -Wextra -O2"
INCLUDES="-I."
LIBS="-lm"

# Source files
SOURCES="
    switching_angles_reader.c
    lookup_table_5d.c
    test_main.c
"

# Compile each source file to object files
echo "Compiling source files..."
for source in $SOURCES; do
    object="build/$(basename ${source%.c}.o)"
    echo "  ${source} -> ${object}"
    $CC $CFLAGS $INCLUDES -c "$source" -o "$object"
done

# Link object files
echo "Linking..."
$CC $CFLAGS build/*.o -o build/test_program $LIBS

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo -e "${GREEN}Build successful!${NC}"
else
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

# Check if the lookup table exists
if [ ! -f "switching_angles_table.bin" ]; then
    echo -e "${RED}Error: switching_angles_table.bin not found!${NC}"
    echo "Please generate the lookup table first using the Python script."
    exit 1
fi

# Run the test program
echo -e "\nRunning tests..."
./build/test_program

# Check if test ran successfully
if [ $? -eq 0 ]; then
    echo -e "\n${GREEN}All tests completed successfully!${NC}"
else
    echo -e "\n${RED}Tests failed!${NC}"
    exit 1
fi