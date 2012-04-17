#include <GL/freeglut.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "uint.h"
#include "draw.h"
#include "common.h"
#include "colors.h"

//////////////////////////////////////////////////////
//extern// int screen_width,screen_height; 	      /**/
//////////////////////////////////////////////////////

void glPrint(char *s,int x, int y){
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0f,screen_width,screen_height,0.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	glColor4f(0.0f,1.0f,1.0f,1.0f);
	
	int yy=y;
	void *font=GLUT_BITMAP_HELVETICA_12;
	
	glRasterPos2i(x,y);
	while(*s!='\0'){
		glutBitmapCharacter(font,*s);
		if(*s=='\n'){
			yy+=glutBitmapHeight(font);
			char *c=s+1;
			while(*c=='\t')c++;
			s=c;
			glRasterPos2i(x,yy);
		}
		else s++;
	}
	
	glPopMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
	glColor4fv(black); //reset Color
}

void PrintMenu(void){
	uint x=screen_width/15;
	uint y=100;
	
	glPrint("__Keys_________Operation____________\n\
			SpaceBar:\n\
			Alt+Enter:\n\
			P:\n\
			Esc:\n\
			M:\n\
			B:",x,y);
			
	glPrint("Reset Rotation\n\
			Toggle Fullscreen Mode\n\
			Toggle Point Rendering Mode\n\
			Exit Program\n\
			Toggle Menu\n\
			Toggle Box",x+80,y+15);          
}

void screenFade(void){
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0f,screen_width,0.0f,screen_height);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
	glBegin(GL_QUADS);
		glVertex2f(0.0f,0.0f);
		glVertex2f(0.0f,screen_height);
		glVertex2f(screen_width,screen_height);
		glVertex2f(screen_width,0.0f);
	glEnd();
	glEnable(GL_LIGHTING);
	PrintMenu();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glColor4fv(black);
}

void createBox(float *boxMatrix){
	
	glDisable(GL_LIGHTING);
	
	glLineWidth(4.0f);
	
	glBegin(GL_LINES);
		for(uint i=0;i<3;i++){
			glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(boxMatrix[i],boxMatrix[i+3],boxMatrix[i+6]);
		}
	glEnd();	
	
	glPushMatrix();
	glTranslatef(boxMatrix[0]+boxMatrix[1],boxMatrix[3]+boxMatrix[4],boxMatrix[6]+boxMatrix[7]);
	glBegin(GL_LINES);
		for(uint i=0;i<3;i++){
			glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(-boxMatrix[i],-boxMatrix[i+3],boxMatrix[i+6]);
		}
	glEnd();
	glPopMatrix();
		
	glPushMatrix();
	glTranslatef(boxMatrix[0]+boxMatrix[2],boxMatrix[3]+boxMatrix[5],boxMatrix[6]+boxMatrix[8]);
	glBegin(GL_LINES);
		for(uint i=0;i<3;i++){
			glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(-boxMatrix[i],boxMatrix[i+3],-boxMatrix[i+6]);
		}
	glEnd();
	glPopMatrix();
		
	glPushMatrix();
	glTranslatef(boxMatrix[1]+boxMatrix[2],boxMatrix[4]+boxMatrix[5],boxMatrix[7]+boxMatrix[8]);
	glBegin(GL_LINES);
		for(uint i=0;i<3;i++){
			glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(boxMatrix[i],-boxMatrix[i+3],-boxMatrix[i+6]);
		}
	glEnd();
	glPopMatrix();
	
	glEnable(GL_LIGHTING);
}

GLuint createShapeDL(void){
	GLuint shapeDL;
	shapeDL=glGenLists(1);
	
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,blue);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,sh_white);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
	
	glNewList(shapeDL,GL_COMPILE);
		// glutSolidSphere(0.5f,30,30);
		glutSolidCube(1.0f);
	glEndList();
	
	return (shapeDL);
}
