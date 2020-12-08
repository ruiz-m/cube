#ifndef __DISPLAY_H__
#define __DISPLAY_H__
#include<stdlib.h>
#include<SDL2/SDL.h>

typedef struct point_3d
{
        int x,y,z;
}point3d;

typedef struct vector3d
{
	float coord[3];
}vec3d;

typedef struct triangle
{
	vec3d p[3];
}triangle;


typedef struct mesh
{
	int num_tris;
	triangle *tris;
	triangle *tris_proj;
}mesh;


extern void free_mesh(mesh *obj);
extern void mult_mat4x4(vec3d *i, vec3d *o, float mat[4][4]);
extern mesh *create_cube();

extern void set_rotx(float fTheta);
extern void set_rotz(float fTheta);

extern void draw_proj_triangle(SDL_Renderer *render, triangle *triProj);
extern void print_proj_triangle(triangle *triProj);

typedef struct disp
{
	char running;
	int num_objs;
	int width;
	int height;
	SDL_Window *win;
	SDL_Renderer *render;
	mesh **objects;
}Display;

extern Display *create_display();
extern void init(Display *disp, char *title, int xpos, int ypos, int width, int height, char fullscreen);
extern void handle(Display *disp);
extern void update(Display *disp);
extern void render(Display *disp);
extern void free_display(Display *disp);
#endif
