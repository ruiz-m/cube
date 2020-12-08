#include<stdlib.h>
#include<stdio.h>
#include<SDL2/SDL.h>
#include<math.h>
#include"display.h"

//graphics changed
char changed = 0;

//projection
float fNear = 0.1f;
float fFar = 1000.0f;
float fFov = 90.0f;
float fAspR = 0.0f;
float fFactor = 0.0f;
float q = 0.0f;
int fTheta = 0;

float proj[4][4] = {
	{0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f,0.0f},
	{0.0f,0.0f,1.0f,0.0f}};

float rotx[4][4];
float rotz[4][4];

void mult_mat4x4(vec3d *input, vec3d *output, float mat[4][4])
{
	for(int i=0;i<3;++i)
	{
		output->coord[i] = 0.0f;
		for(int j=0;j<3;++j)
		{
			output->coord[i] += input->coord[j]*mat[i][j];
		}
		output->coord[i] += mat[i][3];
	}

	float w = 0.0f;
	for(int i=0;i<3;++i)
	{
		w += input->coord[i]*mat[3][i];
	}
	w += mat[3][3];

	if(w != 0.0f)
	{
		for(int i=0;i<3;++i)
		{
			output->coord[i] /= w;
		}
	}
}

void set_rotx(float fTheta)
{
	rotx[0][0] = 1.0f;
	rotx[1][1] = cos(fTheta*0.05f);
	rotx[1][2] = sin(fTheta*0.05f);
	rotx[2][1] = -1.0f*rotx[1][2];
	rotx[2][2] = rotx[1][1];
	rotx[3][3] = 1.0f;
}

void set_rotz(float fTheta)
{
	rotz[0][0] = cos(fTheta*0.1f);
	rotz[1][0] = sin(fTheta*0.1f);
	rotz[0][1] = -1.0f*rotz[1][0];
	rotz[1][1] = rotz[0][0];
	rotz[2][2] = 1.0f;
	rotz[3][3] = 1.0f;
}

//graph
void free_mesh(mesh *obj)
{
	free(obj->tris);
	free(obj->tris_proj);
	free(obj);
}

mesh *create_cube()
{
	mesh *obj = (mesh*)malloc(sizeof(mesh));
	obj->num_tris = 12;
	obj->tris = (triangle*)malloc(12*sizeof(triangle));
	obj->tris_proj = (triangle*)malloc(12*sizeof(triangle));

	float info[12][3][3] = {
		{ {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f} },
		{ {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} },

		{ {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f} },
		{ {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f} },

		{ {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f} },
		{ {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f} },
		
		{ {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} },
		{ {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f} },
		
		{ {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f} },
		{ {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f} },
		
		{ {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f} },
		{ {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} }};

	for(int i=0;i<12;++i)
	{
		for(int j=0;j<3;++j)
		{
			for(int k=0;k<3;++k)
			{
				obj->tris[i].p[j].coord[k] = info[i][j][k];
			}
		}
	}

	return obj;
}

//draw stuff
void draw_proj_triangle(SDL_Renderer *render, triangle *triProj)
{
	vec3d *point1 = &triProj->p[0];
	vec3d *point2 = &triProj->p[1];
	vec3d *point3 = &triProj->p[2];

	SDL_RenderDrawLine(render, point1->coord[0], point1->coord[1], point2->coord[0], point2->coord[1]);
	SDL_RenderDrawLine(render, point1->coord[0], point1->coord[1], point3->coord[0], point3->coord[1]);
	SDL_RenderDrawLine(render, point2->coord[0], point2->coord[1], point3->coord[0], point3->coord[1]);
}

void print_proj_triangle(triangle *triProj)
{
	printf("%f, %f\n", triProj->p[0].coord[0], triProj->p[0].coord[1]);
	printf("%f, %f\n", triProj->p[1].coord[0], triProj->p[1].coord[1]);
	printf("%f, %f\n", triProj->p[2].coord[0], triProj->p[2].coord[1]);
}

//display
Display *create_display()
{
	Display *disp = (Display*)malloc(sizeof(Display));
	disp->running = 0;
	disp->win = NULL;
	disp->render = NULL;

	//printf("%d, %d", point.x, point.y);
	return disp;
}


void init(Display *disp, char *title, int xpos, int ypos, int width, int height, char fullscreen)
{
	int flags = 0;
	if(fullscreen == 1)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if(SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		printf("subsystems good\n");

		disp->win = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		disp->render = SDL_CreateRenderer(disp->win, -1, 0);
		disp->objects = (mesh**)calloc(2, sizeof(mesh*));
		disp->num_objs = 2;
		disp->running = 1;
		disp->width = width;
		disp->height = height;

		fNear = 0.1f;
		fFar = 1000.0f;
		fFov = 90.0f;
		fAspR = (float)height/width;
		fFactor = 1.0f / tan((fFov * 0.5f)*(3.14159265f/180.0f));
		q = fFar/(fFar - fNear);

		/*printf("%f\n", fFactor);
		printf("%f\n", q);*/

		proj[0][0] = fAspR*fFactor;
	       	proj[1][1] = fFactor;
		proj[2][2] = q;
		proj[2][3] = -1 * fNear * q;
				
		disp->objects[0] = create_cube();

		changed = 1;
	}
	else
	{
		disp->running = 0;
	}
}

void handle(Display *disp)
{
	SDL_Event event;
	SDL_PollEvent(&event);
	switch(event.type)
	{
		case SDL_QUIT:
			disp->running = 0;
			break;
		default:
			fTheta++;
			set_rotx((float)fTheta*(3.14159f/180.0f));
			set_rotz((float)fTheta*(3.14159f/180.0f));
			changed = 1;
			break;
	}
}


void update(Display *disp)
{
	SDL_SetRenderDrawColor(disp->render,64,64,64,255);
	SDL_RenderClear(disp->render);
	SDL_SetRenderDrawColor(disp->render,255,128,0,255);
	
	mesh *obj;
	triangle buffer1;
	triangle buffer2;	
	for(int i=0;i<disp->num_objs;++i)
	{
		if(disp->objects[i] != NULL)
		{
			obj = disp->objects[i];
			for(int j=0;j<obj->num_tris;++j)
			{
				if(changed == 1)
				{
					for(int k=0;k<3;++k)
					{
						mult_mat4x4(&obj->tris[j].p[k], &buffer1.p[k], rotx);
						mult_mat4x4(&buffer1.p[k], &buffer2.p[k], rotz);
					}

					for(int k=0;k<3;++k)
					{
						buffer2.p[k].coord[2] += 3.0f;
					}

					for(int k=0;k<3;++k)
					{
						mult_mat4x4(&buffer2.p[k], &obj->tris_proj[j].p[k], proj);
					}

					/*for(int k=0;k<3;++k)
					{
						mult_mat4x4(&obj->tris[j].p[k], &obj->tris_proj[j].p[k], proj);
					}*/	

					//print_proj_triangle(&triProjected);	
					for(int k=0;k<3;++k)
					{
						obj->tris_proj[j].p[k].coord[0] += 1.0f;
						obj->tris_proj[j].p[k].coord[1] += 1.0f;

						obj->tris_proj[j].p[k].coord[0] *= 0.5 * disp->width;
						obj->tris_proj[j].p[k].coord[1] *= 0.5 * disp->height;
					}
				}
				draw_proj_triangle(disp->render, &obj->tris_proj[j]);	
			}
		}
	}
	changed = 0;
}

void render(Display *disp)
{
	SDL_RenderPresent(disp->render);
}

void free_display(Display *disp)
{
	SDL_DestroyWindow(disp->win);
	SDL_DestroyRenderer(disp->render);
	SDL_Quit();
	for(int i=0;i<disp->num_objs;i++)
	{
		if(disp->objects[i] != NULL)
		{
			free_mesh(disp->objects[i]);
		}
	}

	free(disp->objects);
	free(disp);
}
