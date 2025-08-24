#!/bin/bash

# Ultimate Fixed Profiling Script
# Matt Wolffe, James Madison University, 2025

if [ $# -ne 2 ]; then
    echo "Usage: $0 <optimization_level> <output_file>"
    echo "Example: $0 opt3 opt3_ultimate.out"
    exit 1
fi

OPT_LEVEL=$1
OUTPUT_FILE=$2
BINARY="./build/$OPT_LEVEL/main"

echo "==========================================="
echo "Ultimate Fixed Prime Profiling: $OPT_LEVEL"
echo "==========================================="

# Check if binary exists
if [[ ! -f "$BINARY" ]]; then
    echo "ERROR: Binary $BINARY not found!"
    exit 1
fi

# Extract function info with precise parsing
echo "Extracting function information..."

# Get naive_prime (but not less_naive_prime or naive_prime_squares)
NAIVE_LINE=$(objdump -t "$BINARY" | grep "g     F .text" | grep -E "[[:space:]]naive_prime$")
NAIVE_ADDR=$(echo "$NAIVE_LINE" | awk '{print $1}')
NAIVE_SIZE=$(echo "$NAIVE_LINE" | awk '{print $5}')

# Get naive_prime_squares
NAIVE_SQ_LINE=$(objdump -t "$BINARY" | grep "g     F .text" | grep "naive_prime_squares$")
NAIVE_SQ_ADDR=$(echo "$NAIVE_SQ_LINE" | awk '{print $1}')
NAIVE_SQ_SIZE=$(echo "$NAIVE_SQ_LINE" | awk '{print $5}')

# Get less_naive_prime
LESS_NAIVE_LINE=$(objdump -t "$BINARY" | grep "g     F .text" | grep "less_naive_prime$")
LESS_NAIVE_ADDR=$(echo "$LESS_NAIVE_LINE" | awk '{print $1}')
LESS_NAIVE_SIZE=$(echo "$LESS_NAIVE_LINE" | awk '{print $5}')

echo "Extracted function information:"
echo "  naive_prime:         0x$NAIVE_ADDR (size: 0x$NAIVE_SIZE)"
echo "  naive_prime_squares: 0x$NAIVE_SQ_ADDR (size: 0x$NAIVE_SQ_SIZE)"  
echo "  less_naive_prime:    0x$LESS_NAIVE_ADDR (size: 0x$LESS_NAIVE_SIZE)"

# Verify all functions found
if [[ -z "$NAIVE_ADDR" || -z "$NAIVE_SQ_ADDR" || -z "$LESS_NAIVE_ADDR" ]]; then
    echo "ERROR: Could not extract all function addresses!"
    echo "Debug info:"
    echo "naive_prime line: '$NAIVE_LINE'"
    echo "naive_sq line: '$NAIVE_SQ_LINE'"  
    echo "less_naive line: '$LESS_NAIVE_LINE'"
    exit 1
fi

# Set environment variables for the PIN tool
export NAIVE_PRIME_ADDR="$NAIVE_ADDR"
export NAIVE_PRIME_SIZE="$NAIVE_SIZE"
export NAIVE_PRIME_SQUARES_ADDR="$NAIVE_SQ_ADDR"
export NAIVE_PRIME_SQUARES_SIZE="$NAIVE_SQ_SIZE"
export LESS_NAIVE_PRIME_ADDR="$LESS_NAIVE_ADDR"
export LESS_NAIVE_PRIME_SIZE="$LESS_NAIVE_SIZE"

echo ""
echo "Running PIN profiling with exact function ranges..."
~/pin/pin -t pin-tools/obj-intel64/prime-profiler.so -o "$OUTPUT_FILE" -- "$BINARY" >/dev/null 2>&1

if [[ $? -eq 0 && -f "$OUTPUT_FILE" ]]; then
    echo "✓ Profiling completed successfully!"
    echo ""
    echo "Results summary:"
    echo "==============="
    tail -10 "$OUTPUT_FILE"
    echo ""
    echo "Full results saved to: $OUTPUT_FILE"
    
    # Show if all functions were properly detected
    echo ""
    echo "Function detection status:"
    echo "=========================="
    grep -E "ENV: (naive_prime|less_naive)" "$OUTPUT_FILE"
else
    echo "✗ Profiling failed"
    if [[ -f "$OUTPUT_FILE" ]]; then
        echo "Partial output:"
        head -20 "$OUTPUT_FILE"
    fi
    exit 1
fi