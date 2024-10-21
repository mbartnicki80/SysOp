#define _POSIX_C_SOURCE 200809L
#include "grid.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ncurses.h>
#include <pthread.h>

const int grid_width = 30;
const int grid_height = 30;

typedef struct {
    int start;
    int end;
    char *src;
    char *dst;
} Args;

pthread_t *threads = NULL;
Args *args = NULL;

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void free_threads(int number_of_threads) {
    free(args);
    free(threads);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void update_grid(char *src, char *dst)
{
    for (int i = 0; i < grid_height; ++i)
    {
        for (int j = 0; j < grid_width; ++j)
        {
            dst[i * grid_width + j] = is_alive(i, j, src);
        }
    }
}

void *update_cell(void *arg) {
    Args *args = arg;

    while (1) {
        for (int i = args->start; i <= args->end; ++i)
        {
            int row = i / grid_width;
            int col = i % grid_width;
            args->dst[i] = is_alive(row, col, args->src);
        }
        pause();

        char *temp = args->src;
        args->src = args->dst;
        args->dst = temp;
    }
}

void ignore_handler() {}

void init_threads(char* foreground, char* background, int number_of_threads) {
    if (number_of_threads > grid_height * grid_width)
        number_of_threads = grid_height * grid_width;

    int cells_per_thread = (grid_height * grid_width) / number_of_threads;
    int remaining_cells = (grid_height * grid_width) % number_of_threads;

    int start = 0;
    threads = malloc(number_of_threads * sizeof(pthread_t));
    args = malloc(number_of_threads * sizeof(Args));
    
    signal(SIGUSR1, SIG_IGN);
    // static struct sigaction action;
    // sigemptyset(&action.sa_mask);
    // action.sa_handler = ignore_handler;
    // sigaction(SIGUSR1, &action, NULL);
    for (int i = 0; i < number_of_threads; i++) {
        args[i].start = start;
        args[i].end = start + cells_per_thread + (i < remaining_cells ? 1 : 0);
        args[i].src = foreground;
        args[i].dst = background;
        pthread_create(&threads[i], NULL, update_cell, &args[i]);
        start = args[i].end + 1;
    }
}

void update(int number_of_threads) {
    for (int i = 0; i < number_of_threads; i++)
        pthread_kill(threads[i], SIGUSR1);
}