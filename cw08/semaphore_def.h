#include <semaphore.h>
#include <stdbool.h>
#define QUEUE_SIZE 10
#define MAX_PRINTERS 3
#define SEMAPHORE_NAME "semaphore"

typedef struct {
    sem_t printer_semaphore;
    char text[QUEUE_SIZE];
} printer_t;

typedef struct {
    printer_t printers[MAX_PRINTERS];
    int number_of_printers;
} Printers;