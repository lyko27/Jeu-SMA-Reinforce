#ifndef affichage_h
#define affichage_h

#include <SDL2/SDL.h>

#define TAILLE_CELLULE 10.0f
#define LARGEUR_MONDE 200
#define HAUTEUR_MONDE 200

extern SDL_Window *window;
extern SDL_Renderer *renderer;


void quitter_affichage();
void dessiner_monde();
void actualiser_ecran();
void dessiner_entite(int type_entite, int position_x, int position_y,int frame,int direction);
int init_affichage();

#endif 
