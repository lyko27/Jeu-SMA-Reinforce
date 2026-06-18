#include "affichage.h"
#include <stdio.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture_fond = NULL;
SDL_Texture *texture_chevre = NULL;
SDL_Texture *texture_chevreau = NULL;
SDL_Texture *texture_fermier = NULL;


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
    
    window = SDL_CreateWindow("Jeu de la Ferme", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    texture_fond = IMG_LoadTexture(renderer, "./img/map.png");
    texture_chevre = IMG_LoadTexture(renderer, "./img/goat.png");
    texture_chevreau = IMG_LoadTexture(renderer, "./img/baby_goat.png");
    texture_fermier = IMG_LoadTexture(renderer, "./img/fermier.png");
    
    if (!texture_fond || !texture_chevre || !texture_chevreau || !texture_fermier) {
        printf("Erreur chargement image : %s\n", IMG_GetError());
        return 0;
    }
    return 1;
}


void dessiner_entite(int type_entite, int position_x, int position_y,int frame) {
    
    if (type_entite == 1) {
        //ajouter des if type entite pour chaque entite pour faire la decoupe de l image pour voir que partie du sprite utiliser
        SDL_Rect destination = {position_x, position_y, 64, 64};
        SDL_RenderCopy(renderer, texture_chevre, NULL, &destination);
    } 
    else if (type_entite == 2) { 
        SDL_Rect destination = {position_x, position_y, 64, 64};
        SDL_RenderCopy(renderer, texture_fermier, NULL, &destination);
    }
    else if (type_entite == 3) { 
        SDL_Rect destination = {position_x, position_y, 64, 64};
        SDL_RenderCopy(renderer, texture_chevreau, NULL, &destination);
    }
}

void dessiner_monde() {
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



