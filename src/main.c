#include <stdio.h>
#include "primes.h"


// TODO @mfwolffe update me w/ expected/actual
//                also this will crash  as is right now lol
void test_candidates(unsigned int candidates[], int num_candidates, const char* algo_name, bool (*function_to_test)(unsigned int)) {
    printf("Testing %s:\n", algo_name);
    for (int i = 0; i < num_candidates; i++)
        printf("%s(%u) = %s\n", algo_name, candidates[i], function_to_test(candidates[i]) ? "true" : "false");
}


int main() {
    case_t sml_cnd[] = { {2, true}, {3, true}, {11, true}, {47, true}, {97, true}, {149, true}, {25, false}, {93, false}, {110, false} };
    case_t med_cnd[] = { {257, true}, {499, true}, {1009, true}, {2003, true}, {2341, true}, {3001, true}, {3500, false}, {3751, false}, {3999, false} };
    case_t lrg_cnd[] = { {5003, true}, {10007, true}, {15011, true}, {20021, true}, {25013, true}, {30029, true}, {68937, false}, {15625, false}, {35019, false} };

    size_t n_sml_cnd = sizeof(sml_cnd) / sizeof(sml_cnd[0]);
    size_t n_med_cnd = sizeof(med_cnd) / sizeof(med_cnd[0]);
    size_t n_lrg_cnd = sizeof(lrg_cnd) / sizeof(lrg_cnd[0]);

    test_candidates(sml_cnd, n_sml_cnd, "naive_prime",          naive_prime);
    test_candidates(sml_cnd, n_sml_cnd, "less_naive_prime",     less_naive_prime);
    test_candidates(sml_cnd, n_sml_cnd, "naive_prime_squares",  naive_prime_squares);

    test_candidates(med_cnd, n_med_cnd, "naive_prime",          naive_prime);
    test_candidates(med_cnd, n_med_cnd, "less_naive_prime",     less_naive_prime);
    test_candidates(med_cnd, n_med_cnd, "naive_prime_squares",  naive_prime_squares);

    test_candidates(lrg_cnd, n_lrg_cnd, "naive_prime",          naive_prime);
    test_candidates(lrg_cnd, n_lrg_cnd, "less_naive_prime",     less_naive_prime);
    test_candidates(lrg_cnd, n_lrg_cnd, "naive_prime_squares",  naive_prime_squares);

    return 0;
}

