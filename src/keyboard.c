#include <GL/freeglut.h>
#include <GL/gl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/uint.h"
#include "../include/keyboard.h"
#include "../include/particles.h"
#include "../include/common.h"
#include "../include/boop.h"

////////////////////////////////////////
//extern// tPart *particle; 		/**/
//extern// bool arcball_snap;		/**/
//extern// float ani_speed;			/**/
//extern// float scale;				/**/
////////////////////////////////////////
/**/bool keyMap[256];	  			/**/
/**/bool keySpMap[256];				/**/
/**/bool keyRepeat=true;			/**/
////////////////////////////////////////
/**/bool fullscreen=false;			/**/
/**/bool box_display=false;			/**/
/**/bool arcball_reset=false;		/**/
/**/bool render_points=false;		/**/
/**/bool menu_open=false;			/**/
/**/bool perspective=true;			/**/
/**/bool pause=false;				/**/
////////////////////////////////////////

static void keyOps(void);
static void keySpOps(void);

void keyDown(unsigned char key, int x, int y){
	keyMap[key]=true;
	keyOps();
}

void keyUp(unsigned char key, int x, int y){
	keyMap[key]=false;
}

void specialDown(int key, int x, int y){
	keySpMap[key]=true;
	keySpOps();
}

void specialUp(int key, int x, int y){
	keySpMap[key]=false;
}

static void keyOps(void){
	if(!menu_open){
		if(keyMap['a'])printf("a\n");
		
		if(keyMap['b']){
			box_display=!box_display;
			redisplay=true;
		}
		
		if(keyMap[32]){ //spacebar
			arcball_reset=true;
		}
		
		if(keyMap['p']){
			render_points=!render_points;
			if(render_points)glEnable(GL_POINT_SMOOTH);
			else glDisable(GL_POINT_SMOOTH);
			redisplay=true;
		}
		
		if(keyMap['o']){
			perspective=!perspective;
			glMatrixMode(GL_PROJECTION);
	        glLoadIdentity();
	        GLfloat aspect=(GLfloat)screen_width/(GLfloat)screen_height;
	        if(!perspective){
		        GLfloat height = tan((fov+zoom)*PI/360.0f) * (-init_zoom-boxMatrix[8]*0.5f);//Magic Number 1.7: We translated 1.7*boxMatrix away
		        glOrtho(-height*aspect, height*aspect, -height, height, 1.0f, 50.0f);
	        }
	        else gluPerspective(fov+zoom, aspect , 1.0f, 50.0f);
	        glMatrixMode(GL_MODELVIEW);
			redisplay=true;
		}
		
		if(keyMap['c']){ //Clear all Selections
			for(uint i=0;i<nPart;i++){
				particle[i].selected=0;
				particle[i].hidden=0;
				particle[i].solid = 0;
			}
			scale = 1.0;
			redisplay=true;
		}
		
		if(keyMap['+']){ 
			ani_speed/=2.0f;
		}
		
		if(keyMap['-']){ 
			ani_speed*=2.0f;
		}
		
		if(keyMap['s']){ 
			pause=!pause;
		}
		
		if(keyMap[']']){ 
			scale *= 1.05;
			redisplay=true;
		}
		
		if(keyMap['[']){ 
			scale /= 1.05;
			redisplay=true;
		}
		
		if(keyMap['q']){
			crystallinity(boxMatrix, 1.32, 0.7);
			redisplay=true;
		}
	
		if(keyMap['h']){
			static bool onoff = 0;
			onoff = !onoff;
			for(uint i = 0; i < nPart; i++){
				if(!particle[i].solid && onoff) particle[i].hidden = 1;
				if(!particle[i].solid && !onoff) particle[i].hidden = 0;
			}
			redisplay=true;
		}
	}
	
	if(keyMap[27]){
	    free(particle);
		glutLeaveMainLoop();
	}
	
	if(keyMap['m']){
		menu_open=!menu_open;
		redisplay=true;
	}
	
	if(keyMap[13]){ //Enter
		int mod=glutGetModifiers();
		if(mod==GLUT_ACTIVE_ALT){
			if(fullscreen){
				glutReshapeWindow(600,600);
				glutSetWindowTitle("partViewer3D");
			}
			else{
				glutFullScreen();
				glutSetWindowTitle("partViewer3D Fullscreen");
			}
			fullscreen=!fullscreen;
		}
	}
}

static void keySpOps(void){
	if(keySpMap[GLUT_KEY_F1]){
		if(!keyRepeat)printf("Key Repeat Enabled\n");
		else printf("Key Repeat Disabled\n");
		keyRepeat=!keyRepeat;
		glutSetKeyRepeat(keyRepeat);
	}
}
