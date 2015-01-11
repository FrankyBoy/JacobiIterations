#ifndef COMMON_H_
#define COMMON_H_

#include <stdlib.h>
#include <stdio.h>


typedef enum Color { Black, Red, Green, Yellow,
                     Blue, Magenta, Cyan, White, Default = 9} Color;

typedef struct Vector2* Vector2;

struct Vector2 {
    int x;
    int y;
};

int itoa(int value, char* str, int base);
double drand();
int round_own (double d);
#endif /* COMMON_H_ */
