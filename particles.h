#ifndef PARTICLES_H
#define PARTICLES_H
typedef struct{
	float coords[3];
	float rotation[4];
	GLubyte selColor[2];//Only use 2 colors for up to 65536 particles
	unsigned char selected;
	unsigned char hidden;
}tPart;
extern tPart *particle;
extern uint nPart;
#endif
