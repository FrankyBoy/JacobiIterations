#include "ui.h"
#include <string.h>



int  if_draw_ui           ( Interface this, Environment env );
void if_draw_ui_panel     ( Interface this, Environment env, int xoff );
void if_draw_popup_allert ( Interface this, char *msg );
int  if_draw_popup_readint( Interface this, char *msg, int min, int max, int *target );
void if_draw_map          ( Interface this, Map m, Environment env );

Interface new_interface() {
    Interface ret = (Interface)calloc(1, sizeof(struct Interface));
    if(ret != NULL) {
        ret->renderer     = new_renderer();
        ret->draw_ui      = if_draw_ui;
        ret->popup_allert = if_draw_popup_allert;
        ret->draw_map     = if_draw_map;
    }
    return ret;
}

void dispose_interface( Interface *i ) {
    if( *i != NULL ){
        // set cursor to first position and clean terminal
        dispose_renderer( &(*i)->renderer );
        *i = NULL;
    }
}

int if_draw_ui( Interface this, Environment env ) {

    // int choice;
    Renderer r = this->renderer;
    int panel_seperator;

    while (1) {
        r->clear(r);

        // calculate new pannel seperator every time, as size might have changed
        panel_seperator = r->maxsize->x * 2 / 3;
        r->put_hline(r, 0, 0, r->maxsize->x, HLINE_CHAR); // top 2 lines
        r->put_hline(r, 0, 2, r->maxsize->x, HLINE_CHAR);
        r->put_hline(r, 0, r->maxsize->y - 1, r->maxsize->x, HLINE_CHAR); // bottom 2 lines
        r->put_hline(r, 0, r->maxsize->y - 3, r->maxsize->x, HLINE_CHAR);
        r->put_vline(r, panel_seperator, 3, r->maxsize->y - 6, VLINE_CHAR); // settings seperating line

        int i = HEADER_HEIGHT;

        r->put_str(r, 1, 1, "Jacobi-Iteration", 0);
        r->put_str(r, 1, i++, "Options:", 0);
        r->put_str(r, 2, i++, "1) Change Map Size", 0);
        r->put_str(r, 2, i++, "2) Change Layout", 0);
        r->put_str(r, 2, i++, "3) Change Iteration Steps", 0);
        r->put_str(r, 2, i++, "4) Change Maximum Accuracy", 0);
        r->put_str(r, 2, i++, "5) Toggle 4pt / 8pt Mode", 0);
        r->put_str(r, 2, i++, "6) Run Simulation", 0);
        r->put_str(r, 2, i++, "7) Quit Program", 0);

        if_draw_ui_panel(this, env, panel_seperator + 1);
        r->put_cursor(r, 2, r->maxsize->y - 2);

        r->draw(r);

        int choice;
        while ( !read_int(&choice, 1, 20) ) {
            r->put_str(r, r->maxsize->x - 2, r->maxsize->y - 2, "Faulty Input", 1);
            r->draw(r);
        }
        switch(choice){
            case 1:
                if_draw_popup_readint(this, "Enter world width (3 - 50)", 3, 50, &env->size->x);
                if_draw_popup_readint(this, "Enter world height (3 - 50)", 3, 50, &env->size->y);
                break;
            case 2:
                if_draw_popup_readint(this, "Enter Layout (1 - 4)", 1, 4, &env->layout);
                break;
            case 3:
                if_draw_popup_readint(this, "Enter Iteration steps (1 - 100k)", 1, 1e5, &env->repeats);
                break;
            case 4:
                if_draw_popup_readint(this, "Enter Maximum Accuracy (0 - 100)", 1, 100, &env->repeats);
                break;
            case 5:
                env->extra = !env->extra;
                break;
            case 6: return 1;
            case 7: return 0;
            default: if_draw_popup_allert(this, "Not implemented yet");
        }
    }
    return 0;
}

void if_draw_ui_panel(Interface this, Environment env, int xoff) {

    Renderer r = this->renderer;
    int yoff = HEADER_HEIGHT;

    ++xoff; // get a litte space

    // Environment variables
    r->put_str(r, xoff, yoff++, "Environment:", 0);

    r->put_str(r, xoff + 1, yoff, "Mapsize - X:", 0);
    r->put_int(r, r->maxsize->x - 2, yoff++, env->size->x, 1);

    r->put_str(r, xoff + 1, yoff, "Mapsize - Y:", 0);
    r->put_int(r, r->maxsize->x - 2, yoff++, env->size->y, 1);

    r->put_str(r, xoff + 1, yoff, "Layout:", 0);
    r->put_int(r, r->maxsize->x - 2, yoff++, env->layout, 1);

    r->put_str(r, xoff + 1, yoff, "Max-Steps:", 0);
    r->put_int(r, r->maxsize->x - 3, yoff, env->repeats / 1000, 1);
    r->put_str(r, r->maxsize->x - 3, yoff++, "k", 0);

    r->put_str(r, xoff + 1, yoff, "Max-Accuracy:", 0);
    r->put_int(r, r->maxsize->x - 2, yoff++, env->acc, 1);

    r->put_str(r, xoff + 1, yoff, "Mode:", 0);
    if(env->extra) {
        r->put_str(r, r->maxsize->x - 2, yoff++, "8pt", 1);
    } else {
        r->put_str(r, r->maxsize->x - 2, yoff++, "4pt", 1);
    }

    // Rendering relevant variables
    r->put_str(r, xoff, yoff++, "Renderer:", 0);
    r->put_str(r, xoff + 1, yoff++, "-- Empty --", 0);
}


void if_draw_popup_allert(Interface this, char *msg) {

    Renderer r = this->renderer;
    Color c = Red;
    char *header = " Allert ";
    char *ok_box = "[ OK ]";

    int width = r->buffer->size->x / 2;
    int line =  r->buffer->size->y / 4;  //startzeile

    int i = 0; // line counter
    r->put_str_ctr(r, width/2, line + i, width, header, c, HLINE_CHAR);
    ++i;
    r->put_str_ctr(r, width/2, line + i, width, "", c,' ');
    ++i;
    r->put_str_ctr(r, width/2, line + i, width, msg, c,' ');
    ++i;
    r->put_str_ctr(r, width/2, line + i, width, "", c,' ');
    ++i;
    r->put_str_ctr(r, width/2, line + i, width, ok_box, c,' ');

    r->color_line(r, width - strlen(ok_box) / 2,line + i , strlen(ok_box), Blue);
    ++i;
    r->put_str_ctr(r, width/2, line + i, width, "", c,' ');
    ++i;
    r->put_str_ctr(r, width/2, line + i, width, "", c, HLINE_CHAR);

    // draw left/right line
    r->put_vline(r, width/2, line + 1, i - 1, VLINE_CHAR);
    r->put_vline(r, width*3/2 - 1, line + 1, i - 1, VLINE_CHAR);

    r->draw(r);
    getchar();
}

int if_draw_popup_readint(Interface this, char *msg, int min, int max, int *target) {

    Color c = Blue;
    char *header = " Enter Value ";
    Renderer r = this->renderer;

    int width = r->maxsize->x / 2;
    int line =  r->maxsize->y / 4;  //startzeile

    int i = 0; // line counter
    r->put_str_ctr(r, width/2, line + i, width, header, c, HLINE_CHAR); // header
    ++i;
    r->put_str_ctr(r, width/2, line + i, width, "", c,' ');   // empty line
    ++i;
    r->put_str_ctr(r, width/2, line + i, width, msg, c,' ');      // msg
    ++i;
    r->put_str_ctr(r, width/2, line + i, width, "", c,' ');   // empty line
    ++i;
    r->put_str_ctr(r, width/2, line + i, width, "", c,' ');   // input box
    r->color_line(r, width/2 + 2, line + i, width - 4, Green);
    r->put_cursor(r, width/2 + 2, line + i);
    ++i;
    r->put_str_ctr(r, width/2, line + i, width, "", c,' ');   // empty line
    ++i;
    r->put_str_ctr(r, width/2, line + i, width, "", c, HLINE_CHAR);   // footer

    // draw left/right line
    r->put_vline(r, width/2, line + 1, i - 1, VLINE_CHAR);
    r->put_vline(r, width*3/2 - 1, line + 1, i - 1, VLINE_CHAR);

    do {
        r->draw(r);
    } while(!read_int(target, min, max));
    return 0;
}

void if_draw_map( Interface this, Map m, Environment env ) {
    Renderer r = this->renderer;
    r->clear(r);

    // draw the map in the middle of the screen
    int i, j,
        xoff = (r->maxsize->x - m->size->x * 2) / 2, // da immer 2 felder in x-richtung gezeichnet werden
        yoff = (r->maxsize->y - m->size->y) / 2;
    for(i = 0; i < m->size->y; i++) {
        for(j = 0; j < m->size->x; j++) {
            r->color_line(r, xoff + j * 2, yoff + i , 2, m->get_col(m, j, i));
        }
    }
    r->draw(r);
}
