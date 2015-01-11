#ifndef BUFFER_H_
#define BUFFER_H_

#include "common.h"
#include "io.h"

typedef struct Buffer* Buffer;

struct Buffer {
    Vector2 size;
    Vector2 cursor_pos;

    char *colors;   // 7 colors -> 2 in 1 byte; fg = high, bg = low
    char *chars;    // the output char for the position

    Color (*get_fg_col) ( Buffer this, int x, int y );
    Color (*get_bg_col) ( Buffer this, int x, int y );
    char *(*get_char)   ( Buffer this, int x, int y );
    int   (*set_fg_col) ( Buffer this, int x, int y, Color c );
    int   (*set_bg_col) ( Buffer this, int x, int y, Color c );
    int   (*set_char)   ( Buffer this, int x, int y, char *c );
    void  (*set_cursor) ( Buffer this, int x, int y );
    void  (*flush)      ( Buffer this );
    int   (*clear)      ( Buffer this );
};

Buffer new_buffer();
void dispose_buffer( Buffer * );

#endif /* BUFFER_H_ */
