#ifndef LAB2_H
#define LAB2_H

#include <string.h>
#include <stdio.h>


#define BASE_10 10
#define BASE_16 16


typedef unsigned long u32;
int getebp();
void prints(char *s);
void rpu(u32 x, int base);
void printu(u32 x);
void printd(int x);
void printx(u32 x);
void myprintf(char *fmt, ...);
int save();
int longjump(int v);
int A();
int B();
int C();

#endif

