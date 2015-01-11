#define _BSD_SOURCE
#include <string.h>
#include <stdio.h>
#include "buffer.h"
#define BUFFSIZE 32

// "private" function declarations
Color buff_get_fg_col(Buffer this, int x, int y);
Color buff_get_bg_col(Buffer this, int x, int y);
char* buff_get_char  (Buffer this, int x, int y);

int buff_set_fg_col  (Buffer this, int x, int y, Color c);
int buff_set_bg_col  (Buffer this, int x, int y, Color c);
int buff_set_char    (Buffer this, int x, int y, char *c);
void buff_set_cursor_position (Buffer this, int x, int y);

int buff_in_bounds   (Buffer this, int x, int y);

void buff_flush(Buffer this);
void buff_reset(Buffer this);

int buff_update_termsize(Buffer this);

/**
 * Create a new Buffer with dimensions from $ROWS/$COLUMNS env-variables
 */
Buffer new_buffer() {

    // allocate memory
    Buffer ret = (Buffer) calloc(1,sizeof(struct Buffer));
    ret->size = (Vector2) calloc(1, sizeof(struct Vector2));
    ret->cursor_pos = (Vector2) calloc(1, sizeof(struct Vector2));

    if( ret->size == NULL || ret->cursor_pos == NULL || buff_update_termsize(ret) < 0 ) {
        dispose_buffer( &ret );
    } else { // bind functions
        ret->get_fg_col = buff_get_fg_col;
        ret->get_bg_col = buff_get_bg_col;
        ret->get_char = buff_get_char;

        ret->set_fg_col = buff_set_fg_col;
        ret->set_bg_col = buff_set_bg_col;
        ret->set_char = buff_set_char;
        ret->set_cursor = buff_set_cursor_position;

        ret->clear = buff_update_termsize;
        ret->flush = buff_flush;
    }
    return ret;
}

/**
 * free all allocated space of the buffer and set
 * the pointer to 0 to prevent dangling buffers
 */
void dispose_buffer( Buffer *bptr ) {
    Buffer obj = *bptr;
    if(obj != NULL) {
        // set console to default state ...
        obj->set_cursor(obj, 0, 0);
        obj->clear(obj);         // clear buffers
        obj->flush(obj);         // flush them to terminal

        if(obj->size != NULL) {
            free(obj->size);
        }
        if(obj->colors != NULL) {
            free(obj->colors);
        }
        if(obj->chars != NULL) {
            free(obj->chars);
        }
        free(obj);
    }

    // set console to default values: background, foreground, cursor
}

/**
 * Get/set the foreground color at x/y
 */
Color buff_get_fg_col (Buffer this, int x, int y) {
    if(!buff_in_bounds(this, x, y)) {
        x = 0;
        y = 0;
    }
    return (*(this->colors + x + y * this->size->x) >> 4) & 0x0F;
}

int buff_set_fg_col (Buffer this, int x, int y,Color c) {
    if(!buff_in_bounds(this, x, y)) {
        return 0;
    }
    *(this->colors + x + y * this->size->x) &= 0x0F; // cut out old color
    *(this->colors + x + y * this->size->x) |= c << 4; // add new one
    return 1;
}

/**
 * Get/set the background color at x/y
 */
Color buff_get_bg_col (Buffer this, int x, int y) {
    if(!buff_in_bounds(this, x, y)) {
        x = 0;
        y = 0;
    }
    return *(this->colors + x + y * this->size->x) & 0x0F;
}

int buff_set_bg_col (Buffer this, int x, int y, Color c) {
    if(!buff_in_bounds(this, x, y)) {
        return 0;
    }
    *(this->colors + x + y * this->size->x) &= 0xF0; // delete lower 4 bit
    *(this->colors + x + y * this->size->x) |= c;    // or it with c
    return 1;
}

/**
 * get/set the char* at position x/y
 */
char *buff_get_char(Buffer this, int x, int y) {
    if(!buff_in_bounds(this, x, y)) {
        x = 0;
        y = 0;
    }
    return this->chars + y*this->size->x + x;
}

int buff_set_char(Buffer this, int x, int y, char* c) {
    if(!buff_in_bounds(this, x, y) || x + strlen(c) > this->size->x) {
        return 0;
    }
    strncpy(this->chars + x + y*this->size->x, c, strlen(c)); //don't copy terminating NULL
    return 1;
}

/**
 * Buffer sanity checking
 */
int buff_in_bounds(Buffer this, int x, int y) {
    if(x >= this->size->x || y >= this->size->y || x < 0 || y < 0) {
        return 0;
    }
    return 1;
}

void buff_set_cursor_position(Buffer this, int x, int y) {
    this->cursor_pos->x = x;
    this->cursor_pos->y = y;
}


void buff_flush(Buffer this) {
    // line with maximum entropy:
    // 8 char (set x/y) b->size->x * (change_fg (5), change_bg(5) & char(1)), 1 NULL
    int len = 17 + 11 * this->size->x;
    char *output = (char*)malloc( len * sizeof(char) );

    int i, j, off;
    for(i = 0; i < this->size->y; i++) {
        // null the output
        memset(output, '\0', len );
        off = 0;

        // set position to line nr
        *(output + off++) = 0x1B;
        *(output + off++) = '[';
        off += itoa(i+1, output + off, 10);
        *(output + off++) = ';';
        off += itoa(1, output + off, 10);
        *(output + off++) = 'f';

        for(j = 0; j < this->size->x; j++) {
            Color fg = this->get_fg_col(this, j, i);
            Color bg = this->get_bg_col(this, j, i);

            if(j == 0
                    || fg != this->get_fg_col(this, j - 1, i)
                    || bg != this->get_bg_col(this, j - 1, i)) { // some color changed

                // Vordergrund
                *(output + off++) = 0x1B;
                *(output + off++) = '[';
                off += itoa(30 + fg, output  + off, 10);
                *(output + off++) = 'm';

                // Hintergrund
                *(output + off++) = 0x1B;
                *(output + off++) = '[';
                off += itoa(40 + bg, output  + off, 10);
                *(output + off++) = 'm';
            }
            *(output + off++) = *(this->get_char(this, j, i));
        }

        // last line? place cursor to designated position and change color to appropriate
        if(i == this->size->y - 1) {
            // cursor pos
            *(output + off++) = 0x1B;
            *(output + off++) = '[';
            off += itoa(this->cursor_pos->y + 1, output + off, 10);
            *(output + off++) = ';';
            off += itoa(this->cursor_pos->x + 1, output + off, 10);
            *(output + off++) = 'f';

            // Vordergrund
            *(output + off++) = 0x1B;
            *(output + off++) = '[';
            off += itoa(30 + this->get_fg_col(this, this->cursor_pos->x, this->cursor_pos->y), output + off, 10);
            *(output + off++) = 'm';

            // Hintergrund
            *(output + off++) = 0x1B;
            *(output + off++) = '[';
            off += itoa(40 + this->get_bg_col(this, this->cursor_pos->x, this->cursor_pos->y), output + off, 10);
            *(output + off++) = 'm';
        } else {
            // for piping to text file -> create new lines
            *(output + off++) = '\n';
        }
        fputs (output, stdout);
    }
}

void buff_reset(Buffer this) {
    // set the fields to default values (chars -> spaces; colors -> terminal-default)
    memset(this->colors, 0x99, this->size->x * this->size->y);
    memset(this->chars, ' ', this->size->x * this->size->y);
}
/**
 * returns:
 *   0 ... if nothing changed
 *   1 ... buffer updated successfully
 *  -1 ... error
 */
int buff_update_termsize(Buffer this) {
    int env_x = 0, env_y = 0;

    char key[BUFFSIZE];

    FILE *fp = popen( "resize", "r" );  /* run a process, and read it's stdout */

    while ( fgets( key, BUFFSIZE, fp)) {
        if(strncmp(key, "COLUMNS=", 8) == 0) {
            env_x = atoi(key + 8);
        } else if(strncmp(key, "LINES=", 6) == 0) {
            env_y = atoi(key + 6);
        }
    }

    pclose( fp );

    if(this->size->x != env_x || this->size->y != env_y) {
        this->size->x = env_x;
        this->size->y = env_y;

        // free old stuff
        if(this->chars != NULL) {
            free(this->chars);
        }
        if(this->colors != NULL) {
            free(this->colors);
        }
        this->colors = (char*)malloc(this->size->x * this->size->y * sizeof(char));
        this->chars = (char*)malloc(this->size->x * this->size->y * sizeof(char));

        if(this->colors == NULL || this->chars == 0) {
            return -1;
        }
        buff_reset(this);
        return 1;
    }
    buff_reset(this);
    return 0;
}
