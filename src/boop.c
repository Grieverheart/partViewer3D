#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/uint.h"
#include "../include/particles.h"
#define PI 3.1415926535897932
#define EMPTY -1
#define qvindex(Li, Mi, Part, Xi) 2 * (2 * Li + 1) * Part + 2 * (Mi + Li) + Xi


static uint n[3];
static int *cell_list=NULL, *cell=NULL;
float crystal;

static inline float sqr_d(float a){
	//////////////////////////////////////
	//  Returns the square of a float  //
	//////////////////////////////////////
	return a * a;
}

static inline void min_image(float *target, float *origin, float *box_measure, float *result){
	///////////////////////////////////////////////////////////////////
	//  Find the mimnimum distance by using the boundary conditions  //
	///////////////////////////////////////////////////////////////////
	
	/* Calculate the distance vector according to the boundary conditions */
	for(uint i = 0; i < 3; i++){
		result[i] = target[i] - origin[i];
		if(result[i] > 0.5 * box_measure[3 * i + i]) result[i] -= box_measure[3 * i + i];
		else if(result[i] < -0.5 * box_measure[3 * i + i]) result[i] += box_measure[3 * i + i];
	}
}

static inline uint cell_index(float *position, float *box){
	/////////////////////////////////////////////////////////////////
	// Calculate the cell index given the particle position.       //
	// The number of cells and the cell size in each direction	   //
	// are needed. Since we're using reduced coords r_cell=1.0/n   //
	/////////////////////////////////////////////////////////////////
	uint temp = (uint)((position[0] / box[0]) * n[0]) + n[0] * (uint)((position[1] / box[4]) * n[1]);
	return temp + n[0] * n[1] * (uint)((position[2] / box[8]) * n[2]);
}

static inline uint neigh(uint icell, uint m){
	////////////////////////////////////////////////////////////////////////////////
	//  Give a neighbour of cell "icell" given the index "m". The number of cells //
	//	"n" for each dimension and the total number of cells is needed.			  //
	//	Note1: Cell icell (i.e. the current cell) occurs for m=4,13 for D=2,3	  //
	////////////////////////////////////////////////////////////////////////////////
	uint cell_d[3], new_cell_d[3];
	cell_d[0] = icell % n[0];
	cell_d[1] = (icell / n[0]) % n[1];
	cell_d[2] = icell / (n[0] * n[1]);
	
	new_cell_d[0] = (n[0] + cell_d[0] + m % 3 - 1) % n[0];
	new_cell_d[1] = (n[1] + cell_d[1] + (m / 3) % 3 - 1) % n[1];
	new_cell_d[2] = (n[2] + cell_d[2] + m / 9 - 1) % n[2];
	
	uint temp = new_cell_d[0] + new_cell_d[1] * n[0];
	return temp + new_cell_d[2] * n[0] * n[1];
}

static void create_list(float *box, float cutoff){ 
	////////////////////////////////////////////////////////////////////////
	//  Create a Cell linked list for the particles at "centers" in a box //
	//	with matrix "box". "cell" contains the cells and cell_list the 	  //
	//	particles in each cell.											  //
	////////////////////////////////////////////////////////////////////////
	uint ncells = 1;
	float cell_cutoff = cutoff + 0.01;
	
	for(uint i = 0; i < 3; i++){
		n[i] = (uint)(box[3 * i + i] / cell_cutoff);
		ncells *= n[i];
	}
	
	cell_list = malloc(nPart * sizeof(*cell_list));
	cell = malloc(ncells * sizeof(*cell));
	if(cell == NULL || cell_list == NULL){
		printf("ERROR\n");
		exit(0);
	}
	
	for(uint i = 0; i < ncells; i++) cell[i] = EMPTY; //Empty the cells
	uint icell;
	for(uint i = 0; i < nPart; i++){
		icell = cell_index(particle[i].coords, box);
		cell_list[i] = cell[icell]; //Insert the particle in the lists
		cell[icell] = i;
	}
}

static inline float binomial_coeff(int n, int k){
	/////////////////////////////////////////////////
	//  Calculates the binomial coefficient (n,k)  //
	/////////////////////////////////////////////////
	if(k > n){
		printf("Binomia Coefficient: Incorrect arguments\n");
		exit(0);
	}
	float num = 1;
	float dnm = 1;
	for(uint i = 1; i <= k; i++){
		num *= (n - k + i);
		dnm *= i;
	}
	return floor(num / dnm);
}

static inline float fall_fact(int x, int m){
	////////////////////////////////////////////
	//  Calculates the falling factorial x^m  //
	////////////////////////////////////////////
	if((x + 1) < m){
		printf("Falling Factorial: Incorrect arguments\n");
		exit(0);
	}
	float result = 1.0;
	for(uint i = x - m + 1; i <= x; i++){
		result *= i;
	}
	return result;
}

static inline float fact(float x){
	///////////////////////////////////
	//  Calculates the factorial x!  //
	///////////////////////////////////
	if(x < 0.0){
		printf("Factorial: Incorrect arguments\n");
		exit(0);
	}
	if(x <= 1.0) return 1.0;
	else return x * fact(x-1.0);
}

static inline float lgndr_poly(int m, int l, float x){
	/////////////////////////////////////////////////////
	//  Calculates the Associated Legendre Polynomial  //
	/////////////////////////////////////////////////////
	
	if(m > l || l < 0){
		printf("Legendre Polynomial: Incorrect indices\n");
		exit(0);
	}
	if(fabs(x) > 1.0){
		printf("Legendre Polynomial: Incorrect argument\n");
		exit(0);
	}
	
	int mm;
	float factor;
	if(m < 0){
		mm = -m;
		factor = (fact(l - mm) / fact(l + mm)) * pow(sqrt(1 - x * x), mm) / (1 << l);
	}
	else{
		mm = m;
		factor = pow(-sqrt(1 - x * x), mm) / (1 << l);
	}
	
	float sum_part = 0.0;
	for(uint k = 0; k <= l / 2; k++){
		if(l - 2 * k < mm) break;
		float bin = binomial_coeff(l, k) * binomial_coeff(2 * (l - k), l);
		int sign;
		sign = ((k & 1) == 0)? 1 : -1;
		sum_part += sign * fall_fact((l - 2 * k), mm) * bin * pow(x, (float)(l - 2 * k - mm));
	}
	return factor * sum_part;
}

static inline void sph_harm(int m, int l, float *r, float *harmonic){
	//////////////////////////////////////////////////////////
	//  Calculates the Spherical Harmonic for a given bond  //
	//////////////////////////////////////////////////////////
	float norm = 0.0;
	for(uint i = 0; i < 3; i++) norm += sqr_d(r[i]);
	norm = sqrt(norm);
	
	float cos_th = r[2] / norm;
	float m_phi = m * atan2(r[1], r[0]);
	
	float factor = sqrt((2.0 * l + 1) * fact(l - m) / (fact(l + m) * 4.0 * PI));
	factor *= lgndr_poly(m, l, cos_th);
	harmonic[0] = factor * cos(m_phi);
	harmonic[1] = factor * sin(m_phi);
}

void q(int l, float *box, float cutoff, float *result){
	//////////////////////////////////////////////////////////
	//  Calculates the Bond Order Orientational Parameter	//
	//	for a configuration of particles.					//
	//////////////////////////////////////////////////////////
	
	create_list(box, cutoff);
	
	float factor = (2.0 * PI) / (l + 0.5);
	
	for(uint i = 0; i < nPart; i++){
		uint icell = cell_index(particle[i].coords, box);
		result[i] = 0.0;
		for(int m = -l; m <= l; m++){
			float q_complex[2] = {0.0, 0.0};
			int nbs = 0;
			for(uint j = 0; j < 27; j++){ //Loop through the neighbouring cells
				for(int a = cell[neigh(icell, j)]; a != EMPTY; a = cell_list[a]){ //This loop traverses the linked list
					if(a != i){
						float bond[3];
						min_image(particle[a].coords, particle[i].coords, box, bond);
						if( sqrt(sqr_d(bond[0]) + sqr_d(bond[1]) + sqr_d(bond[2])) < cutoff){
							nbs++;
							float complex[2];
							sph_harm(m, l, bond, complex);
							q_complex[0] += complex[0];
							q_complex[1] += complex[1];
						}
					}
				}
			}
			if(nbs != 0) result[i] += (sqr_d(q_complex[0]) + sqr_d(q_complex[1])) / sqr_d(nbs);
		}
		result[i] = sqrt(result[i] * factor);
	}
	free(cell);
	free(cell_list);
}

//////////////////////////////////* Work in Progress *///////////////////////////////////
void crystallinity(float *box, float cutoff, float crystcut, uint nbonds){
	//////////////////////////////////////////////////////////
	//  Calculates the Bond Order Orientational Parameter	//
	//	for a configuration of particles.					//
	//////////////////////////////////////////////////////////
	crystal = 0.0;
	int l = 6;
	
	create_list(box, cutoff);
	float *qvectors;
	qvectors = malloc(2 * (2 * l + 1) * nPart * sizeof(*qvectors));
	
	///////////////* Calculate 2*(l+1) dimensional vectors for each particle *///////////////
	for(uint i = 0; i < nPart; i++){
		uint icell = cell_index(particle[i].coords, box);
		float norm = 0.0;
		for(int m = -l; m <= l; m++){
			qvectors[qvindex(l, m, i, 0)] = 0.0;
			qvectors[qvindex(l, m, i, 1)] = 0.0;
			float q_complex[2] = {0.0, 0.0};
			int nbs = 0;
			for(uint k = 0; k < 27; k++){ //Loop through the neighbouring cells
				for(int a = cell[neigh(icell, k)]; a != EMPTY; a = cell_list[a]){ //This loop traverses the linked list
					if(a != i){
						float bond[3];
						min_image(particle[a].coords, particle[i].coords, box, bond);
						if( sqrt(sqr_d(bond[0]) + sqr_d(bond[1]) + sqr_d(bond[2])) < cutoff){
							nbs++;
							float complex[2];
							sph_harm(m, l, bond, complex);
							q_complex[0] += complex[0];
							q_complex[1] += complex[1];
						}
					}
				}
			}
			if(nbs != 0){
				qvectors[qvindex(l, m, i, 0)] = q_complex[0] / nbs;
				qvectors[qvindex(l, m, i, 1)] = q_complex[1] / nbs;
				norm += (sqr_d(qvectors[qvindex(l, m, i, 0)]) + sqr_d(qvectors[qvindex(l, m, i, 1)]));
			}
		}
		for(int m = -l; m <= l; m++){
			qvectors[qvindex(l, m, i, 0)] /= sqrt(norm);
			qvectors[qvindex(l, m, i, 1)] /= sqrt(norm);
		}
	}
	
	
	///////////////* Calculate the number of solid-like bonds for each particle *///////////////
	for(uint i = 0; i < nPart; i++){
		uint bonds = 0;
		uint icell = cell_index(particle[i].coords, box);
		for(uint k = 0; k < 27; k++){ //Loop through the neighbouring cells
			for(int a = cell[neigh(icell, k)]; a != EMPTY; a = cell_list[a]){ //This loop traverses the linked list
				if(a != i){
					float bond[3];
					min_image(particle[a].coords, particle[i].coords, box, bond);
					if( sqrt(sqr_d(bond[0]) + sqr_d(bond[1]) + sqr_d(bond[2])) < cutoff){
						float cryst = 0.0;
						for(int m = -l; m <= l; m++){
							cryst += qvectors[qvindex(l, m, i, 0)] * qvectors[qvindex(l, m, a, 0)] + qvectors[qvindex(l, m, a, 1)] * qvectors[qvindex(l, m, a, 1)];
						}
						if(cryst > crystcut) bonds++;
					}
				}
			}
		}
		if(bonds > nbonds){
			particle[i].solid = 1;
			crystal++;
		}
		else particle[i].solid = 0;
	}
	crystal = 100.0 * crystal / nPart;
	
	free(cell);
	free(cell_list);
	free(qvectors);
}
