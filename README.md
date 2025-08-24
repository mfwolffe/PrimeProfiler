# PrimeProfiler: Cross-Language Prime Algorithm Performance Analysis

**Matt Wolffe, James Madison University, 2025**

A comprehensive performance analysis framework comparing identical prime number algorithms across multiple programming languages using Intel PIN binary instrumentation.

## Key Findings

### C vs Fortran Performance Results
- Fortran demonstrates up to 9.08x superior performance in naive algorithms  
- Fortran achieves 92.3% instruction reduction (16M → 1.2M instructions)
- C excels in mathematical algorithms by 2.78x in square root optimized implementations
- Compiler optimization strategies can be more impactful than algorithmic improvements

## Implemented Languages

### Fully Analyzed
- **C (GCC)**: Complete performance profiling across all optimization levels
- **Fortran 2008 (gfortran)**: Complete performance profiling with optimization analysis

### Implemented but Not Profiled  
- **Rust (rustc/LLVM)**: Algorithms implemented and verified correct, but Intel PIN binary instrumentation incompatible with LLVM optimization strategy

## Algorithms Implemented

Three prime checking algorithms of increasing sophistication:

1. **`naive_prime`**: Brute force trial division (2 to n-1)
2. **`naive_prime_squares`**: Optimized trial division (2 to √n)
3. **`less_naive_prime`**: Advanced 6k±1 pattern optimization

Each algorithm implemented identically across all languages with 27 comprehensive test cases.

## Performance Results

| Algorithm | Winner | Performance Advantage | Best Configuration |
|-----------|--------|----------------------|-------------------|
| `less_naive_prime` | Fortran | 2.17x faster | Fortran -O3 |
| `naive_prime` | Fortran | 9.08x faster | Fortran -O3 |
| `naive_prime_squares` | C | 2.78x faster | C -O0 |

## Project Structure

```
PrimeProfiler/
├── src/          # C implementations
├── fortran/      # Fortran 2008 implementations  
├── rust/         # Rust implementations (complete but not profiled)
├── pin-tools/    # Intel PIN instrumentation tools
├── scripts/      # Profiling automation scripts
├── makefiles/    # Build configurations for all languages
├── results/      # Performance analysis results
└── C_vs_Fortran_Performance_Analysis.md  # Detailed analysis report
```

## Build System

### Supported Optimization Levels
- **C**: `-O0`, `-O3`, `-Ofast`
- **Fortran**: `-O0`, `-O3`, `-Ofast`  
- **Rust**: `dev`, `release`, `fast` (builds successfully, profiling limited)

### Quick Start
```bash
# Build all C variants
make -f makefiles/Makefile.opt0
make -f makefiles/Makefile.opt3
make -f makefiles/Makefile.optfast

# Build all Fortran variants
make -f makefiles/Makefile.fortran-opt0
make -f makefiles/Makefile.fortran-opt3
make -f makefiles/Makefile.fortran-optfast

# Build Rust variants (for testing)
make -f makefiles/Makefile.rust-dev
make -f makefiles/Makefile.rust-release
make -f makefiles/Makefile.rust-fast
```

## Profiling Framework

### Intel PIN Binary Instrumentation
- **Precise cycle counting**: Hardware-level performance measurement
- **Function-level granularity**: Exact instruction and memory access counting
- **Language-aware symbol resolution**: Handles Fortran name mangling
- **Address range instrumentation**: Captures exact function boundaries

### Automated Analysis Scripts
- `scripts/profile_single_optimization.sh`: Individual optimization level profiling
- `scripts/profile_all_optimizations.sh`: Complete C optimization analysis  
- `scripts/profile_fortran_optimization.sh`: Fortran-specific profiling with symbol detection
- `scripts/profile_c_vs_fortran.sh`: Cross-language comparison framework

## Test Suite

Each algorithm tested against **27 comprehensive test cases**:
- **15 prime numbers**: 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47
- **12 composite numbers**: 1, 4, 6, 8, 9, 10, 12, 14, 15, 16, 18, 20

Identical test cases ensure fair cross-language comparison.

## Technical Requirements

- **Intel PIN Framework**: Binary instrumentation (tested with PIN 3.30)
- **GCC**: C compilation and optimization
- **gfortran**: Fortran 2008 compilation  
- **rustc/cargo**: Rust compilation (optional, for algorithm verification)
- **Linux x86_64**: Development and testing platform

## Key Technical Insights

### Compiler Optimization Strategies
- **gfortran**: Aggressive loop transformation and memory optimization
- **GCC**: Conservative, predictable optimization maintaining structure
- **LLVM/rustc**: Optimization so aggressive it breaks binary instrumentation tools

### Performance Analysis Methodology Limitations
- **Traditional binary instrumentation** works excellently for GCC-family compilers
- **LLVM-optimized binaries** require alternative profiling approaches
- **Compiler choice affects both performance AND analysis feasibility**

## Results and Analysis

Complete performance analysis available in:
- **`C_vs_Fortran_Performance_Analysis.md`**: Comprehensive technical report
- **`results/c_vs_fortran/`**: Raw profiling data and CSV results
- **`analyze_results.py`**: Analysis automation script

## Future Work

1. **Alternative Rust profiling**: Investigate LLVM-compatible profiling tools
2. **Additional languages**: Extend to other compilers (Clang, Intel ICC)
3. **Algorithm expansion**: Test with more complex numerical algorithms
4. **Hardware analysis**: Explore different CPU architectures

## Project History
Originally conceived as an exploration of Intel PIN binary instrumentation on naive prime algorithms posited as a challenge by a professor at JMU. The work began with curiosity about the 6k±1 optimization pattern for prime checking and expanded to reveal fundamental differences in compiler optimization strategies.

*For detailed methodology, complete results, and technical analysis, see `C_vs_Fortran_Performance_Analysis.md`*
