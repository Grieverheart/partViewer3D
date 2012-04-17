#include <GL/freeglut.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "uint.h"
#include "mouse.h"
#include "selection.h"
#include "common.h"

//////////////////////////////////////////////////////
//extern// int screen_width,screen_height; 	      /**/
//extern// bool redisplay;				   	      /**/
//extern// bool arcball_reset;			   	      /**/
//////////////////////////////////////////////////////
/**/int last_mx=0,last_my=0,cur_mx=0,cur_my=0;    /**/
/**/bool dragging=false;					   	  /**/
//////////////////////////////////////////////////////
/**/GLfloat LastRotMatrix[16];					  /**/
/**/GLfloat ThisRotMatrix[]={1.0f,0.0f,0.0f,0.0f, /**/
/**/					 	 0.0f,1.0f,0.0f,0.0f, /**/
/**/					 	 0.0f,0.0f,1.0f,0.0f, /**/
/**/					 	 0.0f,0.0f,0.0f,1.0f};/**/
//////////////////////////////////////////////////////


static void axisAngle2Ortho4f(GLfloat *M, GLfloat aa[4]){
	GLfloat c=cosf(aa[0]*PI/180.0f);
	GLfloat C=1.0f-c;
	GLfloat s=sinf(aa[0]*PI/180.0f);
	GLfloat norm=0.0f;
	
	for(uint i=1;i<4;i++)norm+=aa[i]*aa[i];
	norm=sqrt(norm);
	for(uint i=1;i<4;i++)aa[i]/=norm;
	
	M[0]=aa[1]*aa[1]*C+c;
	M[1]=aa[2]*aa[1]*C+aa[3]*s;
	M[2]=aa[3]*aa[1]*C-aa[2]*s;
	M[3]=0.0f;
	M[4]=M[1]-2.0f*aa[3]*s;
	M[5]=aa[2]*aa[2]*C+c;
	M[6]=aa[2]*aa[3]*C+aa[1]*s;
	M[7]=0.0f;
	M[8]=M[2]+2.0f*aa[2]*s;
	M[9]=M[6]-2.0f*aa[1]*s;
	M[10]=aa[3]*aa[3]*C+c;
	M[11]=0.0f;
	M[12]=0.0f;
	M[13]=0.0f;
	M[14]=0.0f;
	M[15]=1.0f;
}

static inline void multMatrix4f(GLfloat *A,GLfloat *B){
	GLfloat sum;
    GLfloat C[16];
	
    for(uint i=0;i<3;i++){
        for(uint k=0;k<3;k++){
            sum=0.0f;
            for(uint j=0;j<3;j++){
                sum+=A[4*j+i]*B[4*k+j];   
            }
            C[4*k+i]=sum;
        }
    }
    for(uint i=0;i<3;i++){
		for(uint j=0;j<3;j++){
			A[4*j+i]=C[4*j+i];
		}
	}
}

static inline void assignMatrix4f(GLfloat *A, GLfloat *B){
	for(uint i=0;i<16;i++)A[i]=B[i];
}

static inline void setIdentityMatrix4f(GLfloat *A){
	for(uint i=0;i<16;i++){
		if(i%5==0)A[i]=1.0f;
		else A[i]=0.0f;
	}
}

static void getArcballVec3f(int x, int y, GLfloat *P){
	int screen_min=screen_height;
	if(screen_width<screen_height)screen_min=screen_width;
	P[0]=(2.0f*x-screen_width)/screen_min;
	P[1]=-(2.0f*y-screen_height)/screen_min; //negate y coordinate (openGL uses -y)
	P[2]=0.0f;
	
	GLfloat temp_square=P[0]*P[0]+P[1]*P[1];
	if(temp_square<=0.5f) P[2]=sqrt(1.0f-temp_square);
	else P[2]=0.5f/sqrt(temp_square);
}

void idleArcball(void){
	if(cur_mx!=last_mx||cur_my!=last_my){
		GLfloat a[3],b[3],c[4];
		GLfloat dot=0.0f;
		
		getArcballVec3f(last_mx,last_my,a);
		getArcballVec3f(cur_mx,cur_my,b);
		for(uint i=0;i<3;i++){
			dot+=a[i]*b[i];
			c[i+1]=a[(i+1)%3]*b[(i+2)%3]-b[(i+1)%3]*a[(i+2)%3];
		}
		if(dot>1.0f)dot=1.0f;
		else if(dot<-1.0f)dot=-1.0f;
		c[0]=(180.0f/PI)*acos(dot);
		
		axisAngle2Ortho4f(ThisRotMatrix,c);
		multMatrix4f(ThisRotMatrix,LastRotMatrix);
		
		redisplay=true;
	}
	if(arcball_reset){
		setIdentityMatrix4f(ThisRotMatrix);
		arcball_reset=false;
		redisplay=true;
	}
}

void onMouse(int button, int state, int x, int y){
	if(!menu_open){
		if(button==GLUT_LEFT_BUTTON){
			if(state==GLUT_DOWN){
				dragging=true;
				cur_mx=x;
				last_mx=cur_mx;
				cur_my=y;
				last_my=cur_my;
				assignMatrix4f(LastRotMatrix,ThisRotMatrix);
			}
			else if(dragging){
				dragging=false;
				if(last_mx==x&&last_my==y){
				//Regular Clicking goes here//
					selectParticle(x,y);
					redisplay=true;
				}
				
				last_mx=cur_mx;
				last_my=cur_my;
			}
		}
		if(button==3||button==4){
			zoom=zoom+(button*2-7)*1.5f; // put wheel up and down in one
			if(zoom<-58)zoom=-58.0f;
			else if(zoom>90)zoom=90.0f;
			glMatrixMode(GL_PROJECTION);
	        glLoadIdentity();
	        GLfloat aspect=(GLfloat)screen_width/(GLfloat)screen_height;
	        if(!perspective){
		        GLfloat height = tan((fov+zoom)*PI/360.0f) * boxMatrix[8]*1.7f;//Magic Number 1.7: We translated 1.7*boxMatrix away
		        glOrtho(-height*aspect, height*aspect, -height, height, 1.0f, 50.0f);
	        }
	        else gluPerspective(fov+zoom, aspect , 1.0f, 50.0f);
	        glMatrixMode(GL_MODELVIEW);
			redisplay=true;
		}
	}
}

void onMotion(int x, int y){
	if(dragging){
		cur_mx=x;
		cur_my=y;
	}
}
