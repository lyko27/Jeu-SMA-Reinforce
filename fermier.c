#include "fermier.h"

void init_fermier(Fermier *fermier) {
    fermier->x = TAILLE_MAP / 2.0f;
    fermier->y = TAILLE_MAP / 2.0f;
    fermier->speed = VITESSE_FERMIER;
}

void update_fermier(Fermier *fermier, int x, int y) {
    if (x=0){
        if(y=1)
        if(y=-1)
    }
    if (y=0){
        if(x=1)
        if(x=-1)
    }   

    if (fermier->x < 0) fermier->x = 0;
    if (fermier->x + WIDTH_FERMIER > TAILLE_MAP) fermier->x = TAILLE_MAP - WIDTH_FERMIER;
    if (fermier->y < 0) fermier->y = 0;
    if (fermier->y + HEIGHT_FERMIER > TAILLE_MAP) fermier->y = TAILLE_MAP - HEIGHT_FERMIER;
}