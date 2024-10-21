#include "collatz_conjecture.h"

int collatz_conjecture(int input) {
    if (input % 2 == 0)
        return input /= 2;
    else
        return 3*input + 1;
}

int test_collatz_convergence(int input, int max_iter) {
    if (input == 1)
        return 0;

    for (int i = 0; i < max_iter; i++) {
        input = collatz_conjecture(input);
        if (input == 1)
            return i + 1;
    }
    
    return -1;
}