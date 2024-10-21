/**
 * W zadaniu mamy cztery wątki: producenta i konsumenta.
 * Każdy producent może wyprodukować zadaną z góry liczbę wartości liczby typu int, tj. 3
 * Każdy wątek producenta produkuje wartości i przechowuje je we współdzielonym buforze o długości 2, 
 * podczas gdy wątek konsumenta pobiera wartości z bufora.
 * 
 * Używamy muteksu i dwóch semaforów (full, empty) do synchronizacji dostępu do buforów. 
 * Muteks służy do zarządzania dostępem do sekcji krytycznej. 
 * Semafory umożliwiają wątkom sygnalizować sobie nawzajem, czy można wykonać operacje zapisu/odczytu.
 * 
 * Każdy wątek producenta generuje liczbą losowa, a następnie wykonuje operację na semaforze.  
 * Jeśli zapis jest możliwy to zajmuje muteks, wstawia wcześniej wygenerowaną liczbę do bufora o indeksie (na pozycji) in
 * wyświetla komunikat na ekranie 
 * (który producent wykonał operację na buforze, zawartość bufora o indeksie (na pozycji) in, wartość indeksu in)
 * inkrementuje wartość indeksu in (operacja modulo), zwalnia mutex
 * a następnie wykonuje operacje na semaforze.
 * 
 * Każdy wątek konsumenta wykonuje operacje na semaforze.
 * Jeśli odczyt jest możliwy to zajmuje muteks
 * wczytuje zawartość bufora o indeksie (z pozycji) out
 * wyświetla komunikat na ekranie
 * (który spośród trzech konsumentów wykonał operację na buforze, pobrana zawartość bufora, wartość indeksu out)
 * modyfikuje wartość indeksu out
 * Zwalnia muteks, a następnie wykonuje operacje na semaforze.
 * 
 * Funkcja main tworzy wątki producenta i konsumenta
*/



#include "buffer.h"
#include <stdlib.h>
#include <stdio.h>

#define MaxItems 3       // liczba wartosci, ktora producent produkuje, zas konsument konsumuje
#define BufferSize 2     // wielkosc bufora
sem_t empty;  // sem. uzupelnic
sem_t full;  // sem. uzupelnic
int in = 0;
int out = 0;
int buffer[BufferSize];  // uzupelnic
pthread_mutex_t mutex;  // mutex uzupelnic

void *producer(void *pno)
{   
    int item;
    for(int i = 0; i < MaxItems; i++) {
        item = rand();           // Wygeneruj losowy element
        sem_wait(&empty);        // operacja na mutex lub semafor
        pthread_mutex_lock(&mutex);  // operacja na mutex lub semafor
        buffer[in] = item;
        printf("Producer %d: Insert Item %d at %d\n", *((int *)pno), buffer[in], in);
        in = (in + 1) % BufferSize;  // operacja na in
        pthread_mutex_unlock(&mutex);  // operacja na mutex lub semafor
        sem_post(&full);  // operacja na mutex lub semafor
    }
    return NULL;
}

void *consumer(void *cno)
{   
    for(int i = 0; i < MaxItems; i++) {
        sem_wait(&full);  // operacja na mutex lub semafor
        pthread_mutex_lock(&mutex);  // operacja na mutex lub semafor
        int item = buffer[out];
        printf("Consumer %d: Remove Item %d from %d\n", *((int *)cno), item, out);
        out = (out + 1) % BufferSize;  // operacja na out
        pthread_mutex_unlock(&mutex);  // operacja na mutex lub semafor
        sem_post(&empty);  // operacja na mutex lub semafor
    }
    return NULL;
}

int main()
{   
    pthread_t pro[4], con[4];
    pthread_mutex_init(&mutex, NULL);  // mutex
    sem_init(&empty, 0, BufferSize);  // semafor
    sem_init(&full, 0, 0);  // semafor

    int a[4] = {1, 2, 3, 4};  // etykiety (numery) producenta i konsumenta

    for(int i = 0; i < 4; i++) {
        pthread_create(&pro[i], NULL, producer, (void *)&a[i]);
    }
    for(int i = 0; i < 4; i++) {
        pthread_create(&con[i], NULL, consumer, (void *)&a[i]);
    }

    for(int i = 0; i < 4; i++) {
        pthread_join(pro[i], NULL);
    }
    for(int i = 0; i < 4; i++) {
        pthread_join(con[i], NULL);
    }

    pthread_mutex_destroy(&mutex);  // mutex
    sem_destroy(&empty);  // semafor
    sem_destroy(&full);  // semafor

    return 0;
}
