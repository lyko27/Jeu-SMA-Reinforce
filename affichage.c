#include "affichage.h"
#include <stdio.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture_fond = NULL;
SDL_Texture *texture_chevre = NULL;
SDL_Texture *texture_chevreau = NULL;
SDL_Texture *texture_fermier = NULL;





int init_affichage(){
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            printf("Erreur SDL_Init : %s\n", SDL_GetError());
            return 1;
        }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur IMG_Init : %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }
    window = SDL_CreateWindow("Affichage Image", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
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

void dessiner_monde() {
    SDL_RenderCopy(renderer, texture_fond, NULL, NULL);
}

int deplacement(int i_initial,int i_final){
    if (i_initial < i_final) {
        i_initial++;
        }
    else if(i_initial > i_final) {
        i_initial--;
    return i_initial;
        }
    }

void dessiner_entite(int type_entite, int x_initial, int y_initial, int x_final, int y_final) {
    
    int prochain_x=x_initial;
    int prochain_y=y_initial;
    if (type_entite == 1) {
        while (prochain_x < x_final || prochain_y < y_final || prochain_x > x_final || prochain_y >y_final ) {
            prochain_x=deplacement(prochain_x,x_final);
            prochain_y=deplacement(prochain_y,y_final);
            SDL_Rect destination = {x_initial, y_initial, 64, 64};
            SDL_RenderCopy(renderer, texture_chevre, NULL, &destination);
            SDL_RenderPresent(renderer);
            SDL_Delay(10); 
        }
        SDL_Rect destination = {x, y, 64, 64};
        SDL_RenderCopy(renderer, texture_chevre, NULL, &destination);
    } 
    else if (type_entite == 2) { 
        while (prochain_x < x_final || prochain_y < y_final || prochain_x > x_final || prochain_y >y_final ) {
            prochain_x=deplacement(prochain_x,x_final);
            prochain_y=deplacement(prochain_y,y_final);
            SDL_Rect destination = {x_initial, y_initial, 64, 64};
            SDL_RenderCopy(renderer, texture_chevre, NULL, &destination);
            SDL_RenderPresent(renderer);
            SDL_Delay(10); 
        }
        SDL_Rect destination = {x, y, 64, 64};
        SDL_RenderCopy(renderer, texture_fermier, NULL, &destination);
    }
    else if (type_entite == 3) { 
        while (prochain_x < x_final || prochain_y < y_final || prochain_x > x_final || prochain_y >y_final ) {
            prochain_x=deplacement(prochain_x,x_final);
            prochain_y=deplacement(prochain_y,y_final);
            SDL_Rect destination = {x_initial, y_initial, 64, 64};
            SDL_RenderCopy(renderer, texture_chevre, NULL, &destination);
            SDL_RenderPresent(renderer);
            SDL_Delay(10); 
        }
        SDL_Rect destination = {x, y, 64, 64};
        SDL_RenderCopy(renderer, texture_chevreau, NULL, &destination);
    }
}

void actualiser_ecran() {
    SDL_RenderPresent(renderer);
}

void quitter_affichage() {
    SDL_DestroyTexture(texture_fond);
    SDL_DestroyTexture(texture_chevre);
    SDL_DestroyTexture(texture_fermier);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}



