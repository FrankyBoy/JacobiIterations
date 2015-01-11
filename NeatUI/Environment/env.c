#include "env.h"

Environment new_env() {
    Environment ret = (Environment) calloc(1,sizeof(struct Environment));
    ret->size = (Vector2) calloc(1, sizeof(struct Vector2));
    ret->acc        = 1;
    ret->extra      = 0;
    ret->size->x     = 20;
    ret->size->y     = 20;
    ret->layout     = 1;
    ret->repeats    = 1e4;
    ret->r_pause    = 100;
    ret->r_stepsize = 1;

    return ret;
}

void dispose_env( Environment* env ) {
    if(*env != NULL) {
        free ( *env );
        *env = NULL;
    }
}
