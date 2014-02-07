#include "Lab2.h"

int main()
{
    int r, a, b, c;
    int x = 12345;
    int y = -12345;
    u32 u = 12345678;
    char *str = "Hello";
    char d = 'A';
    /*prints(str);
    putchar('\n');
    putchar('\r');
    printu(x);
    putchar('\n');
    putchar('\r');
    printd(y);
    putchar('\n');
    putchar('\r');
    printd(0);
    putchar('\n');
    putchar('\r');
    printx(x);
    putchar('\n');
    putchar('\r');*/
    myprintf("char = %c, string = %s, int = %d, negInt = %d, u32 = %u, hex = %x\n", d, str, x, y, u, x);

    myprintf("Enter main\n");
    a = 1;
    b = 2;
    c = 3;

    r = save();
    if (r == 0)
    {
        A();
        myprintf("Normal Return\n");
    }
    else
    {
        myprintf("Back to main via long jump r = %d\n", r);
        myprintf("a=%d b=%d c=%d\n", a, b, c);
    }
    myprintf("Exit Main\n");

    return 0;
}
