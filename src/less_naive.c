#include "primes.h"

bool less_naive_prime(unsigned int n)
{
  // the numbers 1,0 are not prime; no 'negative primes'
  if (n <= 1)
    return false;

  // both 2 and 3 are prime
  if (n == 2 || n == 3)
    return true;

  // Implementation of loop below leaves numbers unchecked [6,10].
  // also by logic above these two checks allow the below loop to work
  if (n % 2 == 0 || n % 3 == 0)
    return false;

  // i=5 & i+=6 so that it is easier to check remaining potential divisors
  // could do this part several ways technically. the check against sqrt(n)
  // is done indirectly (i * i) as I am assuming using the sqrt() fn in math.h
  // is not very performant?
  for (int i = 5; i * i <= n; i += 6)
    // check divisors of form 6n + 1 and 6n + 5 (i + 2 & i here)
    if (n % (i + 2) == 0 || n % i == 0)
      // if [(i + 2) | candidate] || (i | candidate), candidate is not prime
      return false;

  // otherwise prime
  return true;
}
