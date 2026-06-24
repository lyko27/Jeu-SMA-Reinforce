#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "monde.h"
#include "affichage.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture_fond = NULL;
SDL_Texture *texture_chevre = NULL;
SDL_Texture *texture_chevreau = NULL;
SDL_Texture *texture_fermier = NULL;
SDL_Texture *texture_wolf = NULL;
TTF_Font *police_compteur = NULL;
SDL_Texture *texture_planche = NULL;

int init_affichage()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Erreur SDL_Init : %s\n", SDL_GetError());
        return 0;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("Erreur IMG_Init : %s\n", IMG_GetError());
        SDL_Quit();
        return 0;
    }

    if (TTF_Init() == -1)
    {
        printf("Attention Erreur TTF_Init : %s\n", TTF_GetError());
    }
    else
    {
        police_compteur = TTF_OpenFont("./fonts/PixeloidSans.ttf", 24);
        if (!police_compteur)
        {
            printf("Attention : Impossible de charger la police. Erreur : %s\n", TTF_GetError());
        }
    }

    window = SDL_CreateWindow("Jeu de la Ferme", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LARGEUR, HAUTEUR, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(renderer, LARGEUR, HAUTEUR);

    texture_fond = IMG_LoadTexture(renderer, "./images/map.png");
    texture_chevre = IMG_LoadTexture(renderer, "./images/goat.png");
    texture_chevreau = IMG_LoadTexture(renderer, "./images/baby_goat.png");
    texture_fermier = IMG_LoadTexture(renderer, "./images/fermier_marche.png");
    texture_wolf = IMG_LoadTexture(renderer, "./images/loup.png");
    texture_planche = IMG_LoadTexture(renderer, "./images/ath.png");

    if (!texture_fond || !texture_chevre || !texture_chevreau || !texture_fermier || !texture_planche)
    {
        printf("Erreur chargement image : %s\n", IMG_GetError());
        return 0;
    }
    return 1;
}

int gestion_direction_chevre(int direction)
{
    if (direction == 1)
        return 3;
    else if (direction == 2)
        return 1;
    else if (direction == 3)
        return 0;
    else if (direction == 4)
        return 2;
    return 0;
}

int gestion_direction_fermier(int direction)
{
    if (direction == 1)
        return 0;
    else if (direction == 2)
        return 3;
    else if (direction == 3)
        return 2;
    else if (direction == 4)
        return 1;
    return 2;
}

int gestion_direction_wolf(int direction)
{
    if (direction == 1)
        return 3;
    else if (direction == 2)
        return 2;
    else if (direction == 3)
        return 0;
    else if (direction == 4)
        return 1;
    return 0;
}

// Modification de la fonction pour prendre en compte les deux compteurs de nombre de chevres et de loups vivants
void afficher_planche(int nb_chevres, int nb_loups)
{
    if (!police_compteur || !texture_planche)
        return;

    // Afficher la planche en bois en arrière-plan du HUD
    SDL_Rect position_planche;
    position_planche.x = 20; // Décalage depuis la gauche
    position_planche.y = 20; // Décalage depuis le haut
    // Tu peux forcer une taille (ex: position_hud.w = 300; position_hud.h = 100;)
    // ou récupérer la taille originale de l'image avec SDL_QueryTexture :
    SDL_QueryTexture(texture_planche, NULL, NULL, &position_planche.w, &position_planche.h);

    // Zoomer un peu la planche si elle est trop petite (optionnel)
    position_planche.w *= 1.5;
    position_planche.h *= 1.5;

    SDL_RenderCopy(renderer, texture_planche, NULL, &position_planche);

    // Choisir la couleur du texte (ex: un marron foncé ou noir pour bien ressortir sur le bois)
    SDL_Color couleur = {0, 0, 0, 255};

    // 2. Afficher le compteur des chèvres
    char texte_chevres[64];
    snprintf(texte_chevres, sizeof(texte_chevres), "%d", nb_chevres);
    SDL_Surface *surface_chevres = TTF_RenderText_Solid(police_compteur, texte_chevres, couleur);
    if (surface_chevres)
    {
        SDL_Texture *texture_texte_chevres = SDL_CreateTextureFromSurface(renderer, surface_chevres);
        SDL_Rect pos_texte_chevres;
        // Ajuste ces coordonnées (x, y) pour bien placer le texte à côté de la tête de chèvre sur l'image
        pos_texte_chevres.x = position_planche.x + 550;
        pos_texte_chevres.y = position_planche.y + 50;
        pos_texte_chevres.w = surface_chevres->w;
        pos_texte_chevres.h = surface_chevres->h;

        SDL_RenderCopy(renderer, texture_texte_chevres, NULL, &pos_texte_chevres);
        SDL_FreeSurface(surface_chevres);
        SDL_DestroyTexture(texture_texte_chevres);
    }

    // Afficher le compteur des loups
    char texte_loups[64];
    snprintf(texte_loups, sizeof(texte_loups), "%d", nb_loups);
    SDL_Surface *surface_loups = TTF_RenderText_Blended(police_compteur, texte_loups, couleur);

    if (surface_loups)
    {
        SDL_Texture *texture_texte_loups = SDL_CreateTextureFromSurface(renderer, surface_loups);
        SDL_Rect pos_texte_loups;
        // Ajuste ces coordonnées pour bien placer le texte à côté de la tête du loup
        pos_texte_loups.x = position_planche.x + 230;
        pos_texte_loups.y = position_planche.y + 50;
        pos_texte_loups.w = surface_loups->w;
        pos_texte_loups.h = surface_loups->h;

        SDL_RenderCopy(renderer, texture_texte_loups, NULL, &pos_texte_loups);
        SDL_FreeSurface(surface_loups);
        SDL_DestroyTexture(texture_texte_loups);
    }
}

void dessiner_entite(int type_entite, int position_x, int position_y, int frame, int direction)
{
    int nb_image = 0;
    int nb_ligne = 0;
    float zoom = 1.0;
    SDL_Texture *texture_actuelle = NULL;
    SDL_Rect
        source = {0},      // Rectangle définissant la zone totale de la planche
        destination = {0}, // Rectangle définissant où la zone_source doit être déposée dans le renderer
        state = {0};       // Rectangle de la vignette en cours dans la planche

    if (type_entite == 1)
    {
        zoom = 1.5;
        texture_actuelle = texture_chevre;
        nb_image = 4;
        nb_ligne = 6;
        direction = gestion_direction_chevre(direction);
    }
    else if (type_entite == 2)
    {
        texture_actuelle = texture_fermier;
        nb_image = 9;
        nb_ligne = 4;
        direction = gestion_direction_fermier(direction);
    }
    else if (type_entite == 3)
    {
        texture_actuelle = texture_chevreau;
        nb_image = 4;
        nb_ligne = 5;
    }

    else if (type_entite == 4) { 
        zoom=0.6;

        texture_actuelle = texture_wolf;
        nb_image = 3;
        nb_ligne = 4;
        direction = gestion_direction_wolf(direction);
    }

    SDL_QueryTexture(texture_actuelle, NULL, NULL, &source.w, &source.h);

    /*calcul de l'offset */
    int offset_x = source.w / nb_image;
    int offset_y = source.h / nb_ligne;

    state.w = offset_x;
    state.h = offset_y;

    state.x = frame * offset_x; // Décale en X selon l'étape de l'animation
    state.y = direction * offset_y;

    /* la position du sprite sur l ecran avec sa longuer et sa largeur*/
    destination.x = position_x;
    destination.y = position_y;
    destination.w = offset_x * zoom;
    destination.h = offset_y * zoom;

    SDL_RenderCopy(renderer, texture_actuelle, &state, &destination);
}

void dessiner_monde()
{
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture_fond, NULL, NULL);
}

void actualiser_ecran()
{
    SDL_RenderPresent(renderer);
}

void quitter_affichage()
{
    if (police_compteur != NULL)
    {
        TTF_CloseFont(police_compteur);
        police_compteur = NULL;
    }
    TTF_Quit();
    SDL_DestroyTexture(texture_fond);
    SDL_DestroyTexture(texture_chevre);
    SDL_DestroyTexture(texture_fermier);
    SDL_DestroyTexture(texture_wolf);
    SDL_DestroyTexture(texture_chevreau);
    SDL_DestroyTexture(texture_planche);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
