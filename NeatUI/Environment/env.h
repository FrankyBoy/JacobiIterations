#ifndef ENV_H_
#define ENV_H_

#include "common.h"

typedef struct Environment* Environment;

struct Environment {
    int r_pause;
    int r_stepsize;
    Vector2 size;
    int repeats;
    int acc;
    int layout;
    int extra;
};

Environment new_env();
void dispose_env( Environment* env );

#endif /* MAIN_H_ */
