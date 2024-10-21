#include <stdio.h>
#ifdef _DL_
#include <dlfcn.h>
#else
#include "collatz_conjecture.h"
#endif

int main() {
    #ifdef _DL_

    void *handle = dlopen("libcollatz_conjecture.so", RTLD_LAZY);
    if(!handle){
        printf("dlopen error: %s\n", dlerror());
		return 1;
    }

    int (*test_collatz_convergence)(int, int);
    test_collatz_convergence = (int (*)(int, int))dlsym(handle, "test_collatz_convergence");

    if(dlerror() != NULL){
        printf("dlsym error: %s\n", dlerror());
        dlclose(handle);
		return 2;
    }
    printf("input = 27, max_iter = 120, result = %3d\n", (*test_collatz_convergence)(27, 120));
    //printf("input = 5, max_iter = 10, result = %3d\n", (*test_collatz_convergence)(5, 10));
    //printf("input = 10, max_iter = 50, result = %3d\n", (*test_collatz_convergence)(10,  50));
	//printf("input = 100, max_iter = 100, result = %3d\n", (*test_collatz_convergence)(100,  100));
	//printf("input = 200, max_iter = 25, result = %3d\n", (*test_collatz_convergence)(200,  25));
    dlclose(handle);
    
    #else
    printf("input = 5, max_iter = 10, result = %3d\n", test_collatz_convergence(5,  10));
	printf("input = 10, max_iter = 50, result = %3d\n", test_collatz_convergence(10,  50));
	printf("input = 100, max_iter = 100, result = %3d\n", test_collatz_convergence(100,  100));
	printf("input = 200, max_iter = 25, result = %3d\n", test_collatz_convergence(200,  25));
    return 0;
    #endif
}