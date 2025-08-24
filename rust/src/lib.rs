/// Naive prime checking algorithm - brute force trial division from 2 to n-1
/// Direct Rust translation of the C implementation
#[no_mangle]
#[inline(never)]
pub extern "C" fn naive_prime_c(n: u32) -> bool {
    if n <= 1 {
        return false;
    }
    
    for i in 2..n {
        if n % i == 0 {
            return false;
        }
    }
    
    true
}

/// Prime checking with square root optimization - trial division from 2 to √n
/// Direct Rust translation of the C implementation
#[no_mangle]
#[inline(never)]
pub extern "C" fn naive_prime_squares_c(n: u32) -> bool {
    if n <= 1 {
        return false;
    }
    
    if n <= 3 {
        return true;
    }
    
    if n % 2 == 0 || n % 3 == 0 {
        return false;
    }
    
    let sqrt_n = (n as f64).sqrt() as u32;
    
    for i in 2..=sqrt_n {
        if n % i == 0 {
            return false;
        }
    }
    
    true
}

/// Most optimized prime checking using 6k±1 pattern
/// Direct Rust translation of the C implementation
#[no_mangle]
#[inline(never)]
pub extern "C" fn less_naive_prime_c(n: u32) -> bool {
    if n <= 1 {
        return false;
    }
    
    if n <= 3 {
        return true;
    }
    
    if n % 2 == 0 || n % 3 == 0 {
        return false;
    }
    
    let sqrt_n = (n as f64).sqrt() as u32;
    let mut i = 5;
    
    while i <= sqrt_n {
        if n % i == 0 || n % (i + 2) == 0 {
            return false;
        }
        i += 6;
    }
    
    true
}

/// Rust-native version of naive_prime
pub fn naive_prime(n: u32) -> bool {
    naive_prime_c(n)
}

/// Rust-native version of naive_prime_squares
pub fn naive_prime_squares(n: u32) -> bool {
    naive_prime_squares_c(n)
}

/// Rust-native version of less_naive_prime
pub fn less_naive_prime(n: u32) -> bool {
    less_naive_prime_c(n)
}