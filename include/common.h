#ifndef COMMONVARS_H
#define COMMONVARS_H
#define PI 3.14159265358979323846
#include <stdbool.h>
extern bool arcball_reset;
extern bool menu_open;
extern int screen_width,screen_height;
extern float boxMatrix[9];
extern GLuint sphereDL;
extern GLuint CSGDL;
extern GLfloat ThisRotMatrix[16];
static const GLfloat fov=60.0f;
extern float zoom;
extern float init_zoom;
extern bool perspective;
extern bool redisplay;
extern bool use_obj;
extern uint *obj_faces;
extern GLfloat *obj_vertices;
extern uint obj_nV;//Number of vertices
extern uint obj_nF;//Number of faces
extern uint obj_nVpF; //Number of vertices per face
#endif
