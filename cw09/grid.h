#pragma once
#include <stdbool.h>

char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void update_grid(char *src, char *dst);
void init_threads(char* foreground, char* background, int number_of_threads);
void free_threads(int number_of_threads);
void update(int number_of_threads);