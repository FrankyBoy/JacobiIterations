#ifndef UI_H_
#define UI_H_

#include "renderer.h"
#include "io.h"
#include "Environment/env.h"
#include "Map/map.h"
#include "common.h"

#define HLINE_CHAR '-'
#define VLINE_CHAR '|'
#define HEADER_HEIGHT 4

typedef struct Interface* Interface;

struct Interface {
    Renderer renderer;
    int  (*draw_ui)     (Interface, Environment env );
    void (*popup_allert)(Interface, char *msg );
    void (*draw_map)    (Interface, Map m, Environment env);
};

Interface new_interface();
void dispose_interface( Interface * );

#endif /* UI_H_ */
