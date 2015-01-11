#include "common.h"
#include "renderer.h"
#include <string.h>

// "hidden" prototypes
void renderer_put_str    (Renderer this, int x, int y, char *string, int align_right);
void renderer_put_int    (Renderer this, int x, int y, int val, int align_right);
void renderer_put_str_ctr(Renderer this, int x, int y, int boxwidth, char *string, Color c, char fill);
void renderer_put_hline  (Renderer this, int x, int y, int len, char c);
void renderer_put_vline  (Renderer this, int x, int y, int len, char c);
void renderer_color_line (Renderer this, int x, int y, int len, Color c);
void renderer_put_cursor (Renderer this, int x, int y );
void renderer_draw       (Renderer this);
void renderer_clear      (Renderer this);
int renderer_validates   (Renderer this, int x, int y, int width, int height);


Renderer new_renderer() {
    Renderer ret = (Renderer) calloc(1,sizeof(struct Renderer));
    if(ret != NULL) {
        ret->buffer      = new_buffer();
        ret->maxsize     = ret->buffer->size;

        ret->put_str     = renderer_put_str;
        ret->put_int     = renderer_put_int;
        ret->put_str_ctr = renderer_put_str_ctr;
        ret->put_hline   = renderer_put_hline;
        ret->put_vline   = renderer_put_vline;
        ret->color_line  = renderer_color_line;
        ret->put_cursor  = renderer_put_cursor;
        ret->draw        = renderer_draw;
        ret->clear       = renderer_clear;
    }
    return ret;
}

void dispose_renderer(Renderer *rd) {
    Renderer obj = *rd;
    if(obj != NULL) {
        dispose_buffer( &(obj->buffer) );
        free ( *rd );
        *rd = NULL;
    }
}

void renderer_put_cursor(Renderer this, int x, int y) {
    this->buffer->set_cursor(this->buffer, x, y);
}

void renderer_put_str(Renderer this, int x, int y, char *string, int align_right) {
    if(align_right) {
        x = x - strlen(string);
    }
    this->buffer->set_char(this->buffer, x, y, string);
}


void renderer_put_int(Renderer this, int x, int y, int val, int align_right) {
    // use a buffer
    char* buff = (char*)calloc(64*sizeof(char), 1);
    itoa(val, buff, 10);
    renderer_put_str(this, x, y, buff, align_right);
    free(buff);
}

void renderer_put_str_ctr(Renderer this, int x, int y, int boxwidth, char *string, Color c, char fill) {
    if(renderer_validates(this, x, y, boxwidth, 1)) {
        this->put_hline(this, x, y, boxwidth, fill);
        this->put_str(this, x + (boxwidth - strlen(string)) / 2, y, string, 0);
        this->color_line(this, x, y, boxwidth, c);
    }
}

void renderer_put_hline(Renderer this, int x, int y, int len, char c) {
    if(renderer_validates(this, x, y, len, 1)) {
        char* line = (char*)malloc(len * sizeof(char) + 1);
        if(line != NULL) {
            memset(line, c, len);
            *(line + len) = '\0';
            this->buffer->set_char(this->buffer, x, y, line);
        }
    }
}

void renderer_put_vline(Renderer this, int x, int y, int len, char c) {
    if(renderer_validates(this, x, y, 1, len)) {
        int i;
        for (i = 0; i < len; i++) {
            *this->buffer->get_char(this->buffer, x, y + i) = c;
        }
    }
}

void renderer_color_line(Renderer this, int x, int y, int len, Color c) {
    if(renderer_validates(this, x, y, 1, 1)) {
        int i;
        for(i = 0; i < len; i++) {
            this->buffer->set_bg_col(this->buffer, x + i, y, c);
        }
    }
}

int renderer_validates(Renderer this, int x, int y, int width, int height) {
    if(x < 0 || x + width > this->maxsize->x || y < 0 || y > this->maxsize->y) {
        return 0;
    }
    return 1;
}

void renderer_draw ( Renderer this ) {
    this->buffer->flush(this->buffer);
}

void renderer_clear ( Renderer this ) {
    this->buffer->clear(this->buffer);
}
