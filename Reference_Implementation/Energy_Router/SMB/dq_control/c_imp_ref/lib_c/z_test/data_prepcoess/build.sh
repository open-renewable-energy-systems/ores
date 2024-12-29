#!/bin/bash

# Directory setup
CURR_DIR=$(pwd)
PROJECT_ROOT="../../.."
LIB_DIR="$PROJECT_ROOT/lib_c/log_data_rw"
OUTPUT_DIR="output"
TEST_DATA="test_data.txt"  # Default test data file

# Debug: Print paths
echo "Current directory: $CURR_DIR"
echo "Library directory: $LIB_DIR"

# Create output directory if it doesn't exist
mkdir -p $OUTPUT_DIR

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Verify library files exist
if [ ! -f "$LIB_DIR/log_data_rw.h" ] || [ ! -f "$LIB_DIR/log_data_rw.c" ]; then
    echo -e "${RED}Error: Library files not found in $LIB_DIR${NC}"
    echo "Expected files:"
    echo "$LIB_DIR/log_data_rw.h"
    echo "$LIB_DIR/log_data_rw.c"
    exit 1
fi

# Compile the C program
echo -e "${GREEN}Compiling C program...${NC}"
gcc -o process_data \
    main.c \
    "$LIB_DIR/log_data_rw.c" \
    -I"$LIB_DIR" \
    -lm \
    -Wall

if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation failed!${NC}"
    exit 1
fi

# Check if input file is provided
if [ $# -eq 0 ]; then
    # Check if default test data exists
    if [ -f "$TEST_DATA" ]; then
        echo -e "${GREEN}Using default test data file: $TEST_DATA${NC}"
        INPUT_FILE=$TEST_DATA
    else
        echo -e "${RED}No input file provided and no default test data found.${NC}"
        echo "Usage: $0 <input_log_file>"
        echo "Please provide an input log file or create a test_data.txt file"
        exit 1
    fi
else
    INPUT_FILE=$1
fi

# Verify input file exists
if [ ! -f "$INPUT_FILE" ]; then
    echo -e "${RED}Error: Input file '$INPUT_FILE' not found${NC}"
    exit 1
fi

OUTPUT_CSV="$OUTPUT_DIR/processed_data.csv"

# Run the C program
echo -e "${GREEN}Processing data...${NC}"
./process_data "$INPUT_FILE" "$OUTPUT_CSV"

if [ $? -ne 0 ]; then
    echo -e "${RED}Data processing failed!${NC}"
    exit 1
fi

# Check if Python and required packages are installed
if ! command -v python3 &> /dev/null; then
    echo -e "${RED}Python3 is not installed!${NC}"
    exit 1
fi

# Check for required Python packages
python3 -c "import pandas; import matplotlib" 2>/dev/null
if [ $? -ne 0 ]; then
    echo -e "${GREEN}Installing required Python packages...${NC}"
    pip install pandas matplotlib
fi

# Run the plotting script
echo -e "${GREEN}Generating plots...${NC}"
python3 plot_results.py "$OUTPUT_CSV"

if [ $? -eq 0 ]; then
    echo -e "${GREEN}Process completed successfully!${NC}"
else
    echo -e "${RED}Plot generation failed!${NC}"
    exit 1
fi