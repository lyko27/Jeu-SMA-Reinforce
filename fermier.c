#include "fermier.h"

void init_fermier(Fermier *fermier) {
    fermier->x = 347;
    fermier->y = 185;
    fermier->speed = VITESSE_FERMIER;
}

void update_fermier(Fermier *fermier, int x, int y) {
    if (x=0){
        if(y=1){fermier->y+=VITESSE_FERMIER;}
        if(y=-1){fermier->y-=VITESSE_FERMIER;}
    }
    if (y=0){
        if(x=1){fermier->x+=VITESSE_FERMIER;}
        if(x=-1){fermier->x-=VITESSE_FERMIER;}
    }   

    if (fermier->x < 0) fermier->x = 0;
    if (fermier->x + WIDTH_FERMIER > TAILLE_MAP) fermier->x = TAILLE_MAP - WIDTH_FERMIER;
    if (fermier->y < 0) fermier->y = 0;
    if (fermier->y + HEIGHT_FERMIER > TAILLE_MAP) fermier->y = TAILLE_MAP - HEIGHT_FERMIER;
}