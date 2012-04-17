#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
				particle=malloc(nPart*sizeof(*particle));
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
		    particle[i-3].selected=0;
		}
		i++;
	}
	free(t1);
	fclose(fp);
}
