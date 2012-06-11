#include <GL/freeglut.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/uint.h"
#include "../include/draw.h"
#include <math.h>
#include "../include/common.h"
#include "../include/colors.h"

//////////////////////////////////////////////////////
//extern// int screen_width,screen_height; 	      /**/
//extern// bool use_obj							  /**/
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
	glEnable(GL_LINE_SMOOTH);
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
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(-boxMatrix[0],0.0f,0.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(-boxMatrix[1],-boxMatrix[4],0.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(boxMatrix[2],boxMatrix[5],boxMatrix[8]);
	glEnd();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(boxMatrix[0]+boxMatrix[2],boxMatrix[3]+boxMatrix[5],boxMatrix[6]+boxMatrix[8]);
	glBegin(GL_LINES);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(-boxMatrix[0],0.0f,0.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(boxMatrix[1],boxMatrix[4],0.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(-boxMatrix[2],-boxMatrix[5],-boxMatrix[8]);
	glEnd();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(boxMatrix[1]+boxMatrix[2],boxMatrix[4]+boxMatrix[5],boxMatrix[7]+boxMatrix[8]);
	glBegin(GL_LINES);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(boxMatrix[0],0.0f,0.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(-boxMatrix[1],-boxMatrix[4],0.0f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(-boxMatrix[2],-boxMatrix[5],-boxMatrix[8]);
	glEnd();
	glPopMatrix();
	
	glDisable(GL_LINE_SMOOTH);
	glEnable(GL_LIGHTING);
}

static inline void cross3fn(GLfloat *a, GLfloat *b, GLfloat *result){
	GLfloat norm=0.0f;
	result[0]=a[1]*b[2]-a[2]*b[1];
	result[1]=a[2]*b[0]-a[0]*b[2];
	result[2]=a[0]*b[1]-a[1]*b[0];
	for(uint i=0;i<3;i++){
		norm+=result[i]*result[i];
	}
	norm=sqrt(norm);
	for(uint i=0;i<3;i++){
		result[i]/=norm;
	}
}

static inline void obj_shape(void){
	GLfloat normal[3];
	GLfloat vec1[3],vec2[3];
	uint vertex=0;
	uint vert1=0,vert2=0,vert3=0;
	GLenum mode;
	
	if(obj_nVpF==3){
		mode=GL_TRIANGLES;
	}
	else if(obj_nVpF==4){
		mode=GL_QUADS;
	}
	else{
		mode=GL_POLYGON;
	}
	for(uint i=0;i<obj_nF;i++){
		vert1=obj_faces[i*obj_nVpF];
		vert2=obj_faces[i*obj_nVpF+1];
		vert3=obj_faces[i*obj_nVpF+2];
		for(uint v=0;v<3;v++){
			vec1[v]=obj_vertices[3*vert3+v]-obj_vertices[3*vert2+v];
			vec2[v]=obj_vertices[3*vert1+v]-obj_vertices[3*vert2+v];
		}
		cross3fn(vec1,vec2,normal);
		glBegin(mode);
			glNormal3fv(normal);
			for(uint j=0;j<obj_nVpF;j++){
				vertex=obj_faces[i*obj_nVpF+j];
				glVertex3fv((obj_vertices+3*vertex));
			}
		glEnd();
	}
}

GLuint createShapeDL(void){
	GLuint shapeDL;
	shapeDL=glGenLists(1);
	
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,MediumSlateBlue);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,sh_white);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
	
	glNewList(shapeDL,GL_COMPILE);
		if(use_obj)obj_shape();
		else glutSolidSphere(0.5f,30,30);
	glEndList();
	
	return (shapeDL);
}
