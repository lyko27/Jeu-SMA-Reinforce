#include "affichage.h"
#include <stdio.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture_fond = NULL;
SDL_Texture *texture_chevre = NULL;
SDL_Texture *texture_chevreau = NULL;
SDL_Texture *texture_fermier = NULL;





void dessiner_entite(int type_entite, int position_x, int position_y,int frame) {
    
    if (type_entite == 1) {
        //ajouter des if pour chaque entite pour faire la decoupe de l image pour voir que partie du sprite utiliser
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
        SDL_Rect destination = {prochain_x, prochain_y, 64, 64};
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
        SDL_Rect destination = {prochain_x, prochain_y, 64, 64};
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
        SDL_Rect destination = {prochain_x, prochain_y, 64, 64};
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



