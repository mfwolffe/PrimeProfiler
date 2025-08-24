use std::time::Instant;

/// Test case structure matching C and Fortran implementations
#[derive(Clone, Copy)]
struct TestCase {
    n: u32,
    expected: bool,
}

/// All 27 test cases - identical to C and Fortran versions
const TEST_CASES: [TestCase; 27] = [
    TestCase { n: 2, expected: true },
    TestCase { n: 3, expected: true },
    TestCase { n: 5, expected: true },
    TestCase { n: 7, expected: true },
    TestCase { n: 11, expected: true },
    TestCase { n: 13, expected: true },
    TestCase { n: 17, expected: true },
    TestCase { n: 19, expected: true },
    TestCase { n: 23, expected: true },
    TestCase { n: 29, expected: true },
    TestCase { n: 31, expected: true },
    TestCase { n: 37, expected: true },
    TestCase { n: 41, expected: true },
    TestCase { n: 43, expected: true },
    TestCase { n: 47, expected: true },
    TestCase { n: 1, expected: false },
    TestCase { n: 4, expected: false },
    TestCase { n: 6, expected: false },
    TestCase { n: 8, expected: false },
    TestCase { n: 9, expected: false },
    TestCase { n: 10, expected: false },
    TestCase { n: 12, expected: false },
    TestCase { n: 14, expected: false },
    TestCase { n: 15, expected: false },
    TestCase { n: 16, expected: false },
    TestCase { n: 18, expected: false },
    TestCase { n: 20, expected: false },
];

// Import the prime checking functions from lib.rs
use rust_prime_profiler::{naive_prime, naive_prime_squares, less_naive_prime};

/// Test runner for a specific algorithm function
fn test_algorithm(name: &str, prime_fn: fn(u32) -> bool) -> bool {
    println!("Testing {}...", name);
    let mut all_passed = true;
    
    for (i, test_case) in TEST_CASES.iter().enumerate() {
        let result = prime_fn(test_case.n);
        
        if result == test_case.expected {
            println!("  Test {}: {} -> {} ✓", i + 1, test_case.n, result);
        } else {
            println!("  Test {}: {} -> {} ✗ (expected {})", 
                    i + 1, test_case.n, result, test_case.expected);
            all_passed = false;
        }
    }
    
    println!("  {}: {}\n", name, if all_passed { "PASSED" } else { "FAILED" });
    all_passed
}

fn main() {
    println!("=======================================================");
    println!("Rust Prime Algorithm Testing");
    println!("Testing 27 cases across 3 algorithms");
    println!("=======================================================\n");

    let start_time = Instant::now();

    // Test all three algorithms
    let naive_passed = test_algorithm("naive_prime", naive_prime);
    let squares_passed = test_algorithm("naive_prime_squares", naive_prime_squares);
    let less_naive_passed = test_algorithm("less_naive_prime", less_naive_prime);

    let duration = start_time.elapsed();
    
    println!("=======================================================");
    println!("FINAL RESULTS:");
    println!("  naive_prime:         {}", if naive_passed { "PASSED" } else { "FAILED" });
    println!("  naive_prime_squares: {}", if squares_passed { "PASSED" } else { "FAILED" });
    println!("  less_naive_prime:    {}", if less_naive_passed { "PASSED" } else { "FAILED" });
    println!();
    println!("Overall: {}", 
             if naive_passed && squares_passed && less_naive_passed { 
                 "ALL TESTS PASSED" 
             } else { 
                 "SOME TESTS FAILED" 
             });
    println!("Total runtime: {:.2?}", duration);
    println!("=======================================================");
}