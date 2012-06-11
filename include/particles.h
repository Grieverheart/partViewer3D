#ifndef PARTICLES_H
#define PARTICLES_H
#include <GL/gl.h>
typedef struct{
	float coords[3];
	float rotation[4];
	GLubyte selColor[2];//Only use 2 colors for up to 65536 particles
	unsigned char selected;
	unsigned char hidden;
	unsigned char solid;
}tPart;
extern tPart *particle;
extern uint nPart;
#endif
