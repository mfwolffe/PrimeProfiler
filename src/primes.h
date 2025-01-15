#ifndef PRIMES_H
#define PRIMES_H

#include <stdbool.h>

typedef struct {
  bool actual;
  bool expected; 
  unsigned int n;
} case_t;


void test_cndts(case_t *cndts, int n_cndts, const char* algo_str, bool (*prime_fn)(unsigned int));

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
