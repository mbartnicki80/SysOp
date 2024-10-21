#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


/*
 * Funkcja 'read_end' powinna:
 *  - otworzyc plik o nazwie przekazanej w argumencie
 *    'file_name' w trybie tylko do odczytu,
 *  - przeczytac ostatnie 8 bajtow tego pliku i zapisac
 *    wynik w argumencie 'result'.
 */

void read_end(char *file_name, char *result){
    FILE* file_read = fopen(file_name, "r");
    if (file_read == NULL) {
        perror("open error");
        return;
    }
    FILE* file_out = fopen(result, "w");
    if (file_out == NULL) {
        perror("open error");
        return;
    }
    fseek(file_read, 0, SEEK_END);
    fseek(file_read, -8, SEEK_CUR);
    char buffer[8];
    fread(buffer, 1, 8, file_read);
    for (int i = 0; i < 8; i++)
        fwrite(&buffer[i], sizeof(char), 1, file_out);
    fclose(file_read);
    fclose(file_out);
}


int main(int argc, char *argv[]) {
    int result[2];

    if (argc < 2) return -1;
    read_end(argv[1], (char *) result);
    printf("magic number: %d\n", (result[0] ^ result[1]) % 1000);
    return 0;
}
