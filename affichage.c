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
SDL_Texture *texture_planche = NULL;
SDL_Texture *texture_pause = NULL;
SDL_Texture *texture_mode = NULL;
SDL_Texture *texture_fin = NULL;
TTF_Font *police_compteur = NULL;

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
    texture_pause = IMG_LoadTexture(renderer, "./images/reprendre.png"); 
    texture_mode = IMG_LoadTexture(renderer, "./images/mode.png");   
    texture_fin = IMG_LoadTexture(renderer, "./images/fin.png");

    if (!texture_fond || !texture_chevre || !texture_chevreau || !texture_fermier || !texture_planche || !texture_pause || !texture_mode || !texture_fin)
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

    SDL_QueryTexture(texture_planche, NULL, NULL, &position_planche.w, &position_planche.h);

    // Zoom de la plamche
    position_planche.w *= 1.5;
    position_planche.h *= 1.5;

    SDL_RenderCopy(renderer, texture_planche, NULL, &position_planche);

    // Choix la couleur du texte 
    SDL_Color couleur = {0, 0, 0, 255};

    // Affichage du compteur des chèvres
    char texte_chevres[64];
    snprintf(texte_chevres, sizeof(texte_chevres), "%d", nb_chevres);
    SDL_Surface *surface_chevres = TTF_RenderText_Solid(police_compteur, texte_chevres, couleur);
    if (surface_chevres)
    {
        SDL_Texture *texture_texte_chevres = SDL_CreateTextureFromSurface(renderer, surface_chevres);
        SDL_Rect pos_texte_chevres;
        // position du texte pour le mettre dans la planche
        pos_texte_chevres.x = position_planche.x + 550;
        pos_texte_chevres.y = position_planche.y + 50;
        pos_texte_chevres.w = surface_chevres->w;
        pos_texte_chevres.h = surface_chevres->h;

        SDL_RenderCopy(renderer, texture_texte_chevres, NULL, &pos_texte_chevres);
        SDL_FreeSurface(surface_chevres);
        SDL_DestroyTexture(texture_texte_chevres);
    }

    // Affichage du compteur des loups
    char texte_loups[64];
    snprintf(texte_loups, sizeof(texte_loups), "%d", nb_loups);
    SDL_Surface *surface_loups = TTF_RenderText_Blended(police_compteur, texte_loups, couleur);

    if (surface_loups)
    {
        SDL_Texture *texture_texte_loups = SDL_CreateTextureFromSurface(renderer, surface_loups);
        SDL_Rect pos_texte_loups;
        // position du texte du loup sur la planche
        pos_texte_loups.x = position_planche.x + 230;
        pos_texte_loups.y = position_planche.y + 50;
        pos_texte_loups.w = surface_loups->w;
        pos_texte_loups.h = surface_loups->h;

        SDL_RenderCopy(renderer, texture_texte_loups, NULL, &pos_texte_loups);
        SDL_FreeSurface(surface_loups);
        SDL_DestroyTexture(texture_texte_loups);
    }
}

void afficher_mode(int mode)
{
    if (!police_compteur || !texture_mode)
        return;

    // Afficher la planche en bois en arrière-plan du HUD
    SDL_Rect position_mode;
    position_mode.x = 1200; // Décalage depuis la gauche
    position_mode.y = 20; // Décalage depuis le haut

    SDL_QueryTexture(texture_planche, NULL, NULL, &position_mode.w, &position_mode.h);

    // Zoom de la plamche
    position_mode.w *= 1.5;
    position_mode.h *= 1.5;

    SDL_RenderCopy(renderer, texture_mode, NULL, &position_mode);

    // Choix la couleur du texte 
    SDL_Color couleur = {0, 0, 0, 255};

    // Affichage du compteur des chèvres
    char texte_mode[64];
    if (mode==1){
    snprintf(texte_mode, sizeof(texte_mode), "%s", "AUTOMATIQUE");
    }
    else{
        snprintf(texte_mode, sizeof(texte_mode), "%s", "MANUEL");
    }
    SDL_Surface *surface_mode = TTF_RenderText_Solid(police_compteur, texte_mode, couleur);
    if (surface_mode)
    {
        SDL_Texture *texture_texte_mode = SDL_CreateTextureFromSurface(renderer, surface_mode);
        SDL_Rect pos_texte_mode;
        // position du texte pour le mettre dans la planche
        pos_texte_mode.x = position_mode.x + 400;
        pos_texte_mode.y = position_mode.y + 50;
        pos_texte_mode.w = surface_mode->w;
        pos_texte_mode.h = surface_mode->h;

        SDL_RenderCopy(renderer, texture_texte_mode, NULL, &pos_texte_mode);
        SDL_FreeSurface(surface_mode);
        SDL_DestroyTexture(texture_texte_mode);
    }
}

void afficher_pause()
{

    // on vérifie que l'image a bien été chargée
    if (!texture_pause) return;

    // On dessine un rectangle noir semi-transparent sur tout l'écran
    // Cela permet de griser le jeu en arrière-plan et de mettre le menu en valeur
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);//definir comment la couleur et l image vont se blend se melanger superposition en presant compre la transparence
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150); // 150 correspond à l'opacité (0 = invisible, 255 = totalement noir)
    SDL_Rect fond_assombri = {0, 0, LARGEUR, HAUTEUR};
    SDL_RenderFillRect(renderer, &fond_assombri);
    // ------------------------------------------------

    SDL_Rect destination;
    
    // Récupérer les dimensions largeur et hauteur de l'image
    SDL_QueryTexture(texture_pause, NULL, NULL, &destination.w, &destination.h);

    // Calculer les coordonnées exactes pour que l'image soit parfaitement au centre
    destination.x = (LARGEUR - destination.w) / 2;
    destination.y = (HAUTEUR - destination.h) / 2;

    // Afficher la texture sur l'écran
    SDL_RenderCopy(renderer, texture_pause, NULL, &destination);
}
    

void afficher_fin()
{

    // on vérifie que l'image a bien été chargée
    if (!texture_fin) return;

    // On dessine un rectangle noir semi-transparent sur tout l'écran
    // Cela permet de griser le jeu en arrière-plan et de mettre le menu en valeur
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);//definir comment la couleur et l image vont se blend se melanger superposition en presant compre la transparence
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150); // 150 correspond à l'opacité (0 = invisible, 255 = totalement noir)
    SDL_Rect fond_assombri = {0, 0, LARGEUR, HAUTEUR};
    SDL_RenderFillRect(renderer, &fond_assombri);
    // ------------------------------------------------

    SDL_Rect destination;
    
    // Récupérer les dimensions largeur et hauteur de l'image
    SDL_QueryTexture(texture_fin, NULL, NULL, &destination.w, &destination.h);

    // Calculer les coordonnées exactes pour que l'image soit parfaitement au centre
    destination.x = (LARGEUR - destination.w) / 2;
    destination.y = (HAUTEUR - destination.h) / 2;

    // Afficher la texture sur l'écran
    SDL_RenderCopy(renderer, texture_fin, NULL, &destination);
}

void dessiner_entite(int type_entite, int position_x, int position_y, int frame, int direction)
{
    int nb_image = 0;
    int nb_ligne = 0;
    float zoom = 1.0;
    SDL_Texture *texture_actuelle = NULL;
    SDL_Rect
        source = {0},      // Rectangle pour le dessin des entites
        destination = {0}, 
        state = {0};       

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
    SDL_DestroyTexture(texture_pause);
    SDL_DestroyTexture(texture_mode);
    SDL_DestroyTexture(texture_fond);
    SDL_DestroyTexture(texture_chevre);
    SDL_DestroyTexture(texture_fermier);
    SDL_DestroyTexture(texture_wolf);
    SDL_DestroyTexture(texture_chevreau);
    SDL_DestroyTexture(texture_planche);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(texture_fin);
    IMG_Quit();
    SDL_Quit();
}

