#ifndef affichage_h
#define affichage_h

#include <SDL2/SDL.h>

#define TAILLE_CELLULE 10.0f
#define LARGEUR_MONDE 200
#define HAUTEUR_MONDE 200

extern SDL_Window *window;
extern SDL_Renderer *renderer;


void dessiner_monde(int *monde, int offset_x, int offset_y);

#endif 
