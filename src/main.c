#include<stdio.h>
#include<stdlib.h>
#include"display.h"

int main(int argc, char *args[])
{
	Display *disp = create_display();
	init(disp, "title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
	while(disp->running == 1)
	{
		handle(disp);
		update(disp);
		render(disp);
	}

	free_display(disp);
	return 0;
}
