#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "goat.h"

int check_collision(float x1, float y1, float x2, float y2) {
    if ((x1+WIDTH_GOAT<x2 || x2+WIDTH_GOAT<x1)&&(y1+HEIGHT_GOAT<y2 || y2+HEIGHT_GOAT<y1)){
        return 0;
    }
    return 1;
}