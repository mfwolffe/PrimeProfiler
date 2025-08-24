#!/bin/bash

# Comprehensive C vs Fortran Prime Algorithm Comparison
# Matt Wolffe, James Madison University, 2025

echo "======================================================="
echo "C vs Fortran Prime Algorithm Performance Comparison"
echo "Comprehensive Testing Across Multiple Optimization Levels"
echo "======================================================="
echo ""

# Results directory
RESULTS_DIR="results/c_vs_fortran"
mkdir -p "$RESULTS_DIR"

# Optimization levels to test
C_OPT_LEVELS=("opt0" "opt3" "optfast")
FORTRAN_OPT_LEVELS=("fortran-opt0" "fortran-opt3" "fortran-optfast")

echo "Building all required binaries..."
echo "================================"

# Build C binaries
for opt in "${C_OPT_LEVELS[@]}"; do
    echo "Building C $opt..."
    make -f makefiles/Makefile.$opt >/dev/null 2>&1
    if [[ $? -ne 0 ]]; then
        echo "✗ Failed to build C $opt"
        exit 1
    fi
    echo "✓ C $opt built"
done

# Build Fortran binaries
for opt in "${FORTRAN_OPT_LEVELS[@]}"; do
    echo "Building Fortran $opt..."
    make -f makefiles/Makefile.$opt >/dev/null 2>&1
    if [[ $? -ne 0 ]]; then
        echo "✗ Failed to build Fortran $opt"
        exit 1
    fi
    echo "✓ Fortran $opt built"
done

echo ""
echo "Running C profiling..."
echo "====================="

# Profile C implementations
for opt in "${C_OPT_LEVELS[@]}"; do
    echo "Profiling C $opt..."
    output_file="$RESULTS_DIR/c_${opt}_results.out"
    
    if ./scripts/profile_single_optimization.sh "$opt" "$output_file"; then
        echo "✓ C $opt profiling completed"
    else
        echo "✗ C $opt profiling failed"
    fi
done

echo ""
echo "Running Fortran profiling..."
echo "============================"

# Profile Fortran implementations
for opt in "${FORTRAN_OPT_LEVELS[@]}"; do
    echo "Profiling Fortran $opt..."
    output_file="$RESULTS_DIR/fortran_${opt}_results.out"
    
    if ./scripts/profile_fortran_optimization.sh "$opt" "$output_file"; then
        echo "✓ Fortran $opt profiling completed"
    else
        echo "✗ Fortran $opt profiling failed"
    fi
done

echo ""
echo "======================================================="
echo "CROSS-LANGUAGE PERFORMANCE COMPARISON"
echo "======================================================="

# Create master comparison CSV
comparison_csv="$RESULTS_DIR/c_vs_fortran_comparison.csv"
echo "language,optimization,function,calls,instructions,mem_reads,mem_writes,branches,total_cycles,avg_cycles_per_call" > "$comparison_csv"

# Extract and combine results
echo "Language   Optimization  Function            Avg Cycles/Call   Instructions   Branches"
echo "========================================================================================"

# Process C results
for opt in "${C_OPT_LEVELS[@]}"; do
    result_file="$RESULTS_DIR/c_${opt}_results.out"
    if [[ -f "$result_file" ]]; then
        grep -A 3 "^less_naive_prime,\|^naive_prime,\|^naive_prime_squares," "$result_file" | \
        while IFS=',' read -r func calls instructions mem_reads mem_writes branches total_cycles avg_cycles; do
            if [[ "$func" =~ ^(less_naive_prime|naive_prime|naive_prime_squares)$ ]]; then
                echo "C,$opt,$func,$calls,$instructions,$mem_reads,$mem_writes,$branches,$total_cycles,$avg_cycles" >> "$comparison_csv"
                printf "%-10s %-12s %-20s %12s %12s %10s\n" "C" "$opt" "$func" "$avg_cycles" "$instructions" "$branches"
            fi
        done
    fi
done

# Process Fortran results  
for opt in "${FORTRAN_OPT_LEVELS[@]}"; do
    result_file="$RESULTS_DIR/fortran_${opt}_results.out"
    if [[ -f "$result_file" ]]; then
        grep -A 3 "^less_naive_prime,\|^naive_prime,\|^naive_prime_squares," "$result_file" | \
        while IFS=',' read -r func calls instructions mem_reads mem_writes branches total_cycles avg_cycles; do
            if [[ "$func" =~ ^(less_naive_prime|naive_prime|naive_prime_squares)$ ]]; then
                echo "Fortran,$opt,$func,$calls,$instructions,$mem_reads,$mem_writes,$branches,$total_cycles,$avg_cycles" >> "$comparison_csv"
                # Convert fortran optimization names for display
                display_opt=$(echo "$opt" | sed 's/fortran-//')
                printf "%-10s %-12s %-20s %12s %12s %10s\n" "Fortran" "$display_opt" "$func" "$avg_cycles" "$instructions" "$branches"
            fi
        done
    fi
done

echo ""
echo "======================================================="
echo "KEY FINDINGS SUMMARY"
echo "======================================================="

if [[ -f "$comparison_csv" ]]; then
    echo "Fastest implementations by algorithm (lower cycles/call = better):"
    echo "=================================================================="
    
    # Find best performers for each algorithm
    for algorithm in "less_naive_prime" "naive_prime" "naive_prime_squares"; do
        echo ""
        echo "$algorithm winners:"
        echo "$(printf '%-10s %-12s %15s' 'Language' 'Optimization' 'Cycles/Call')"
        echo "----------------------------------------"
        
        tail -n +2 "$comparison_csv" | grep ",$algorithm," | sort -t',' -k9 -n | head -3 | \
        while IFS=',' read -r lang opt func calls instructions mem_reads mem_writes branches total_cycles avg_cycles; do
            display_opt=$(echo "$opt" | sed 's/fortran-//')
            printf "%-10s %-12s %15s\n" "$lang" "$display_opt" "$avg_cycles"
        done
    done
    
    echo ""
    echo "Performance ratios (C baseline = 1.0):"
    echo "======================================"
    
    # Calculate performance ratios for opt3 
    c_opt3_less_naive=$(tail -n +2 "$comparison_csv" | grep "C,opt3,less_naive_prime," | cut -d',' -f9)
    c_opt3_naive=$(tail -n +2 "$comparison_csv" | grep "C,opt3,naive_prime," | cut -d',' -f9)
    c_opt3_squares=$(tail -n +2 "$comparison_csv" | grep "C,opt3,naive_prime_squares," | cut -d',' -f9)
    
    fortran_opt3_less_naive=$(tail -n +2 "$comparison_csv" | grep "Fortran,fortran-opt3,less_naive_prime," | cut -d',' -f9)
    fortran_opt3_naive=$(tail -n +2 "$comparison_csv" | grep "Fortran,fortran-opt3,naive_prime," | cut -d',' -f9)
    fortran_opt3_squares=$(tail -n +2 "$comparison_csv" | grep "Fortran,fortran-opt3,naive_prime_squares," | cut -d',' -f9)
    
    if [[ -n "$c_opt3_less_naive" && -n "$fortran_opt3_less_naive" ]]; then
        ratio=$(echo "scale=2; $c_opt3_less_naive / $fortran_opt3_less_naive" | bc -l 2>/dev/null || echo "N/A")
        echo "less_naive_prime (opt3):   C=1.00, Fortran=${ratio}x"
    fi
    
    if [[ -n "$c_opt3_naive" && -n "$fortran_opt3_naive" ]]; then
        ratio=$(echo "scale=2; $c_opt3_naive / $fortran_opt3_naive" | bc -l 2>/dev/null || echo "N/A")
        echo "naive_prime (opt3):        C=1.00, Fortran=${ratio}x"
    fi
    
    if [[ -n "$c_opt3_squares" && -n "$fortran_opt3_squares" ]]; then
        ratio=$(echo "scale=2; $c_opt3_squares / $fortran_opt3_squares" | bc -l 2>/dev/null || echo "N/A")
        echo "naive_prime_squares (opt3): C=1.00, Fortran=${ratio}x"
    fi
fi

echo ""
echo "Detailed results saved to: $RESULTS_DIR/"
echo "Master comparison CSV: $comparison_csv"
echo ""
echo "Analysis complete! 🚀"