#ifndef affichage_h
#define affichage_h

#include <SDL2/SDL.h>

#define TAILLE_CELLULE 10.0f
#define LARGEUR_MONDE 200
#define HAUTEUR_MONDE 200

extern SDL_Window *window;
extern SDL_Renderer *renderer;

void afficher_planche(int nb_chevres, int nb_loups);
void quitter_affichage();
void dessiner_monde();
void actualiser_ecran();
void dessiner_entite(int type_entite, int position_x, int position_y, int frame, int direction);
int init_affichage();
void afficher_fin();
void afficher_mode(int mode);
void afficher_pause();

#endif


