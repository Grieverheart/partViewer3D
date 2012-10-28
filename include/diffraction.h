#ifndef __DIFFRACTION_H
#define __DIFFRACTION_H

extern float *intensities;
extern uint diffrRes;

void diffrInit(void);
void diffrCalculate(void);
void diffrClean(void);

#endif