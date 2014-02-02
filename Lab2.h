#ifndef LAB2_H
#define LAB2_H

#include <string.h>
#include <stdio.h>

#define BASE_10 10
#define BASE_16 16

char *table = "0123456789ABCDEF";
typedef unsigned long u32;

void putchar(char c);
int rpu(int x, int base);
int printu(int x);
int printd(int x)
int printx(int x);
void myprintf(char *fmt, ...);

#endif

