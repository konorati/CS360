#include "Lab2.h"

void prints(char *s)
{
	char *current = s;
	while(current++)
	{
		putchar(*current);
	}
}

void rpu(u32 x, int base)
{
	char c;
	if(x)
	{
		c = table[x%base];
		rpu(x/base, base);
		putchar(c);
	}
}

void printu(u32 x)
{
    if(x==0)
    {
        putchar('0');
    }
    else
    {
        rpu(x, BASE_10);
    }
}

void printd(int x)
{
    if (x < 0)
    {
        putchar('-');
        x *= -1;
    }
    if (x == 0)
    {
        putchar('0');
    }
    else
    {
        rpu((u32)x, BASE_10);
    }
}

void printx(u32 x)
{
    if(x == 0)
    {
        putchar('0');
    }
    else
    {
        rpu(x, BASE_16);
    }
}
