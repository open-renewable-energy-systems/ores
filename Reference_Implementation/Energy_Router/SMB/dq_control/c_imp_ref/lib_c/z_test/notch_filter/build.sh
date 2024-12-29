#!/bin/bash

# Directory structure (relative to test/notch_filter)
CURRENT_DIR="$(pwd)"
PROJECT_ROOT="$(cd ../../.. && pwd)"      # Go up three levels to project root
LIB_C_DIR="$PROJECT_ROOT/lib_c"           # Explicitly set lib_c directory
NOTCH_DIR="$LIB_C_DIR/notch_filter"      # Path to notch filter directory

# Create build directory if it doesn't exist
BUILD_DIR="$CURRENT_DIR/build"
mkdir -p $BUILD_DIR

# Print directories for debugging
echo "Current directory: $CURRENT_DIR"
echo "Project root: $PROJECT_ROOT"
echo "Lib_c directory: $LIB_C_DIR"
echo "Notch filter directory: $NOTCH_DIR"
echo "Build directory: $BUILD_DIR"

# Compiler settings
CC=gcc
CFLAGS="-Wall -Wextra -O2 -I$NOTCH_DIR"
LDFLAGS="-lm"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Function to print status
print_status() {
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ $1${NC}"
    else
        echo -e "${RED}✗ $1${NC}"
        exit 1
    fi
}

# Clean build directory
echo "Cleaning build directory..."
rm -rf $BUILD_DIR/*
print_status "Clean completed"

# Compile notch_filter.c
echo "Compiling notch_filter.c..."
$CC $CFLAGS -c "$NOTCH_DIR/notch_filter.c" -o "$BUILD_DIR/notch_filter.o"
print_status "Compiled notch_filter.c"

# Compile main.c
echo "Compiling main.c..."
$CC $CFLAGS -c "$CURRENT_DIR/main.c" -o "$BUILD_DIR/main.o"
print_status "Compiled main.c"

# Link
echo "Linking..."
OBJECTS="$BUILD_DIR/notch_filter.o $BUILD_DIR/main.o"
echo "Object files: $OBJECTS"

# Verify object files exist
for obj in $OBJECTS; do
    if [ ! -f "$obj" ]; then
        echo -e "${RED}Error: Object file not found: $obj${NC}"
        exit 1
    fi
done

$CC $OBJECTS -o "$BUILD_DIR/notch_filter_test" $LDFLAGS
print_status "Linking completed"

# After successful linking
if [ $? -eq 0 ]; then
    echo -e "\nRunning tests..."
    "$BUILD_DIR/notch_filter_test"
    
    if [ $? -eq 0 ]; then
        echo -e "\nCreating plots..."
        if command -v python3 &> /dev/null; then
            python3 plot_results.py
            if [ $? -eq 0 ]; then
                echo -e "${GREEN}✓ Tests run and plots created successfully${NC}"
                echo "Plot file: filter_results.png"
            else
                echo -e "${RED}✗ Error creating plots${NC}"
                exit 1
            fi
        else
            echo -e "${RED}✗ Python3 not found. Please install Python3 to generate plots.${NC}"
            exit 1
        fi
    else
        echo -e "${RED}✗ Error running tests${NC}"
        exit 1
    fi
fi

echo -e "\nBuild process completed!"