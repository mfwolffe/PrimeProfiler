#ifndef PRIMES_H
#define PRIMES_H

#include <stdbool.h>

typedef struct {
  bool actual;
  bool expected; 
  unsigned int n;
} case_t;

bool naive_prime(unsigned int n);
bool naive_prime_squares(unsigned int n);
bool less_naive_prime(unsigned int n);

#endif // PRIMES_H
