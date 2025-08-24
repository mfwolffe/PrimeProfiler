#!/bin/bash

# Comprehensive Prime Algorithm Performance Analysis
# Matt Wolffe, James Madison University, 2025

echo "=========================================="
echo "Prime Algorithm Performance Analysis"
echo "Comprehensive Testing Across All Optimization Levels"
echo "=========================================="
echo ""

# Results directory
RESULTS_DIR="results"
mkdir -p "$RESULTS_DIR"

# Array of optimization levels to test
OPT_LEVELS=("opt0" "opt1" "opt2" "opt3" "optfast")

echo "Running profiling for all optimization levels..."
echo ""

# Run profiling for each optimization level
for opt in "${OPT_LEVELS[@]}"; do
    echo "----------------------------------------"
    echo "Testing $opt optimization level..."
    echo "----------------------------------------"
    
    output_file="$RESULTS_DIR/${opt}_results.out"
    
    # Run the profiling
    if ./scripts/ultimate_fixed_profiling.sh "$opt" "$output_file"; then
        echo "✓ $opt completed successfully"
    else
        echo "✗ $opt failed"
        continue
    fi
    echo ""
done

echo "=========================================="
echo "COMPREHENSIVE RESULTS SUMMARY"
echo "=========================================="
echo ""

# Create summary table
echo "Optimization Level Analysis:"
echo "============================"
printf "%-12s %-20s %-12s %-15s %-15s\n" "Opt Level" "Function" "Calls" "Instructions" "Avg Cyc/Call"
echo "--------------------------------------------------------------------------------"

for opt in "${OPT_LEVELS[@]}"; do
    result_file="$RESULTS_DIR/${opt}_results.out"
    if [[ -f "$result_file" ]]; then
        echo "[$opt]"
        # Extract the function performance data
        grep -A 3 "naive_prime.*27" "$result_file" | while read -r line; do
            if [[ "$line" =~ ^(naive_prime|less_naive_prime|naive_prime_squares) ]]; then
                # Parse the line to extract metrics
                func_name=$(echo "$line" | awk '{print $1}')
                calls=$(echo "$line" | awk '{print $2}')
                instructions=$(echo "$line" | awk '{print $3}')
                avg_cycles=$(echo "$line" | awk '{print $7}')
                printf "%-12s %-20s %-12s %-15s %-15s\n" "$opt" "$func_name" "$calls" "$instructions" "$avg_cycles"
            fi
        done
        echo ""
    else
        echo "[$opt] - No results available"
        echo ""
    fi
done

echo "=========================================="
echo "CSV EXPORT FOR ANALYSIS"
echo "=========================================="

# Create master CSV file
csv_file="$RESULTS_DIR/comprehensive_results.csv"
echo "optimization,function,calls,instructions,mem_reads,mem_writes,branches,total_cycles,avg_cycles_per_call" > "$csv_file"

for opt in "${OPT_LEVELS[@]}"; do
    result_file="$RESULTS_DIR/${opt}_results.out"
    if [[ -f "$result_file" ]]; then
        # Extract CSV data and prepend optimization level
        grep -A 3 "^less_naive_prime,\|^naive_prime,\|^naive_prime_squares," "$result_file" | \
        while IFS=',' read -r func calls instructions mem_reads mem_writes branches total_cycles avg_cycles; do
            if [[ "$func" =~ ^(less_naive_prime|naive_prime|naive_prime_squares)$ ]]; then
                echo "$opt,$func,$calls,$instructions,$mem_reads,$mem_writes,$branches,$total_cycles,$avg_cycles" >> "$csv_file"
            fi
        done
    fi
done

echo "Results exported to: $csv_file"
echo ""

# Performance ranking analysis
echo "=========================================="
echo "PERFORMANCE RANKING BY AVG CYCLES/CALL"
echo "=========================================="

if [[ -f "$csv_file" ]]; then
    echo "Best performing implementations (lower is better):"
    echo "------------------------------------------------"
    
    # Sort by avg cycles per call and display top performers
    tail -n +2 "$csv_file" | sort -t',' -k9 -n | head -10 | \
    while IFS=',' read -r opt func calls instructions mem_reads mem_writes branches total_cycles avg_cycles; do
        printf "%-8s %-20s %12s cycles/call\n" "$opt" "$func" "$avg_cycles"
    done
    
    echo ""
    echo "Worst performing implementations:"
    echo "--------------------------------"
    
    # Sort by avg cycles per call (descending) and show worst performers  
    tail -n +2 "$csv_file" | sort -t',' -k9 -nr | head -5 | \
    while IFS=',' read -r opt func calls instructions mem_reads mem_writes branches total_cycles avg_cycles; do
        printf "%-8s %-20s %12s cycles/call\n" "$opt" "$func" "$avg_cycles"
    done
fi

echo ""
echo "Analysis complete! Check $RESULTS_DIR/ for detailed results."