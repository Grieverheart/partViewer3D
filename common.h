#ifndef COMMONVARS_H
#define COMMONVARS_H
#define PI 3.14159265358979323846
#include <stdbool.h>
extern bool arcball_reset;
extern bool menu_open;
extern int screen_width,screen_height;
extern float boxMatrix[9];
extern GLuint sphereDL;
extern GLfloat ThisRotMatrix[16];
static const GLfloat fov=60.0f;
extern float zoom;
extern bool perspective;
extern bool redisplay;
#endif
