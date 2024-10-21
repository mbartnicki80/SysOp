#include <stdio.h>
#include <time.h>
#define BUFFER_SIZE 1024

void reverse(FILE* input, FILE* output) {
    fseek(input, 0, SEEK_END);
    unsigned long size = ftell(input);
    char buffer[1];
    fseek(input, -1, SEEK_CUR);

    while (size > 0) {
        fread(buffer, sizeof(char), 1, input);
        fwrite(&buffer[0], sizeof(char), 1, output);
        size--;
        fseek(input, -2, SEEK_CUR);
    }
}

void reverse_1024(FILE* input, FILE* output) {
    fseek(input, 0, SEEK_END);
    unsigned long size = ftell(input);
    
    char buffer[BUFFER_SIZE];
    fseek(input, -BUFFER_SIZE, SEEK_CUR);

    while (size > 0) {
        unsigned long read_size = BUFFER_SIZE;
        if (size < BUFFER_SIZE) {
            read_size = size;
            fseek(input, 0, SEEK_SET);
        }

        fread(buffer, 1, read_size, input);
        
        for (long i = read_size - 1; i >= 0; i--)
            fwrite(&buffer[i], 1, 1, output);

        fseek(input, -2 * BUFFER_SIZE, SEEK_CUR);
        size -= read_size;
    }
}

int main(int argc, char** argv) {
    if (argc != 4) {
        printf("Inaccurate number of arguments\nUsage: <input> <output> <0 - 1 by 1 byte, else 1024 bytes>\n");
        return -1;
    }

    FILE* input = fopen(argv[1], "r");
    if (input == NULL) {
        printf("Error: Cannot open file '%s'\n", argv[1]);
        return -2;
    }

    FILE* output = fopen(argv[2], "w");
    if (output == NULL) {
        printf("Error: Cannot open file '%s'\n", argv[2]);
        fclose(input);
        return -2;
    }

    clock_t start, end;
    double cpu_time_used;

    start = clock();

    if (*argv[3] == '0')
        reverse(input, output);
    else
        reverse_1024(input, output);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken: %f seconds\n", cpu_time_used);

    fclose(input);
    fclose(output);
    return 0;
}