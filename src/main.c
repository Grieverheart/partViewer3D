#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "../include/uint.h"
#include "../include/keyboard.h"
#include "../include/mouse.h"
#include "../include/particles.h"
#include "../include/coord_parser.h"
#include "../include/draw.h"
#include "../include/colors.h"
#include "../include/common.h"
#include "../include/boop.h"
#include "../include/diffraction.h"
#include "../include/displaylistPrimitive.h"

////////////////////////////////////////////////////////
//extern// tPart *particle;				       		/**/
//extern// float boxMatrix[9]; 			       		/**/
//extern// uint nPart;					   	   		/**/
//extern// GLfloat ThisRotMatrix[16];				/**/
//extern// bool render_points;						/**/
//extern// bool rotating;							/**/
//extern// uint diffrRes;							/**/
//extern// extern float *intensities; 				/**/
////////////////////////////////////////////////////////
/**/int screen_width=0,screen_height=0;		   		/**/
/**/bool redisplay=false;					   		/**/
/**/bool use_obj;									/**/
/**/bool csg_mode=false;                            /**/
/**/float zoom=0.0f;						   		/**/
/**/float scale=1.0f;								/**/
/**/float init_zoom=-22.0f;							/**/
/**/float csg_boxSize;                              /**/
/**/static float fps=60.0f;	 						/**/
/**/bool animation=false;							/**/
/**/char **ani_matrix;								/**/
/**/uint ani_files=0;								/**/
/**/uint ani_frame=1;								/**/
/**/float ani_speed=1000.0f;						/**/
/**/float rot_speed = 0.5f;							/**/
////////////////////////////////////////////////////////
/**/GLuint sphereDL;      							/**/
/**/GLuint CSGDL;      							    /**/
////////////////////////////////////////////////////////
/**/static const GLfloat light_p[3][4]={			/**/
/**/					   {-1.0f,1.0f,0.5f,0.1f},  /**/
/**/					   {-0.3f,1.0f,0.5f,0.5f},  /**/
/**/					   {-0.3f,0.6f,0.5f,0.5f}}; /**/
////////////////////////////////////////////////////////
/**/const GLfloat* CrystalColors[] = {				/**/
/**/	MediumSlateBlue,                            /**/
/**/	SeaGreen,                                   /**/
/**/	Maroon,                                     /**/
/**/	Sienna};                                    /**/
////////////////////////////////////////////////////////

static inline void multMatrixBA4f(GLfloat *A,GLfloat *B){
	GLfloat sum;
    GLfloat C[16];
	
    for(uint i=0;i<3;i++){
        for(uint k=0;k<3;k++){
            sum=0.0f;
            for(uint j=0;j<3;j++){
                sum+=B[4*j+i]*A[4*k+j];   
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

void renderDiffraction(void){
	glDisable(GL_DEPTH_TEST | GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0f,screen_width,screen_height,0.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(0, screen_height);
	int min = screen_height;
	if(min > screen_width) min = screen_width;
	glPixelZoom((float)min / 600.0f, (float)min / 600.0f);
	glDrawPixels(diffrRes, diffrRes, GL_RGB, GL_FLOAT, intensities);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST | GL_LIGHTING);
}

void renderShape(void){
	glLoadIdentity(); //Just to be sure
	glTranslatef(0.0f,0.0f,init_zoom);//Fix Zoom
	if(menu_open)glTranslatef(boxMatrix[0]/2.0f,0.0f,0.0f);
	if(rotating){
		float tempc = cosf(rot_speed*2.0f*PI / 360.0f);
		float temps = sinf(rot_speed*2.0f*PI / 360.0f);
		float tempMatrix[16] = {
			tempc,	0.0f,	-temps,	0.0f,
			0.0f,	1.0f,	0.0f,	0.0f,
			temps,	0.0f,	tempc,	0.0f,
			0.0f,	0.0f,	0.0f,	1.0f
		};
		multMatrixBA4f(ThisRotMatrix, tempMatrix);
	}
	glMultMatrixf(ThisRotMatrix);
	glTranslatef(-(boxMatrix[0]+boxMatrix[1]+boxMatrix[2])/2.0f,
				 -(boxMatrix[4]+boxMatrix[5])/2.0f,
				 -boxMatrix[8]/2.0f); //Center Box
	
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
		else if(!particle[i].hidden){
			glScalef(scale,scale,scale);
			glRotatef(particle[i].rotation[0],particle[i].rotation[1],particle[i].rotation[2],particle[i].rotation[3]);
			if(particle[i].selected){
				glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,emissive);
				glCallList(sphereDL);
				glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);//Reset Material
			}
			else{
				glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,CrystalColors[particle[i].solid]);
				glCallList(sphereDL);
			}
		}
		glPopMatrix();
	}
	if(menu_open)screenFade();
}

void renderInCSGMode(void){
	glLoadIdentity(); //Just to be sure
	glTranslatef(0.0f,0.0f,init_zoom);//Fix Zoom
	if(menu_open)glTranslatef(boxMatrix[0]/2.0f,0.0f,0.0f);
	if(rotating){
		float tempc = cosf(rot_speed*2.0f*PI / 360.0f);
		float temps = sinf(rot_speed*2.0f*PI / 360.0f);
		float tempMatrix[16] = {
			tempc,	0.0f,	-temps,	0.0f,
			0.0f,	1.0f,	0.0f,	0.0f,
			temps,	0.0f,	tempc,	0.0f,
			0.0f,	0.0f,	0.0f,	1.0f
		};
		multMatrixBA4f(ThisRotMatrix, tempMatrix);
	}
	glMultMatrixf(ThisRotMatrix);
	glTranslatef(-(boxMatrix[0]+boxMatrix[1]+boxMatrix[2])/2.0f,
				 -(boxMatrix[4]+boxMatrix[5])/2.0f,
				 -boxMatrix[8]/2.0f); //Center Box
	
	if(box_display)createBox(boxMatrix);
	
	CSGrender();
    glDepthFunc(GL_EQUAL);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,CrystalColors[0]);
	renderPrimitives();
    glDepthFunc(GL_LEQUAL);
	
	if(menu_open)screenFade();
}

static void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	if(csg_mode)renderInCSGMode();
	else renderShape();
	
	char string[20];
	glLoadIdentity();
	sprintf(string,"FPS: %02.0f",fps);
	glPrint(string,5,15);
	if(crystal > 0.0){
		sprintf(string,"C: %.2f%%",crystal);
		glPrint(string,5,30);
	}
	if(animation){
		sprintf(string,"Frame: %03d",ani_frame);
		if(crystal > 0.0) glPrint(string,5,45);
		else glPrint(string,5,30);
	}
	
	if(renderdiff){
		renderDiffraction();
	}
	glutSwapBuffers();
}

static void idle(void){
	static float last_t = 0.0f;
	float this_t = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
	if(this_t-last_t > 1.0f/61.0f){
		if(renderdiff){
			diffrCalculate();
			redisplay = true;
		}
		if(rotating) redisplay = true;
		idleArcball();
		last_t = this_t;
	}
	

	static uint last_time=0;
	static uint this_time=0;
	
	if(animation&&!pause){
		if(ani_frame >= ani_files)ani_frame = 0;
		else if(ani_frame < 0) ani_frame += ani_files;
		this_time=glutGet(GLUT_ELAPSED_TIME);
		if(this_time-last_time>=ani_speed){
			redisplay=true;
			parseCoords(ani_matrix[ani_frame],"\t");
			ani_frame++;
			last_time=this_time;
		}
	}
	
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
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHT0);
	// glEnable(GL_LIGHT1);
	// glEnable(GL_LIGHT2);
	glShadeModel(GL_SMOOTH);
	
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glLightfv(GL_LIGHT0, GL_SPECULAR, sh_white);
	// glLightfv(GL_LIGHT1, GL_SPECULAR, green);
	// glLightfv(GL_LIGHT2, GL_SPECULAR, red);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_POSITION, light_p[0]);
	// glLightfv(GL_LIGHT1, GL_POSITION, light_p[1]);
	// glLightfv(GL_LIGHT2, GL_POSITION, light_p[2]);
	
	glPointSize(6.0f);
	sphereDL=createShapeDL(false);
	CSGDL=createShapeDL(true);
	
	csg_boxSize = boxMatrix[0];
	createCSGList(CSGDL, particle, nPart, csg_boxSize);
	
}

int main(int argc,char *argv[] ){

	uint obj_index=0;
	uint coords_index=0;

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA | GLUT_MULTISAMPLE | GLUT_ALPHA | GLUT_STENCIL);
	glutInitWindowSize(600,600);
	glutInitWindowPosition(100,100);
	glutCreateWindow("partViewer3D");
	
	int err = glewInit();
    if (GLEW_OK != err) {
        // problem: glewInit failed, something is seriously wrong
        printf("GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    } 
	
	//////////* Command line argument processing */////////////
	for(uint i=0;i<argc;i++){
		if(strcmp(argv[i],"-obj")==0){
			obj_index=i+1;
			use_obj=true;
			break;
		}
	}
	for(uint i=0;i<argc;i++){
		if(strcmp((argv[i]+strlen(argv[i])-4),".dat")==0){
			coords_index=i;
			break;
		}
	}
	if(argc > 4){
		animation=true;
		ani_files=argc-3;
		printf("ani_files:%d\n",ani_files);
		ani_matrix=malloc(ani_files*sizeof(*ani_matrix));
		for(uint i=0;i<ani_files;i++){
			ani_matrix[i]=malloc(strlen(argv[i+3])*sizeof(ani_matrix));
			strcpy(ani_matrix[i], argv[i+coords_index]);
		}
	}
	/////////////////////////////////////////////////////////////
	
	if(!animation) parseCoords(argv[coords_index],"\t");
	else parseCoords(ani_matrix[0],"\t");
	for(uint i = 0; i < nPart; i++){
		particle[i].selected = 0;
		particle[i].hidden = 0;
		particle[i].solid = 0;
	}
	if(use_obj)parseObj(argv[obj_index]);
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
