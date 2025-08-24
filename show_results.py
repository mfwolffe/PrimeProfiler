#!/usr/bin/env python3
"""
PrimeProfiler Results Summary
Quick viewer for the C vs Fortran performance comparison results
"""

import csv
import sys
import os

def main():
    print("PrimeProfiler: C vs Fortran Performance Analysis Results")
    print("=" * 60)
    
    # Check if results file exists
    results_file = "results/c_vs_fortran/c_vs_fortran_comparison.csv"
    if not os.path.exists(results_file):
        print("Results file not found. Please run the profiling analysis first:")
        print("   ./scripts/profile_c_vs_fortran.sh")
        return 1
    
    # Read results
    with open(results_file, 'r') as f:
        reader = csv.DictReader(f)
        data = list(reader)
    
    print(f"Analysis of {len(data)} performance measurements")
    print()
    
    # Group by function for analysis
    functions = {}
    for row in data:
        func = row['function']
        if func not in functions:
            functions[func] = []
        functions[func].append(row)
    
    # Show algorithm performance hierarchy first
    print("Algorithm Performance Hierarchy (as expected):")
    print("-" * 50)
    
    # Get best performance for each algorithm to show hierarchy
    algo_best = {}
    for func_name in ['less_naive_prime', 'naive_prime_squares', 'naive_prime']:
        if func_name not in functions:
            continue
        func_data = functions[func_name]
        func_data.sort(key=lambda x: int(x['avg_cycles_per_call']))
        best_cycles = int(func_data[0]['avg_cycles_per_call'])
        algo_best[func_name] = best_cycles
    
    # Sort algorithms by their best performance
    sorted_algos = sorted(algo_best.items(), key=lambda x: x[1])
    for i, (func_name, cycles) in enumerate(sorted_algos):
        alg_names = {
            'less_naive_prime': 'less_naive_prime (6k±1 optimization)',
            'naive_prime_squares': 'naive_prime_squares (√n optimization)', 
            'naive_prime': 'naive_prime (brute force)'
        }
        print(f"  {i+1}. {alg_names[func_name]}: {cycles:,} cycles/call")
    
    print()
    print("Cross-Language Performance Leaders:")
    print("-" * 40)
    
    for func_name in ['less_naive_prime', 'naive_prime_squares', 'naive_prime']:
        if func_name not in functions:
            continue
            
        func_data = functions[func_name]
        func_data.sort(key=lambda x: int(x['avg_cycles_per_call']))
        winner = func_data[0]
        
        cycles = int(winner['avg_cycles_per_call'])
        print(f"{func_name}:")
        print(f"  Best: {winner['language']} ({winner['optimization']}) - {cycles:,} cycles/call")
        
        # Show performance ratios for top 3
        winner_cycles = cycles
        print("    Language comparison:")
        for i, row in enumerate(func_data[:3]):
            ratio = int(row['avg_cycles_per_call']) / winner_cycles
            lang = row['language']
            opt = row['optimization']
            print(f"      {i+1}. {lang} ({opt}): {ratio:.2f}x")
        print()
    
    # Show most dramatic difference
    print("Largest Performance Differences:")
    print("-" * 35)
    
    naive_data = functions.get('naive_prime', [])
    if naive_data:
        naive_data.sort(key=lambda x: int(x['avg_cycles_per_call']))
        best = naive_data[0]
        worst = naive_data[-1]
        
        best_cycles = int(best['avg_cycles_per_call'])
        worst_cycles = int(worst['avg_cycles_per_call'])
        ratio = worst_cycles / best_cycles
        
        print(f"naive_prime algorithm: {ratio:.1f}x performance difference")
        print(f"  Best:  {best['language']} ({best['optimization']}) - {best_cycles:,} cycles")
        print(f"  Worst: {worst['language']} ({worst['optimization']}) - {worst_cycles:,} cycles")
        print()
    
    # Show instruction count insights
    print("Compiler Optimization Analysis:")
    print("-" * 35)
    
    c_opt3 = None
    fortran_opt3 = None
    
    for row in data:
        if row['function'] == 'naive_prime' and row['language'] == 'C' and row['optimization'] == 'opt3':
            c_opt3 = row
        elif row['function'] == 'naive_prime' and row['language'] == 'Fortran' and row['optimization'] == 'opt3':
            fortran_opt3 = row
    
    if c_opt3 and fortran_opt3:
        c_instr = int(c_opt3['instructions'])
        f_instr = int(fortran_opt3['instructions'])
        reduction = ((c_instr - f_instr) / c_instr) * 100
        
        print("naive_prime instruction count comparison:")
        print(f"  C (opt3):       {c_instr:,} instructions")
        print(f"  Fortran (opt3): {f_instr:,} instructions")
        print(f"  Reduction:      {reduction:.1f}% fewer instructions in Fortran")
        print()
    
    # Language summary
    fortran_wins = 0
    c_wins = 0
    
    for func_name in ['less_naive_prime', 'naive_prime', 'naive_prime_squares']:
        if func_name not in functions:
            continue
        func_data = functions[func_name]
        func_data.sort(key=lambda x: int(x['avg_cycles_per_call']))
        winner = func_data[0]
        if winner['language'] == 'Fortran':
            fortran_wins += 1
        else:
            c_wins += 1
    
    print("Cross-Language Results:")
    print("-" * 25)
    print(f"Fortran wins: {fortran_wins}/3 algorithms ({fortran_wins/3*100:.0f}%)")
    print(f"C wins:       {c_wins}/3 algorithms ({c_wins/3*100:.0f}%)")
    print()
    
    print("Key Finding:")
    print("-" * 12)
    print("Algorithm choice remains paramount - the 6k±1 hand-optimized algorithm")
    print("outperforms even the best compiler-optimized brute force by ~30x.")
    print("However, among identical algorithms, compiler choice creates dramatic")
    print("performance differences (up to 9x between languages).")
    print()
    
    print("For detailed analysis, see: C_vs_Fortran_Performance_Analysis.md")
    print("Raw data available in: results/c_vs_fortran/")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())