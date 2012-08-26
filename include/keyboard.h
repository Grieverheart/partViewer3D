#ifndef KEYBOARD_H
#define KEYBOARD_H
void keyDown(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void specialDown(int key, int x, int y);
void specialUp(int key, int x, int y);
extern bool box_display;
extern bool render_points;
extern bool pause;
extern bool rotating;
extern float ani_speed;
extern float scale;
extern uint ani_frame;
extern const GLfloat* CrystalColors[4];
#endif
