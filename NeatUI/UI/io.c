#include "io.h"

int read_int(int *target, int min, int max) {

    int val;
    char ch;
    char buffer[20];

    if( !fgets(buffer, 20, stdin)) {
        return 0;
    }

    if( sscanf(buffer, " %d%c", &val, &ch) != 2 || ch != '\n' || val < min || val > max ) {
        return 0;
    }

    *target = val;
    return 1;
}

int read_double(double *target, double min, double max) {

    double val;
    char ch;
    char buffer[64];
    if( !fgets(buffer, 64, stdin)) {
        return 0;
    }

    if( sscanf(buffer, " %lf%c", &val, &ch) != 2 || ch != '\n' || val < min || val > max ) {
        return 0;
    }

    *target = val;
    return 1;
}
