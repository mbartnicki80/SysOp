#include <stdio.h>

int main() {
    float a = 0;
    for (int i = 100; i >= 0; i-=10) {
        a += 1.111;
        printf("%02d %.2f\n", i, a);
    }
    return 0;
}