#!/bin/bash

echo "=== Prime Algorithm Performance Comparison ==="
echo "Testing hypothesis: Do compiler optimizations negate algorithmic improvements?"
echo ""

# Create results directory
mkdir -p results

# Test each optimization level
for opt_level in opt0 opt1 opt2 opt3 optfast; do
    echo "Testing $opt_level optimization level..."
    
    # Time the execution
    echo "  Running timing analysis..."
    
    # Run multiple times and average
    total_time=0
    for i in {1..5}; do
        start_time=$(date +%s.%N)
        ./build/$opt_level/main > /dev/null
        end_time=$(date +%s.%N)
        run_time=$(echo "$end_time - $start_time" | bc -l)
        total_time=$(echo "$total_time + $run_time" | bc -l)
    done
    
    avg_time=$(echo "scale=6; $total_time / 5" | bc -l)
    
    echo "  Real time: ${real_time}s, User time: ${user_time}s, System time: ${sys_time}s"
    
    # Save results
    echo "$opt_level,$real_time,$user_time,$sys_time" >> results/timing_results.csv
    
    echo ""
done

echo "Results saved to results/timing_results.csv"
echo ""
echo "=== Analysis ==="
echo "Optimization,Real_Time,User_Time,System_Time"
cat results/timing_results.csv

echo ""
echo "Next steps:"
echo "1. Install GCC 9-12 for Pin tool compilation"  
echo "2. Build custom Pin tool for detailed instruction counting"
echo "3. Analyze instruction patterns per algorithm"
echo "4. Compare algorithmic vs compiler optimization benefits"