#ifndef fermier_h
#define fermier_h

#include "utilisateur.h"

#define VITESSE_FERMIER 5.0f
#define WIDTH_FERMIER 50
#define HEIGHT_FERMIER 30
#define TAILLE_MAP 1024

typedef struct {
    float x;
    float y;
    float speed;
    int frame;
    int direction_sprite; // 1 haut, 2 droite, 3 bas, 4,gauche
} Fermier;

Fermier * init_fermier(Fermier *fermier);

Fermier * update_fermier(Fermier *fermier, int x, int y);

#endif
