#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "buffer.h"

typedef struct Renderer* Renderer;

struct Renderer {
    Buffer buffer;
    Vector2 maxsize;
    void (*put_str)    ( Renderer this, int x, int y, char *str, int rtl);
    void (*put_int)    ( Renderer this, int x, int y, int val, int rtl);
    void (*put_str_ctr)( Renderer this, int x, int y, int boxwidth, char *string, Color c, char fill);
    void (*put_hline)  ( Renderer this, int x, int y, int len, char c);
    void (*put_vline)  ( Renderer this, int x, int y, int len, char c);
    void (*color_line) ( Renderer this, int x, int y, int len, Color c);
    void (*put_cursor) ( Renderer this, int x, int y );
    void (*draw)       ( Renderer this );
    void (*clear)      ( Renderer this );
};

// function-header
Renderer new_renderer();
void dispose_renderer(Renderer *);

#endif /* DISPLAY_H_ */
