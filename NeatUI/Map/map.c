#include "map.h"

// don't expose these function in the header file
int   map_in_bounds  (Map m, int x, int y);
int   map_get_element(Map m, int x, int y);
void  map_set_element(Map m, int x, int y, int sec, int val);
char  map_get_flag   (Map m, int x, int y);
char  map_set_flag   (Map m, int x, int y, char val);
void  map_init_layout(Map m, Environment env);
int   map_iterate_4pt     (Map m);
int   map_iterate_8pt     (Map m);
// int   map_above_accuracy  (Map m);
void  map_switch_pointer  (Map m);
Color map_get_col         (Map m, int x, int y);

/**
 * create a new map instance with respect to the environmental settings
 */
Map new_map(Environment env) {

    Map ret = (Map) calloc(1, sizeof(struct Map));
    if(ret != NULL) {
        // init primitives
        ret->size = (Vector2) calloc(1, sizeof(struct Vector2));
        ret->size->x     = env->size->x;
        ret->size->y     = env->size->y;
        ret->acc         = env->acc;
        ret->locked_mask = (char)0x80;
        ret->it_count    = 0;

        // init function pointers
        ret->get     = map_get_element;
        ret->get_col = map_get_col;
        ret->iterate = env->extra ? map_iterate_8pt : map_iterate_4pt;

        // allocate data space
        ret->data    = (int*)  calloc(ret->size->x * ret->size->y, sizeof(int) );
        ret->secdata = (int*)  calloc(ret->size->x * ret->size->y, sizeof(int) );
        ret->flags   = (char*) calloc(ret->size->x * ret->size->y, sizeof(char));

        if( ret->data == NULL || ret->secdata == NULL || ret->flags == NULL ){
            dispose_map( &ret ); //couldn't alloc memory -> dispose map
        } else {
            map_init_layout( ret, env );
        }
    }
    return ret;
}

/**
 * Frees the map's used memory and sets the pointer to NULL
 */
void dispose_map(Map *map) {
    Map obj = *map;
    if(obj != NULL) {
        // first free the pointers
        if( obj->data != NULL ) {
            free( obj->data );
        }
        if( obj->secdata != NULL ) {
            free( obj->secdata );
        }
        if( obj->flags != NULL ) {
            free ( obj->flags );
        }
        // then free the map
        free (obj);
    }
    *map = NULL;
}

void map_init_layout (Map map, Environment env ) {

    // lock all outer boundaries

    int i;
    for(i = 0; i < map->size->x; i++) {
        map_set_flag(map, i, 0, map->locked_mask);
        map_set_flag(map, i, map->size->y - 1, map->locked_mask);
    }
    for(i = 0; i < map->size->y; i++) {
        map_set_flag(map, 0, i, map->locked_mask);
        map_set_flag(map, map->size->x - 1, i, map->locked_mask);
    }

    // setting up initial design
    switch( env->layout ) {
    case 4: {   // all corners have random temperature
        for (i = 0; i < map->size->x; i++) {
            map_set_element(map, i, 0, 0, drand() * 100e3);
            map_set_element(map, i, map->size->y - 1, 0, drand() * 100e3);
        }
        for (i = 0; i < map->size->y; i++) {
            map_set_element(map, 0, i, 0, drand() * 100e3);
            map_set_element(map, map->size->x - 1, i, 0, drand() * 100e3);
        }
    }break;
    case 3: {   // all corners have 20°C. Also there's a central row
        for (i = 0; i < map->size->x; i++) {
            map_set_element(map, i, 0, 0, 20e3);
            map_set_element(map, i, map->size->y - 1, 0, 20e3);
        }
        for (i = 0; i < map->size->y; i++) {
            map_set_element(map, 0, i, 0, 20e3);
            map_set_element(map, map->size->x - 1, i, 0, 20e3);
        }
        for (i = 1; i <= map->size->y/2; i++) { // middle rows
            map_set_element(map, map->size->x/2, i, 0, 20e3);
            map_set_element(map, map->size->x/2 - 1, i, 0, 20e3);
            map_set_flag(map, map->size->x/2, i, map->locked_mask);
            map_set_flag(map, map->size->x/2 - 1, i, map->locked_mask);
        }
    } break;
    case 2: {   // default case with additional side wall
        for (i = 0; i < map->size->y; i++) {
            map_set_element(map, 0, i, 0, 100e3);
        }
    }
    default: {  // two hot and two cold walls
        for (i = 0; i < map->size->x; i++) {
            map_set_element(map, i, 0, 0, 100e3);
            map_set_element(map, i, map->size->y - 1, 0, 100e3);
        }
    }
    }
}

/**
 * Get the Element at position x, y (in Map m)
 * Hooked into the Map-struct with map.get(x, y)
 */
int map_get_element(Map m, int x, int y) {
    if(map_in_bounds(m,x, y)) {
        return *(m->data + y *m->size->x + x);
    }
    return 0;
}

/**
 * Set the element at position x, y (in Map m)
 */
void map_set_element(Map m, int x, int y, int sec, int val) {
    if(map_in_bounds(m,x, y)) {
        int *data = sec ? m->secdata : m->data;
        *(data + y * m->size->x + x) = val;
    }
}

/**
 * Get the Flags for Element at position x, y (in Map m)
 * Hooked into the Map-struct with map.get_flag(x, y)
 */
char map_get_flag(Map m, int x, int y) {
    if(map_in_bounds(m,x, y)) {
        return *(m->flags + y * m->size->x + x);
    }
    return 0;
}

/**
 * Set the Flag for Element at position x, y (in Map m)
 * returns the old flag status
 * Hooked into the Map-struct with map.set_flag(x, y, val)
 */
char map_set_flag(Map m, int x, int y, char val) {
    if(map_in_bounds(m,x, y)) {
        char ret = *(m->flags + y * m->size->x + x);
        *(m->flags + y * m->size->x + x) = val;
        return ret;
    }
    return 0;
}

/**
 * sanity checking for getter/setter
 * Returns 1 if in bounds, 0 otherwise
 */
int map_in_bounds(Map m, int x, int y) {
    if(x >= m->size->x || y >= m->size->y || x < 0 || y < 0) {
        return 0;
    }
    return 1;
}

int map_iterate_4pt (Map m) {
    int i, j;
    for(i = 0; i < m->size->x; i++) {
        for(j = 0; j < m->size->y; j++) {
            if(map_get_flag(m, i, j) == m->locked_mask) {   // field is locked against editing -> just copy it
                map_set_element(m, i, j, 1, map_get_element(m, i, j));
            }else {
                int val = (map_get_element(m, i-1, j)
                         + map_get_element(m, i+1, j)
                         + map_get_element(m, i, j-1)
                         + map_get_element(m, i, j+1)
                         ) / 4;
                map_set_element(m, i, j, 1, val);
            }
        }
    }
    map_switch_pointer(m);
    m->it_count++;
    return map_above_accuracy(m);
}

int map_iterate_8pt (Map m) {
    int i, j;
    for(i = 0; i < m->size->x; i++) {
        for(j = 0; j < m->size->y; j++) {
            if(map_get_flag(m, i, j) == m->locked_mask) {   // field is locked against editing -> just copy it
                map_set_element(m, i, j, 1, map_get_element(m, i, j));
            }else {
                int val = (map_get_element(m, i-1, j-1)
                         + map_get_element(m, i-1, j)
                         + map_get_element(m, i-1, j+1)
                         + map_get_element(m, i, j-1)
                         + map_get_element(m, i, j+1)
                         + map_get_element(m, i+1, j-1)
                         + map_get_element(m, i+1, j)
                         + map_get_element(m, i+1, j+1)
                         ) / 8;
                map_set_element(m, i, j, 1, val);
            }
        }
    }
    map_switch_pointer(m);
    m->it_count++;
    return map_above_accuracy(m);
}

int map_above_accuracy(Map m) {
    int above = 0;
    int i;

    for(i = 0; i < m->size->x * m->size->y; i++) {
        if (abs(*(m->data + i) - *(m->secdata + i)) > above) {
            above = abs(*(m->data + i) - *(m->secdata + i));
        }
    }

    return above;
}

void map_switch_pointer(Map m) {
    int *tmp = m->data;
    m->data = m->secdata;
    m->secdata = tmp;
}

Color map_get_col (Map m, int x, int y) {
    char ret = (char)(m->get(m, x, y) / 12500);
    if(ret > 7) {
        ret = 7;
    }
    return (Color)ret;
}
