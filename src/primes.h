#ifndef PRIMES_H
#define PRIMES_H

#include <stdbool.h>

typedef struct {
  bool actual;
  bool expected; 
  unsigned int n;
} case_t;


void test_candidates(case_t *cndts, int n_cndts, const char* algo_str, bool (*fn_to_test)(unsigned int));

/* algorithms to instrument */

/*
 * brute-force, no sqrt shortcut
 *
 */
bool naive_prime(unsigned int n);


/*
 * brute-force, but with sqrt shortcut
 *
 */
bool naive_prime_squares(unsigned int n);

/*
 * 6k +/- 1 + sqrt shortcut
 *
 */
bool less_naive_prime(unsigned int n);

#endif // PRIMES_H
