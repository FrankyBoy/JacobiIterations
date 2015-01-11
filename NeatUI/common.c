#include "common.h"

/* Original Source: http://www.jb.man.ac.uk/~slowe/cpp/itoa.html
 * */
void strreverse(char* begin, char* end) {
    char aux;
    while(end > begin) {
        aux=*end, *end--=*begin, *begin++=aux;
    }
}

/* Original Source: http://www.jb.man.ac.uk/~slowe/cpp/itoa.html
 * */
int itoa(int value, char* str, int base) {
    static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char* wstr=str;
    int sign;
    div_t res;

    // Validate base
    if (base<2 || base>35){ return 0; }

    // Take care of sign
    if ((sign=value) < 0) value = -value;

    // Conversion. Number is reversed.
    do {
        res = div(value,base);
        *wstr++ = num[res.rem];
    }while((value=res.quot) != 0);
    if(sign<0) *wstr++='-';

    // Reverse string
    strreverse(str,wstr-1);
    return wstr - str;
}

double drand() {
    return (double)rand()/RAND_MAX ;
}

int round_own (double d) {
    return (d - (int)d < 0.5) ? (int)d : (int)d+1;
}
