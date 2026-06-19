#include <stdio.h>
#include <SDL2/SDL_image.h>
#include "affichage.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture_fond = NULL;
SDL_Texture *texture_chevre = NULL;
SDL_Texture *texture_chevreau = NULL;
SDL_Texture *texture_fermier = NULL;
SDL_Texture *texture_loup = NULL;

int init_affichage() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur SDL_Init : %s\n", SDL_GetError());
        return 0; 
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur IMG_Init : %s\n", IMG_GetError());
        SDL_Quit();
        return 0;
    }
    
    window = SDL_CreateWindow("Jeu de la Ferme", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 1024, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    texture_fond = IMG_LoadTexture(renderer, "./images/map.png");
    texture_chevre = IMG_LoadTexture(renderer, "./images/goat.png");
    texture_chevreau = IMG_LoadTexture(renderer, "./images/baby_goat.png");
    texture_fermier = IMG_LoadTexture(renderer, "./images/fermier_marche.png");
    texture_loup = IMG_LoadTexture(renderer, "./images/loup.png");

    if (!texture_fond || !texture_chevre || !texture_chevreau || !texture_fermier) {
        printf("Erreur chargement image : %s\n", IMG_GetError());
        return 0;
    }
    return 1;
}

int gestion_direction_chevre(int direction){
    if (direction==1) return 2;
    else if (direction==2) return 1;
    else if (direction==3) return 0;
    else if (direction==4) return 3;
}

int gestion_direction_fermier(int direction){
    if (direction==1) return 0;
    else if (direction==2) return 3;
    else if (direction==3) return 2;
    else if (direction==4) return 1;
}



void dessiner_entite(int type_entite, int position_x, int position_y,int frame,int direction) {
    int nb_image=0;
    int nb_ligne=0;
    float zoom=1.0;
    SDL_Texture* texture_actuelle = NULL;
    SDL_Rect 
         source = {0},                    // Rectangle définissant la zone totale de la planche
         destination = {0},               // Rectangle définissant où la zone_source doit être déposée dans le renderer
         state = {0};                     // Rectangle de la vignette en cours dans la planche 
       
    if (type_entite == 1) {
        texture_actuelle = texture_chevre;
        nb_image=4;
        nb_ligne=5;
        direction=gestion_direction_chevre(direction);
        } 
    else if (type_entite == 2) { 
        texture_actuelle = texture_fermier;
        nb_image=9;
        nb_ligne=4;
        direction=gestion_direction_fermier(direction);
        }
    else if (type_entite == 3) { 
        texture_actuelle = texture_chevreau;
        nb_image=4;
        nb_ligne=5;
    }
    SDL_QueryTexture(texture_actuelle, NULL, NULL, &source.w, &source.h);

    /*calcul de l'offset */
    int offset_x = source.w / nb_image; 
    int offset_y = source.h / nb_ligne;

    
    state.w = offset_x;
    state.h = offset_y;

    state.x = frame * offset_x;      // Décale en X selon l'étape de l'animation
    state.y = direction * offset_y;


   /* la position du sprite sur l ecran avec sa longuer et sa largeur*/
    destination.x = position_x; 
    destination.y = position_y;
    destination.w = offset_x * zoom;
    destination.h = offset_y * zoom;

    SDL_RenderCopy(renderer, texture_actuelle, &state, &destination);
}

void dessiner_monde() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture_fond, NULL, NULL);
}



void actualiser_ecran() {
    SDL_RenderPresent(renderer);
}

void quitter_affichage() {
    SDL_DestroyTexture(texture_fond);
    SDL_DestroyTexture(texture_chevre);
    SDL_DestroyTexture(texture_fermier);
    SDL_DestroyTexture(texture_chevreau);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}



