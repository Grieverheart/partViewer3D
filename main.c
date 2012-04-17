#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "uint.h"
#include "keyboard.h"
#include "mouse.h"
#include "particles.h"
#include "coord_parser.h"
#include "draw.h"
#include "colors.h"
#include "common.h"

////////////////////////////////////////////////////////
//extern// tPart *particle;				       		/**/
//extern// float boxMatrix[9]; 			       		/**/
//extern// uint nPart;					   	   		/**/
//extern// GLfloat ThisRotMatrix[16];				/**/
//extern// bool render_points;						/**/
////////////////////////////////////////////////////////
/**/int screen_width=0,screen_height=0;		   		/**/
/**/bool redisplay=false;					   		/**/
/**/float zoom=0;							   		/**/
/**/static float fps=0.0;	 						/**/
////////////////////////////////////////////////////////
/**/GLuint sphereDL;      							/**/
////////////////////////////////////////////////////////
/**/static const GLfloat light_p[3][4]={			/**/
/**/					   {-1.0f,1.0f,0.5f,0.1f},  /**/
/**/					   {-0.3f,1.0f,0.5f,0.5f},  /**/
/**/					   {-0.3f,0.6f,0.5f,0.5f}}; /**/
////////////////////////////////////////////////////////

static void countFPS(void){
	static uint frame=0;
	static uint timebase=0;
	
	frame++;
	uint time=glutGet(GLUT_ELAPSED_TIME);
	
	if(time-timebase>500){
		fps=frame*1000.0f/(time-timebase);
		timebase=time;
		frame=0;
	}
}

void renderShape(void){
	glTranslatef(0.0f,0.0f,-2.2f*boxMatrix[8]);//Fix Zoom
	if(menu_open)glTranslatef(boxMatrix[0]/2.0f,0.0f,0.0f);
	glMultMatrixf(ThisRotMatrix);
	glTranslatef(-boxMatrix[0]/2.0f,-boxMatrix[4]/2.0f,-boxMatrix[8]/2.0f); //Center Box
	
	if(box_display)createBox(boxMatrix);
	
	for(uint i=0;i<nPart;i++){
		glPushMatrix();
		glTranslatef(particle[i].coords[0],particle[i].coords[1],particle[i].coords[2]);
		if(render_points){
			glDisable(GL_LIGHTING);
			glBegin(GL_POINTS);
				glVertex3f(0.0f,0.0f,0.0f);
			glEnd();
			glEnable(GL_LIGHTING);
		}
		else{
			glRotatef(particle[i].rotation[0],particle[i].rotation[1],particle[i].rotation[2],particle[i].rotation[3]);
			if(particle[i].selected){
				glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,emissive);
				glCallList(sphereDL);
				glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);//Reset Material
			}
			else glCallList(sphereDL);
		}
		glPopMatrix();
	}
	if(menu_open)screenFade();
}

static void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	char string[20];
	glLoadIdentity();
	sprintf(string,"FPS:%02.0f",fps);
	glPrint(string,5,15);
	
	renderShape();
	glutSwapBuffers();
}

static void idle(void){

	idleArcball();
	
	if(redisplay){
		redisplay=false;
		countFPS();
		glutPostRedisplay();
	}
}

static void reshape(int width, int height){
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov+zoom, (GLfloat)width / (GLfloat)height, 1.0f, 50.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	screen_width=width;
	screen_height=height;
}

static void init(void){
	glClearColor(0.2f,0.2f,0.2f,0.0f);
	glClearDepth(1.0f);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glShadeModel(GL_SMOOTH);
	
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glLightfv(GL_LIGHT0, GL_SPECULAR, sh_white);
	glLightfv(GL_LIGHT1, GL_SPECULAR, green);
	glLightfv(GL_LIGHT2, GL_SPECULAR, red);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_POSITION, light_p[0]);
	glLightfv(GL_LIGHT1, GL_POSITION, light_p[1]);
	glLightfv(GL_LIGHT2, GL_POSITION, light_p[2]);
	
	glPointSize(6.0f);
	sphereDL=createShapeDL();
	
}

int main(int argc,char *argv[] ){
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize(600,600);
	glutInitWindowPosition(100,100);
	glutCreateWindow("partViewer3D");
	
	
	parseCoords(argv[1],"\t");
	init();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutSpecialFunc(specialDown);
	glutSpecialUpFunc(specialUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);
	
	glutMainLoop();
	
	return 1;
}
