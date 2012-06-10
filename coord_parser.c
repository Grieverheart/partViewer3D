#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "uint.h"
#include "coord_parser.h"
#include "particles.h"
#include "common.h"

tPart *particle=NULL;
float boxMatrix[9];
uint nPart=0;

void parseCoords(char *file, char *delim){
	char line[128];
	char *t1=NULL;
	int i=1,u=0;
	
	FILE *fp;
	if(file==NULL)file="melted.dat";
	fp=fopen(file,"r");
	if(!fp){
		printf("Couldn't open file %s\n",file);
		return;
	}
	
	for(uint l=0;l<9;l++)boxMatrix[l]=0.0f; //Initialize the boxMatrix matrix
	
	while(fgets(line,sizeof(line),fp)!= NULL){
		u=0;
		if(line[strlen(line)-1]=='\n')line[strlen(line)-1]='\0'; // Remove the niewline from the end of the string
		for(t1=strtok(line,delim);t1!=NULL;t1=strtok(NULL,delim)){
			if(i==1){
				nPart=(uint)atoi(t1);
				if(particle == NULL) particle=malloc(nPart*sizeof(*particle));
			}
			else if(i==2){
				boxMatrix[u]=atof(t1);
			}
			else if(i>2){
				if(u<3)particle[i-3].coords[u]=atof(t1);
				else particle[i-3].rotation[u-3]=atof(t1);
			}
			u++;
		}
		if(i>2){
		    particle[i-3].selColor[0]=(GLubyte)((i-3)%256);
		    particle[i-3].selColor[1]=(GLubyte)((i-3)/256);
		}
		i++;
	}
	free(t1);
	fclose(fp);
}

uint *obj_faces;
GLfloat *obj_vertices;
uint obj_nV=0;
uint obj_nF=0;
uint obj_nVpF=0;

void parseObj(char *file){
	//////////////////////////////////////////////////////////////////
	//  Parse an .obj file and initialize the particle properties.  //
	//	This is primarilly made for cubes.							//
	//////////////////////////////////////////////////////////////////
	char line[128];
	char *t1=NULL;
	uint v=0,p=0,f=0;
	bool once=false;
	
	FILE *fp;
	fp=fopen(file,"r");
	if(!fp){
		printf("Couldn't open object file %s\n",file);
		return;
	}
	
	printf("Parsing Object file %s...\n",file);
	obj_vertices=malloc(3*40*sizeof(*obj_vertices));//Change 40 if more than 40 vertices
	obj_faces=malloc(3*20*sizeof(*obj_faces));//Change 20 if more than 20 faces
	
	while(fgets(line,sizeof(line),fp)!= NULL){
		if(line[0]=='#')fgets(line,sizeof(line),fp);
		p=0;
		if(line[strlen(line)-1]=='\n')line[strlen(line)-1]='\0'; // Remove the newline from the end of the string
		if(line[0]=='v'){
			for(t1=strtok(line," ");t1!=NULL;t1=strtok(NULL," ")){
				if(p>0)obj_vertices[3*v+p-1]=atof(t1);
				p++;
			}
			v++;
		}
		else if(line[0]=='f'){
			for(t1=strtok(line," ");t1!=NULL;t1=strtok(NULL," ")){
				if(p>0)obj_faces[obj_nVpF*f+p-1]=atoi(t1)-1;
				p++;
			}
			if(!once){
				obj_nVpF=p-1;
				once=true;
			}
			f++;
		}
	}
	obj_nV=v;
	obj_nF=f;
	
	printf("Done Parsing Object file\nCounted %d vertices and %d faces\n",obj_nV,obj_nF);
	obj_vertices=realloc(obj_vertices,3*obj_nV*sizeof(*obj_vertices));
	obj_faces=realloc(obj_faces,obj_nVpF*obj_nF*sizeof(*obj_faces));
	
	free(t1);
	fclose(fp);
}