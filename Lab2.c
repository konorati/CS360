#include "Lab2.h"
char *table = "0123456789ABCDEF";
int savedPC, savedFP;
void prints(char *s)
{
	char *current = s;
	while(*current)
	{
		putchar(*current++);
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

void myprintf(char *fmt, ...)
{
    int ebp;
    char **cp;
    char *fmtStr;
    int *ip;
    char tmp;
    
    ebp = getebp();
    //printf("ebp = %x\n", ebp); // For testing
    cp = (char**)(ebp+8);
    //printf("cp = %x, *cp = %c\n", cp, **cp); // For testing
    fmtStr = *cp;
    //printf("fmtStr = %s", fmtStr);
    ip = (int *)(ebp+12);
    //printf("ip = %x\n", ip); // For testing

    while(*fmtStr != 0)
    {
        if(*fmtStr == '%')
        {
            fmtStr++;
            switch(*fmtStr)
            {
                case 'c': tmp = (char)*ip;
                        putchar(tmp);

                        break;
                case 's': prints((char *)(*ip));
                        break;
                case 'd': printd((int)*ip);
                        break;
                case 'x': printx((u32)*ip);
                        break;
                case 'u': printu((u32)*ip);
                        break;
                default:
                    prints(" Invalid format specifier ");
                    break;
            }
            fflush(stdout);
            ip++;
        }
        else
        {
            putchar(*fmtStr);
            fflush(stdout);
            if(*fmtStr == '\n')
            {
                putchar('\r');
            }
        }
        fmtStr++;
    }

}

int save()
{  
    int *ebp = (int *)getebp();
    savedFP = *ebp;
    savedPC = *(ebp+1);
    myprintf("ebp=%x oldFP=%x retPC=%x\n", ebp, savedFP, savedPC);
    return 0;
}

int longjump(int v)
{
    int *ebp = (int *)getebp();
    *ebp = savedFP; 
    *(ebp+1) = savedPC;
    myprintf("ebp=%x savedFP=%x savedPC=%x\n", ebp, *ebp, *(ebp+1));
    return v;
}

int A()
{
    myprintf("Enter A\n"); 
    B();
    myprintf("Exit A\n");
    return 0;
}

int B()
{
    myprintf("Enter B\n");
    C();
    myprintf("Exit B\n");
    return 0;
}

int C()
{
    myprintf("Enter C\n");
    myprintf("Long jump? (y|n) ");
    if(getchar() == 'y')
    {
        longjump(12345);
    }
    myprintf("Exit C\n");
    return 0;
}
