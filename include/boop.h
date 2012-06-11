#ifndef __BOOP_H
#define __BOOP_H
void q(int l, float *box, float cutoff, float *result);
void crystallinity(float *box, float cutoff, float crystcut, uint nbonds);
extern float crystal;
#endif
