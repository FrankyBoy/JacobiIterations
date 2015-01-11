#ifndef MAP_H_
#define MAP_H_

#include "common.h"
#include "Environment/env.h"


typedef struct Map* Map;

struct Map{
    Vector2 size;
    int     acc;
    int     it_count;
    int     *data;
    int     *secdata;
    char    *flags;
    char    locked_mask;
    int     (*get)    (Map, int x, int y);
    Color   (*get_col)(Map, int x, int y);
    int     (*iterate)(Map);
};

Map  new_map( Environment );
void dispose_map( Map* );
int  map_above_accuracy  (Map m);
#endif /* MAP_H_ */
