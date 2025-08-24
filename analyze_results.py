#!/usr/bin/env python3
import csv

# Read the data
with open('results/three_languages/three_language_comparison.csv', 'r') as f:
    reader = csv.DictReader(f)
    data = list(reader)

print('🚀 ULTIMATE THREE-LANGUAGE PERFORMANCE SHOWDOWN! 🚀')
print('C vs Fortran Prime Algorithm Comparison')
print('=' * 65)
print()

# Group by function for analysis
functions = {}
for row in data:
    func = row['function']
    if func not in functions:
        functions[func] = []
    functions[func].append(row)

print('Performance Results (Cycles per Call - Lower is Better):')
print('=' * 65)

for func_name in ['less_naive_prime', 'naive_prime', 'naive_prime_squares']:
    print(f'\n{func_name.upper()}:')
    func_data = functions[func_name]
    # Sort by cycles per call
    func_data.sort(key=lambda x: int(x['avg_cycles_per_call']))
    
    for row in func_data:
        cycles = int(row['avg_cycles_per_call'])
        lang = row['language']
        opt = row['optimization'] 
        print(f'{lang:<10} {opt:<12} {cycles:<15,}')

print('\n🏆 CHAMPIONS BY ALGORITHM:')
print('=' * 40)

for func_name in ['less_naive_prime', 'naive_prime', 'naive_prime_squares']:
    func_data = functions[func_name]
    func_data.sort(key=lambda x: int(x['avg_cycles_per_call']))
    winner = func_data[0]
    
    winner_lang = winner['language']
    winner_opt = winner['optimization']
    winner_cycles = int(winner['avg_cycles_per_call'])
    
    print(f'{func_name}: {winner_lang} ({winner_opt}) - {winner_cycles:,} cycles/call')
    print('  Performance ratios vs winner:')
    for i, row in enumerate(func_data[:3]):  # Top 3
        ratio = int(row['avg_cycles_per_call']) / winner_cycles
        row_lang = row['language']
        row_opt = row['optimization']
        print(f'    {i+1}. {row_lang} ({row_opt}): {ratio:.2f}x')
    print()

print('🔥 KEY INSIGHTS:')
print('=' * 40)

# Calculate some insights
fortran_wins = 0
c_wins = 0

for func_name in functions:
    func_data = functions[func_name]
    func_data.sort(key=lambda x: int(x['avg_cycles_per_call']))
    winner = func_data[0]
    if winner['language'] == 'Fortran':
        fortran_wins += 1
    else:
        c_wins += 1

print(f'• Fortran wins: {fortran_wins}/3 algorithms ({fortran_wins/3*100:.0f}%)')
print(f'• C wins: {c_wins}/3 algorithms ({c_wins/3*100:.0f}%)')

# Find biggest performance difference
print()
print('🚀 MOST DRAMATIC DIFFERENCES:')
for func_name in ['naive_prime']:
    func_data = functions[func_name]
    func_data.sort(key=lambda x: int(x['avg_cycles_per_call']))
    best = func_data[0]
    worst = func_data[-1]
    
    best_cycles = int(best['avg_cycles_per_call'])
    worst_cycles = int(worst['avg_cycles_per_call'])
    ratio = worst_cycles / best_cycles
    
    best_lang = best['language']
    best_opt = best['optimization']
    worst_lang = worst['language'] 
    worst_opt = worst['optimization']
    
    print(f'{func_name}: {ratio:.1f}x performance difference!')
    print(f'  Best: {best_lang} ({best_opt}) - {best_cycles:,}')
    print(f'  Worst: {worst_lang} ({worst_opt}) - {worst_cycles:,}')

print()
print('💫 INSTRUCTION COUNT ANALYSIS:')
print('=' * 40)

for func_name in ['naive_prime']:
    func_data = functions[func_name]
    print(f'\n{func_name} - Instruction Count Reduction:')
    for row in func_data:
        if row['language'] == 'Fortran' and row['optimization'] == 'opt3':
            fortran_instr = int(row['instructions'])
        elif row['language'] == 'C' and row['optimization'] == 'opt3':
            c_instr = int(row['instructions'])
    
    if fortran_instr and c_instr:
        reduction = ((c_instr - fortran_instr) / c_instr) * 100
        print(f'  C opt3: {c_instr:,} instructions')
        print(f'  Fortran opt3: {fortran_instr:,} instructions')
        print(f'  Fortran achieves {reduction:.1f}% instruction reduction!')