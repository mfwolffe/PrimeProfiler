#!/bin/bash

# Fortran Profiling Script with Dynamic Address Detection
# Matt Wolffe, James Madison University, 2025

if [ $# -ne 2 ]; then
    echo "Usage: $0 <fortran_optimization_level> <output_file>"
    echo "Example: $0 fortran-opt3 fortran_opt3_results.out"
    exit 1
fi

OPT_LEVEL=$1
OUTPUT_FILE=$2
BINARY="./build/$OPT_LEVEL/main"

echo "==========================================="
echo "Fortran Prime Profiling: $OPT_LEVEL"
echo "==========================================="

# Check if binary exists
if [[ ! -f "$BINARY" ]]; then
    echo "ERROR: Binary $BINARY not found!"
    echo "Run: make -f makefiles/Makefile.$OPT_LEVEL"
    exit 1
fi

# Extract function addresses and sizes using objdump
echo "Extracting Fortran function addresses and sizes from $BINARY..."

# Get naive_prime_ (note the trailing underscore for Fortran)
NAIVE_LINE=$(objdump -t "$BINARY" | grep "g     F .text" | grep -E "[[:space:]]naive_prime_$")
NAIVE_ADDR=$(echo "$NAIVE_LINE" | awk '{print $1}')
NAIVE_SIZE=$(echo "$NAIVE_LINE" | awk '{print $5}')

# Get naive_prime_squares_
NAIVE_SQ_LINE=$(objdump -t "$BINARY" | grep "g     F .text" | grep "naive_prime_squares_$")
NAIVE_SQ_ADDR=$(echo "$NAIVE_SQ_LINE" | awk '{print $1}')
NAIVE_SQ_SIZE=$(echo "$NAIVE_SQ_LINE" | awk '{print $5}')

# Get less_naive_prime_
LESS_NAIVE_LINE=$(objdump -t "$BINARY" | grep "g     F .text" | grep "less_naive_prime_$")
LESS_NAIVE_ADDR=$(echo "$LESS_NAIVE_LINE" | awk '{print $1}')
LESS_NAIVE_SIZE=$(echo "$LESS_NAIVE_LINE" | awk '{print $5}')

echo "Extracted Fortran function information:"
echo "  naive_prime_:         0x$NAIVE_ADDR (size: 0x$NAIVE_SIZE)"
echo "  naive_prime_squares_: 0x$NAIVE_SQ_ADDR (size: 0x$NAIVE_SQ_SIZE)"  
echo "  less_naive_prime_:    0x$LESS_NAIVE_ADDR (size: 0x$LESS_NAIVE_SIZE)"

# Verify all functions found
if [[ -z "$NAIVE_ADDR" || -z "$NAIVE_SQ_ADDR" || -z "$LESS_NAIVE_ADDR" ]]; then
    echo "ERROR: Could not extract all Fortran function addresses!"
    echo "Debug info:"
    echo "naive_prime line: '$NAIVE_LINE'"
    echo "naive_sq line: '$NAIVE_SQ_LINE'"  
    echo "less_naive line: '$LESS_NAIVE_LINE'"
    exit 1
fi

# Set environment variables for the PIN tool (using Fortran mangled names)
export NAIVE_PRIME_ADDR="$NAIVE_ADDR"
export NAIVE_PRIME_SIZE="$NAIVE_SIZE"
export NAIVE_PRIME_SQUARES_ADDR="$NAIVE_SQ_ADDR"
export NAIVE_PRIME_SQUARES_SIZE="$NAIVE_SQ_SIZE"
export LESS_NAIVE_PRIME_ADDR="$LESS_NAIVE_ADDR"
export LESS_NAIVE_PRIME_SIZE="$LESS_NAIVE_SIZE"

# Set language identifier for PIN tool output
export LANGUAGE="Fortran"

echo ""
echo "Running PIN profiling with exact Fortran function ranges..."
~/pin/pin -t pin-tools/obj-intel64/prime-profiler-fortran.so -o "$OUTPUT_FILE" -- "$BINARY" >/dev/null 2>&1

if [[ $? -eq 0 && -f "$OUTPUT_FILE" ]]; then
    echo "✓ Fortran profiling completed successfully!"
    echo ""
    echo "Results summary:"
    echo "==============="
    tail -10 "$OUTPUT_FILE"
    echo ""
    echo "Full results saved to: $OUTPUT_FILE"
    
    # Show function detection status
    echo ""
    echo "Fortran function detection status:"
    echo "=================================="
    grep -E "ENV: (naive_prime|less_naive)" "$OUTPUT_FILE"
else
    echo "✗ Fortran profiling failed"
    if [[ -f "$OUTPUT_FILE" ]]; then
        echo "Partial output:"
        head -20 "$OUTPUT_FILE"
    fi
    exit 1
fi