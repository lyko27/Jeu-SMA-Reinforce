#include <stdio.h>
#include <SDL2/SDL_image.h>
#include "affichage.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture_fond = NULL;
SDL_Texture *texture_chevre = NULL;
SDL_Texture *texture_chevreau = NULL;
SDL_Texture *texture_fermier = NULL;
SDL_Texture *texture_wolf = NULL;

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
    
    window = SDL_CreateWindow("Jeu de la Ferme", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(renderer, 1024, 1024);
    
    texture_fond = IMG_LoadTexture(renderer, "./images/map.png");
    texture_chevre = IMG_LoadTexture(renderer, "./images/goat.png");
    texture_chevreau = IMG_LoadTexture(renderer, "./images/baby_goat.png");
    texture_fermier = IMG_LoadTexture(renderer, "./images/fermier_marche.png");
    texture_wolf = IMG_LoadTexture(renderer, "./images/loup.png");

    if (!texture_fond || !texture_chevre || !texture_chevreau || !texture_fermier) {
        printf("Erreur chargement image : %s\n", IMG_GetError());
        return 0;
    }
    return 1;
}

int gestion_direction_chevre(int direction){
    if (direction==1) return 3;
    else if (direction==2) return 1;
    else if (direction==3) return 0;
    else if (direction==4) return 2;
    return 0;
}

int gestion_direction_fermier(int direction){
    if (direction==1) return 0;
    else if (direction==2) return 3;
    else if (direction==3) return 2;
    else if (direction==4) return 1;
    return 2;
}


int gestion_direction_wolf(int direction){
    if (direction==1) return 2;
    else if (direction==2) return 1;
    else if (direction==3) return 0;
    else if (direction==4) return 3;
    else if (direction==0) return 2;
    return 0;
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
        zoom=1.5;
        texture_actuelle = texture_chevre;
        nb_image=4;
        nb_ligne=6;
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
    else if (type_entite == 4) { 
        zoom=0.5;
        texture_actuelle = texture_wolf;
        nb_image=4;
        nb_ligne=6;
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

SDL_Rect obtenir_hitbox_entite(int type_entite, int position_x, int position_y) {
    int nb_image = 0;
    int nb_ligne = 0;
    float zoom = 1.0;
    SDL_Texture* texture_actuelle = NULL;
    
    //On récupère les mêmes propriétés que pour le dessin selon l'entite (le sprite)
    if (type_entite == 1) { // Chèvre
        zoom = 1.5;
        texture_actuelle = texture_chevre;
        nb_image = 4;
        nb_ligne = 6;
    } 
    else if (type_entite == 2) { // Fermier
        zoom = 1.0;
        texture_actuelle = texture_fermier;
        nb_image = 9;
        nb_ligne = 4;
    }
    else if (type_entite == 3) { // Chevreau
        zoom = 1.0;
        texture_actuelle = texture_chevreau;
        nb_image = 4;
        nb_ligne = 5;
    }
    else if (type_entite == 4) { // Loup
        zoom = 0.5;
        texture_actuelle = texture_wolf;
        nb_image = 4;
        nb_ligne = 6;
    }

    //Calcul de la taille globale du sprite
    int texture_w = 0, texture_h = 0;
    SDL_QueryTexture(texture_actuelle, NULL, NULL, &texture_w, &texture_h);
    
    int offset_x = texture_w / nb_image; 
    int offset_y = texture_h / nb_ligne;

    SDL_Rect hitbox;
    hitbox.x = position_x;
    hitbox.y = position_y;
    hitbox.w = offset_x * zoom;
    hitbox.h = offset_y * zoom;

    // 3. Personnalisation de la taille de la hitbox on peut changer manuellement les marges
    // On réduit le rectangle pour coller au corps réel de l'entite
    int marge_x = 0;
    int marge_y = 0;

    if (type_entite == 1) {      // Chèvre : on retire par exemple 15px sur les côtés
        marge_x = 15; 
        marge_y = 10;
    }
    else if (type_entite == 2) { // Fermier : souvent plus vertical
        marge_x = 20; 
        marge_y = 5;
    }
    else if (type_entite == 4) { // Loup
        marge_x = 10;
        marge_y = 8;
    }

    // Application des marges pour recentrer la hitbox
    hitbox.x += marge_x;
    hitbox.y += marge_y;
    hitbox.w -= (marge_x * 2);
    hitbox.h -= (marge_y * 2);

    return hitbox;
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
    SDL_DestroyTexture(texture_wolf);
    SDL_DestroyTexture(texture_chevreau);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}



