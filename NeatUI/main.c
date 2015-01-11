#include <stdio.h>
#include <stdlib.h>

#include "Environment/env.h"
#include "UI/ui.h"
#include "Map/map.h"

void run_iteration(Interface ifce, Map map, Environment env);

int main(int argc, char *argv[]) {

    Environment env = new_env();
    Interface ifce = new_interface();
    Map map;


    while ( ifce->draw_ui(ifce, env) ) {
        map = new_map( env );
        if( map != NULL ) {
            run_iteration(ifce, map, env );
            dispose_map( &map );
        } else {
            ifce->popup_allert(ifce, "Konnte nicht genug Speicher allocieren" );
        }
    }
    dispose_env( &env );
    dispose_interface( &ifce );
    return 0;
}

void run_iteration(Interface ifce, Map map, Environment env) {
    ifce->renderer->clear(ifce->renderer);
    do {
        if(map->it_count % env->r_stepsize == 0) {
            ifce->draw_map(ifce, map, env);
        }
    } while(map->iterate(map) > env->acc);

    ifce->draw_map(ifce, map, env);
    ifce->popup_allert(ifce, "Simulation beendet");
}
