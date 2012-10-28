#ifndef GLDRAW_H
#define GLDRAW_H
void glPrint(char *s,int x, int y);
void PrintMenu(void);
void screenFade(void);
void createBox(float *boxMatrix);
GLuint createShapeDL(bool flip);
#endif