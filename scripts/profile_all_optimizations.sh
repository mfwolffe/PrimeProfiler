#!/bin/bash

# Comprehensive Prime Algorithm Profiling Script
# Matt Wolffe, James Madison University, 2025

echo "=========================================="
echo "Prime Algorithm Optimization Profiler"
echo "=========================================="
echo "Testing all optimization levels..."
echo ""

# Configuration
PIN_TOOL="pin-tools/obj-intel64/prime-comprehensive.so"
PIN_BIN="$HOME/pin/pin"
RESULTS_DIR="results"

# Create results directory
mkdir -p "$RESULTS_DIR"

# Optimization levels to test
declare -a OPT_LEVELS=("opt0" "opt1" "opt2" "opt3" "optfast")
declare -a OPT_NAMES=("O0 (Debug)" "O1 (Basic)" "O2 (Standard)" "O3 (Heavy)" "Ofast (Aggressive)")

echo "Cleaning previous results..."
rm -f "$RESULTS_DIR"/*.out "$RESULTS_DIR"/*.csv "$RESULTS_DIR"/summary_*.txt

echo "Starting profiling runs..."
echo ""

# Run profiling for each optimization level
for i in "${!OPT_LEVELS[@]}"; do
    OPT="${OPT_LEVELS[$i]}"
    NAME="${OPT_NAMES[$i]}"
    
    echo "[$((i+1))/${#OPT_LEVELS[@]}] Profiling $NAME ($OPT)..."
    
    # Check if binary exists
    if [[ ! -f "build/$OPT/main" ]]; then
        echo "  ERROR: Binary build/$OPT/main not found! Skipping..."
        continue
    fi
    
    # Run PIN profiling
    OUTPUT_FILE="$RESULTS_DIR/${OPT}_profile.out"
    
    echo "  Running: $PIN_BIN -t $PIN_TOOL -o $OUTPUT_FILE -- ./build/$OPT/main"
    
    if $PIN_BIN -t "$PIN_TOOL" -o "$OUTPUT_FILE" -- "./build/$OPT/main" > /dev/null 2>&1; then
        echo "  ✓ Completed successfully"
        
        # Extract CSV data for summary
        if grep -q "CSV DATA" "$OUTPUT_FILE"; then
            tail -n +$(grep -n "CSV DATA" "$OUTPUT_FILE" | tail -1 | cut -d: -f1) "$OUTPUT_FILE" | tail -n +3 > "$RESULTS_DIR/${OPT}.csv"
            echo "  ✓ CSV data extracted"
        else
            echo "  ⚠ Warning: No CSV data found in output"
        fi
    else
        echo "  ✗ Failed to run profiling"
    fi
    
    echo ""
done

echo "=========================================="
echo "Generating Summary Reports..."
echo "=========================================="

# Generate comprehensive summary
SUMMARY_FILE="$RESULTS_DIR/optimization_summary.txt"

cat > "$SUMMARY_FILE" << EOF
Prime Algorithm Profiling Results
Generated: $(date)
========================================

OPTIMIZATION LEVEL COMPARISON:
EOF

echo "" >> "$SUMMARY_FILE"
printf "%-15s %-12s %-15s %-15s %-15s\n" "Optimization" "Function" "Instructions" "Calls" "Avg Cyc/Call" >> "$SUMMARY_FILE"
printf "%s\n" "$(printf '%.0s-' {1..80})" >> "$SUMMARY_FILE"

# Process each optimization level
for OPT in "${OPT_LEVELS[@]}"; do
    if [[ -f "$RESULTS_DIR/${OPT}.csv" ]]; then
        while IFS=',' read -r func calls instrs mem_r mem_w branches total_cyc avg_cyc; do
            if [[ "$func" != "function" ]]; then  # Skip header
                printf "%-15s %-12s %-15s %-15s %-15s\n" "$OPT" "$func" "$instrs" "$calls" "$avg_cyc" >> "$SUMMARY_FILE"
            fi
        done < "$RESULTS_DIR/${OPT}.csv"
    else
        printf "%-15s %-12s %-15s %-15s %-15s\n" "$OPT" "ERROR" "No data" "-" "-" >> "$SUMMARY_FILE"
    fi
done

# Create efficiency comparison
echo "" >> "$SUMMARY_FILE"
echo "EFFICIENCY RATIOS (Instructions Executed):" >> "$SUMMARY_FILE"
echo "==========================================" >> "$SUMMARY_FILE"

for OPT in "${OPT_LEVELS[@]}"; do
    if [[ -f "$RESULTS_DIR/${OPT}.csv" ]]; then
        echo "" >> "$SUMMARY_FILE"
        echo "$OPT Results:" >> "$SUMMARY_FILE"
        
        # Extract instruction counts
        naive=$(grep "^naive_prime," "$RESULTS_DIR/${OPT}.csv" | cut -d, -f3 2>/dev/null || echo "0")
        squares=$(grep "^naive_prime_squares," "$RESULTS_DIR/${OPT}.csv" | cut -d, -f3 2>/dev/null || echo "0")
        less_naive=$(grep "^less_naive_prime," "$RESULTS_DIR/${OPT}.csv" | cut -d, -f3 2>/dev/null || echo "0")
        
        if [[ "$less_naive" -gt 0 ]]; then
            naive_ratio=$(echo "scale=2; $naive / $less_naive" | bc 2>/dev/null || echo "N/A")
            squares_ratio=$(echo "scale=2; $squares / $less_naive" | bc 2>/dev/null || echo "N/A")
        else
            naive_ratio="N/A"
            squares_ratio="N/A"
        fi
        
        printf "  naive_prime:         %15s instructions (%.1fx vs less_naive)\n" "$naive" "$naive_ratio" >> "$SUMMARY_FILE"
        printf "  naive_prime_squares: %15s instructions (%.1fx vs less_naive)\n" "$squares" "$squares_ratio" >> "$SUMMARY_FILE"
        printf "  less_naive_prime:    %15s instructions (baseline)\n" "$less_naive" >> "$SUMMARY_FILE"
    fi
done

# Generate CSV comparison file
CSV_SUMMARY="$RESULTS_DIR/comparison.csv"
echo "optimization,function,calls,instructions,mem_reads,mem_writes,branches,total_cycles,avg_cycles_per_call" > "$CSV_SUMMARY"

for OPT in "${OPT_LEVELS[@]}"; do
    if [[ -f "$RESULTS_DIR/${OPT}.csv" ]]; then
        tail -n +2 "$RESULTS_DIR/${OPT}.csv" | sed "s/^/$OPT,/" >> "$CSV_SUMMARY"
    fi
done

echo ""
echo "=========================================="
echo "PROFILING COMPLETE!"
echo "=========================================="
echo ""
echo "Results saved to:"
echo "  • Individual profiles: $RESULTS_DIR/*_profile.out"
echo "  • Summary report:      $SUMMARY_FILE"
echo "  • CSV comparison:      $CSV_SUMMARY"
echo ""
echo "Key findings preview:"
if [[ -f "$SUMMARY_FILE" ]]; then
    echo "----------------------------------------"
    tail -n 20 "$SUMMARY_FILE"
    echo "----------------------------------------"
fi
echo ""
echo "To view full results:"
echo "  cat $SUMMARY_FILE"
echo "  cat $CSV_SUMMARY"
echo ""