#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Usage: <number_of_threads>");
		exit(EXIT_FAILURE);
	}
	int number_of_threads = atoi(argv[1]);

	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	char *foreground = create_grid();
	char *background = create_grid();
	char *tmp;

	init_grid(foreground);
	init_threads(foreground, background, number_of_threads);
	while (true)
	{
		draw_grid(foreground);
		usleep(500 * 1000);

		// Step simulation
		//update_grid(foreground, background);
		
		update(number_of_threads);
		tmp = foreground;
		foreground = background;
		background = tmp;
	}

	endwin(); // End curses mode
	free_threads(number_of_threads);
	destroy_grid(foreground);
	destroy_grid(background);

	return 0;
}
