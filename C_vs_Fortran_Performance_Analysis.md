# C vs Fortran Prime Algorithm Performance Analysis

**Matt Wolffe, James Madison University, 2025**

## Executive Summary

This comprehensive study compares the performance of identical prime number algorithms implemented in C and Fortran across three optimization levels using Intel PIN binary instrumentation. The results reveal dramatic performance differences that highlight the distinct optimization strategies employed by GCC and gfortran compilers.

**Key Finding**: Fortran demonstrates up to **9.08x superior performance** in naive algorithms while C excels in mathematically optimized implementations, demonstrating that compiler optimization strategies can be more impactful than algorithmic improvements.

**Additional Investigation**: An attempt was made to extend this comparison to include Rust implementations using rustc/LLVM. While the Rust algorithms were successfully implemented and verified for correctness, Intel PIN binary instrumentation proved incompatible with LLVM-optimized binaries due to aggressive function inlining and optimization that obscures function boundaries from runtime analysis tools.

## Methodology

### Test Environment
- **Languages**: C (GCC) vs Fortran 2008 (gfortran)
- **Optimization Levels**: -O0, -O3, -Ofast
- **Instrumentation**: Intel PIN binary analysis
- **Test Suite**: 27 identical prime number test cases
- **Algorithms**: Three implementations of increasing sophistication

### Algorithm Implementations

1. **naive_prime**: Brute force trial division (2 to n-1)
2. **naive_prime_squares**: Optimized trial division (2 to √n)  
3. **less_naive_prime**: Advanced 6k±1 pattern optimization

All algorithms were translated identically between languages, ensuring fair comparison of compiler optimization rather than implementation differences.

## Performance Results

### Overall Winner by Algorithm (-O3 Optimization)

| Algorithm | Winner | Performance Ratio | C Cycles/Call | Fortran Cycles/Call |
|-----------|--------|-------------------|---------------|---------------------|
| **less_naive_prime** | **Fortran** | **2.17x faster** | 555,989 | 256,069 |
| **naive_prime** | **Fortran** | **9.08x faster** | 71,123,758 | 7,835,860 |
| **naive_prime_squares** | **C** | **2.78x faster** | 415,938 | 1,155,852 |

### Complete Performance Matrix

#### Cycles per Function Call (Lower = Better)

| Algorithm | C -O0 | C -O3 | C -Ofast | Fortran -O0 | Fortran -O3 | Fortran -Ofast |
|-----------|-------|-------|----------|-------------|-------------|----------------|
| **less_naive_prime** | 350,024 | 555,989 | 345,430 | 656,977 | **256,069** | 292,921 |
| **naive_prime** | 61,738,477 | 71,123,758 | 62,870,427 | 23,269,588 | **7,835,860** | 11,875,061 |
| **naive_prime_squares** | **313,135** | 415,938 | **340,235** | 671,118 | 1,155,852 | 1,436,952 |

## Deep Dive Analysis

### The Fortran Miracle: naive_prime Optimization

The most striking result is Fortran's 9.08x performance advantage in the `naive_prime` algorithm:

**Instruction Count Reduction:**
- C: 15,996,294 instructions executed
- Fortran: 1,237,423 instructions executed  
- **92.3% instruction reduction**

**Memory Access Transformation:**
- C: 7,998,093 memory reads, 1,599,669 writes
- Fortran: 54 memory reads, 0 writes
- **99.999% memory access reduction**

**Branch Optimization:**
- C: 3,199,275 branches
- Fortran: 222,810 branches
- **93.0% branch reduction**

This suggests gfortran's aggressive loop optimization completely restructured the algorithm's execution pattern, likely through advanced vectorization or mathematical reduction techniques that GCC did not apply to the equivalent C code.

### The C Counter-Attack: naive_prime_squares

Conversely, C dominates in the square root optimized algorithm:

**Why C Wins:**
- C efficiently handles the `sqrt()` function call and floating-point conversion
- GCC's optimization better preserves the mathematical optimization intent
- Fortran's aggressive optimization may have interfered with the square root calculation

### Optimization Level Impact

**Fortran Optimization Characteristics:**
- -O0 to -O3: Dramatic improvements (23M → 7.8M cycles for naive_prime)
- -O3 to -Ofast: Mixed results, sometimes regression
- Best performance typically at -O3

**C Optimization Characteristics:**  
- More consistent across optimization levels
- -O0 and -Ofast often perform similarly
- -O3 sometimes shows performance regression

## Compiler Strategy Analysis

### gfortran's Approach
- **Aggressive loop transformation**: Willing to completely restructure algorithms
- **Memory access minimization**: Extraordinary reduction in memory operations
- **Mathematical optimization**: Strong pattern recognition for numerical algorithms
- **Risk/reward**: High variance in results - spectacular wins or notable losses

### GCC's Approach
- **Conservative optimization**: Maintains algorithmic structure
- **Consistent performance**: Predictable optimization outcomes
- **Memory efficiency**: Good but not transformative
- **Reliability**: Steady performance across different algorithmic patterns

## Statistical Summary

### Performance Distribution
- **Fortran wins**: 2 out of 3 algorithms (67%)
- **Maximum Fortran advantage**: 9.08x (naive_prime)
- **Maximum C advantage**: 2.78x (naive_prime_squares)
- **Average performance difference**: 4.68x in favor of winner

### Optimization Level Effectiveness
- **Best Fortran optimization**: -O3 (2 out of 3 algorithms)
- **Best C optimization**: -O0/-Ofast (algorithm dependent)
- **Most consistent**: C across all levels
- **Most variable**: Fortran with high peaks and valleys

## Practical Implications

### When to Choose Fortran
1. **Computationally intensive loops** where compiler can apply aggressive optimization
2. **Mathematical algorithms** with recognizable numerical patterns  
3. **Scientific computing** where 9x speedups justify language choice
4. **Memory-bound applications** where access pattern optimization is critical

### When to Choose C
1. **Mixed workloads** requiring consistent performance
2. **Mathematical libraries** with complex function calls
3. **Systems programming** where predictable optimization is essential
4. **Performance-critical code** where worst-case performance matters

## Conclusions

This analysis reveals that **compiler optimization strategy is as important as algorithmic choice**. The same algorithm can perform radically differently depending on the compiler's ability to recognize and optimize specific patterns.

**Key Takeaways:**

1. **Fortran's numerical heritage** shows in its exceptional optimization of traditional mathematical algorithms
2. **C's systems heritage** provides more predictable, conservative optimization suitable for diverse workloads  
3. **Algorithm complexity interacts with compiler sophistication** - simpler algorithms benefit more from aggressive optimization
4. **Performance benchmarking must consider compiler choice** as a primary variable, not just algorithmic implementation

The 9.08x performance difference in identical algorithms demonstrates that in high-performance computing, **the choice of language and compiler can be more impactful than algorithmic optimizations**.

## Technical Details

### Measurement Methodology
- **Binary instrumentation**: Intel PIN for precise cycle counting
- **Function-level granularity**: Exact address range detection
- **Language-aware symbol resolution**: Handles Fortran name mangling
- **Statistical validity**: 27 test cases per algorithm
- **Reproducible builds**: Identical compiler flags across languages

### Data Availability
- Complete results: `results/c_vs_fortran/c_vs_fortran_comparison.csv`
- Individual optimization profiles available in `results/c_vs_fortran/`
- Source code and build scripts available in project repository

## Technical Notes

### Rust Implementation Limitation

During this study, equivalent Rust implementations of all three prime algorithms were successfully developed and verified for correctness. However, attempts to extend the performance comparison to include Rust encountered a fundamental incompatibility between Intel PIN binary instrumentation and LLVM-optimized binaries.

**Challenge Details:**
- Rust algorithms compiled successfully with rustc/LLVM at multiple optimization levels
- All 27 test cases passed verification, confirming algorithmic correctness
- Intel PIN's RTN (routine) instrumentation failed to detect function boundaries in LLVM-optimized binaries
- Address-based instrumentation also failed due to aggressive function inlining and optimization

**Root Cause:**
LLVM's optimization strategy differs fundamentally from GCC/gfortran. Where GCC maintains relatively discrete function boundaries that PIN can instrument, LLVM's aggressive inlining, loop unrolling, and code generation create optimized binaries that obscure individual function calls from runtime analysis tools.

**Implications:**
This limitation highlights an important consideration in cross-language performance analysis: **the choice of compiler toolchain affects not only performance but also the feasibility of certain analysis methodologies**. Modern LLVM-based languages may require alternative profiling approaches such as:
- Source-level instrumentation
- Sampling-based profilers
- Language-specific profiling tools
- Custom benchmarking harnesses

The Rust implementation remains available in the repository (`rust/` directory) for future analysis using LLVM-compatible profiling tools.

---
*Generated using Intel PIN binary instrumentation framework with precise cycle-level measurements across GCC and gfortran compiler optimization strategies.*