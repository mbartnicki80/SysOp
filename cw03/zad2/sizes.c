#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

int main(void) {
    DIR* dir = opendir(".");
    if(dir == NULL) {
        printf("Could not open directory");
        return -1;
    }

    struct stat bufor_stat; 
    unsigned long long total_size = 0;
    struct dirent* dir_file = readdir(dir);

    while (dir_file != NULL) {
        stat(dir_file -> d_name, &bufor_stat);
        if (!S_ISDIR(bufor_stat.st_mode)) {
            printf("%8ld %s\n", bufor_stat.st_size, dir_file -> d_name);
            total_size += bufor_stat.st_size;
        }
        dir_file = readdir(dir);
    }

    printf("%llu total\n", total_size);
    closedir(dir);
    return 0;
}