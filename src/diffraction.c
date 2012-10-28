#include <stdlib.h>
#include <math.h>
#include "../include/uint.h"
#include "../include/diffraction.h"
#include "../include/particles.h"
#include "../include/common.h"
#include "../include/fftw3.h"

float *intensities;
static fftwf_complex *density;
static fftwf_complex *out;
static fftwf_plan p;
uint diffrRes = 128;


static inline float sqrf(float a){
	return a*a;
}

static inline void matrixVec4f(float *M, float *v){
	float temp[3];
	temp[0] = M[0] * v[0] + M[4] * v[1] + M[8] * v[2];
	temp[1] = M[1] * v[0] + M[5] * v[1] + M[9] * v[2];
	temp[2] = M[2] * v[0] + M[6] * v[1] + M[10] * v[2];
	for(uint i = 0; i < 3; i++) v[i] = temp[i];
}

void diffrInit(void){
	density = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * diffrRes * diffrRes);
	out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * diffrRes * diffrRes);
	p = fftwf_plan_dft_2d(diffrRes, diffrRes, density, out, FFTW_FORWARD, FFTW_MEASURE);
	intensities = malloc(3*diffrRes*diffrRes*sizeof(*intensities));
}

void diffrCalculate(void){
	//diffrReset density Matrix
	for(uint i = 0; i < diffrRes * diffrRes; i++){
		density[i][0] = 0.0f;
		density[i][1] = 0.0f;
	}
	
	float centers[3 * nPart];
	float shiftx = (boxMatrix[0] + boxMatrix[1] + boxMatrix[2]) * 0.5f;
	float shifty = (boxMatrix[4] + boxMatrix[5]) * 0.5f;
	float shiftz = boxMatrix[8] * 0.5f;
	for(uint n = 0; n < nPart; n++){
		//Center particles for rotation
		centers[3*n + 0] = particle[n].coords[0] - shiftx;
		centers[3*n + 1] = particle[n].coords[1] - shifty;
		centers[3*n + 2] = particle[n].coords[2] - shiftz;
		matrixVec4f(ThisRotMatrix, centers + 3*n);
		//Uncenter particles
		centers[3*n + 0] += shiftx;
		centers[3*n + 1] += shifty;
		centers[3*n + 2] += shiftz;
	}
	
	float stepSize = 0.0f; //Length of diagonal of opposite vertices
	for(uint i = 0 ; i < 3; i++){
		float temp = 0.0f;
		for(uint j = 0; j < 3; j++){
			temp += boxMatrix[3*i + j];
		}
		stepSize += sqrf(temp);
	}
	stepSize = 0.8*sqrt(stepSize) / diffrRes; //0.8 just a zoom factor
	//Bin particles in density matrix
	for(uint i = 0; i < nPart; i++){
		int x = (int)(centers[3*i + 0] / stepSize);
		int y = (int)(centers[3*i + 1] / stepSize);
		if(x >= 0 && x < diffrRes && y >= 0 && y < diffrRes) density[diffrRes*x + y][0] += 1.0f;
	}
	//Calculate FFT
	fftwf_execute(p);
	
	float factor = pow(stepSize, 3);
	float max = 0.0f;
	for(uint i = 0; i < diffrRes; i++){
		uint shift_i = (i + diffrRes / 2) % diffrRes;
		for(uint j = 0; j < diffrRes; j++){
			uint shift_j = (j + diffrRes / 2) % diffrRes;
			uint index = diffrRes * shift_i + shift_j;
			intensities[3*(i*diffrRes+j)] = sqrt(factor * (sqrf(out[index][0]) + sqrf(out[index][1])) ); //Calculate sqrt for bigger intensity
			if(intensities[3*(i*diffrRes+j)] > max) max = intensities[3*(i*diffrRes+j)];
		}
	}
	//Calculate Colors
	for(uint i = 0; i < diffrRes; i++){
		for(uint j = 0; j < diffrRes; j++){
			uint index = 3 * (diffrRes * i + j);
			float intensity = (int)(765 * intensities[index] / max);
			
			float r, g, b;
			int test = intensity / 255;
			if(test == 0){
				r = intensity / 255.0f;
				g = 0.0f, b = 0.0f;
			}
			else if(test == 1){
				r = 1.0f;
				g = (intensity - 255) / 255.0f;
				b = 0.0f;
			}
			else{
				r = 1.0f, g = 1.0f;
				b = (intensity - 510) / 255.0f;
			}
			intensities[index + 0] = r;
			intensities[index + 1] = g;
			intensities[index + 2] = b;
		}
	}
}

void diffrClean(void){
	fftwf_destroy_plan(p);
	fftwf_free(out);
	fftwf_free(density);
	free(intensities);
}