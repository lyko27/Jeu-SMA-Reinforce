#ifndef MONDE_H
#define MONDE_H

#include "goat.h"
#include "loup.h"
#include "fermier.h"

#define LARGEUR 2048
#define HAUTEUR 1152
#define MARGE 90

// Zone du lac (obstacle)
#define LAC_X 1050
#define LAC_Y 803
#define LAC_WIDTH 195
#define LAC_HEIGHT 195

// Zone de la maison (obstacle)
#define MAISON_X 738
#define MAISON_Y 0
#define MAISON_WIDTH 175
#define MAISON_HEIGHT 195

#define WIDTH_GOAT 50
#define HEIGHT_GOAT 30
#define WIDTH_WOLF 50
#define HEIGHT_WOLF 30
#define WIDTH_FERMIER 30
#define HEIGHT_FERMIER 60
#define VITESSE_FERMIER 5

// Le Monde
typedef struct monde_t
{
    int largeur;
    int hauteur;
    Fermier *fermiers;
    Goat **goats_tab;
    int capacite_max_goat; // Taille allouée pour le tableau (pour ajouter des bébés chèvres)
    int nb_goat;           // Nombre actuel de chèvres
    Wolf **wolfs_tab;
    int capacite_max_wolf;
    int nb_wolf;
    int mode; // 0 = clavier pour le fermier, 1 = politique apprise
} monde;

typedef struct
{
    float x;
    float y;
    float w;
    float h;
} Hitbox;

Hitbox creer_hitbox(float x, float y, float w, float h, float marge_gauche, float marge_droite, float marge_haut, float marge_bas);
Hitbox get_hitbox_fermier(float x, float y);
Hitbox get_hitbox_goat(float x, float y);
Hitbox get_hitbox_wolf(float x, float y);

monde *creer_monde(int largeur, int hauteur);
monde *generer_un_monde(monde *monde_courant);
monde *mis_à_jour_monde(monde *monde_courant, int tick_animation, int input_x, int input_y);
int check_collision_rect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
int check_collision_obstacles(Hitbox hb);
void afficher_monde(monde *monde_courant);

// Inclusion des modules spécifiques restructurés
#include "monde_goat.h"
#include "monde_wolf.h"
#include "monde_fermier.h"

#endif