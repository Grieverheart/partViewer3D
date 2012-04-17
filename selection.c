#include <GL/gl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "uint.h"
#include "particles.h"
#include "common.h"
#include "selection.h"

///////////////////////////////////////
//extern//GLuint sphereDL;			 //
//extern//GLfloat ThisRotMatrix[16]; //
//extern//float boxMatrix[9];		 //
///////////////////////////////////////

static void renderInSelection(void){
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DITHER);
	glDisable(GL_BLEND);
	
	glLoadIdentity(); //Just to be sure
	glTranslatef(0.0f,0.0f,-2.2f*boxMatrix[8]);//Fix Zoom
	glMultMatrixf(ThisRotMatrix);
	glTranslatef(-boxMatrix[0]/2.0f,-boxMatrix[4]/2.0f,-boxMatrix[8]/2.0f); //Center Box
	
	for(uint i=0;i<nPart;i++){
		glPushMatrix();
		glTranslatef(particle[i].coords[0],particle[i].coords[1],particle[i].coords[2]);
		glRotatef(particle[i].rotation[0],particle[i].rotation[1],particle[i].rotation[2],particle[i].rotation[3]);
		glColor3ub(particle[i].selColor[0],particle[i].selColor[1],0);//Set Color Id for Selection
		glCallList(sphereDL);
		glPopMatrix();
	}
	
	glEnable(GL_LIGHTING);
	glEnable(GL_DITHER);
	glEnable(GL_BLEND);
}

void selectParticle(int mouse_x,int mouse_y){
	GLint viewport[4];
	GLubyte pixel[3];
	static uint selection=0;
	
	renderInSelection();
	
	glGetIntegerv(GL_VIEWPORT,viewport);
	
	glReadPixels(mouse_x,viewport[3]-mouse_y,1,1,GL_RGB,GL_UNSIGNED_BYTE,(GLvoid*)pixel);
	
	selection=(uint)(pixel[0]+pixel[1]*256);
	if(selection>=nPart)return;//Background is 51,51,51
	particle[selection].selected=!particle[selection].selected;
	if(particle[selection].selected){
		printf("P[%03d] coords: %f,%f,%f\n",selection,
											particle[selection].coords[0],
											particle[selection].coords[1],
											particle[selection].coords[2]);
	}
}
