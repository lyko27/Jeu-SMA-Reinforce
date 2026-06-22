#ifndef MONDE_H
#define MONDE_H

#include "goat.h"
#include "loup.h"
#include "fermier.h"

#define WIDTH_GOAT 50
#define HEIGHT_GOAT 30
#define WIDTH_WOLF 50
#define HEIGHT_WOLF 30
#define WIDTH_FERMIER 30
#define HEIGHT_FERMIER 60
#define VITESSE_FERMIER 5

// Le Monde
typedef struct
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
} monde;

typedef struct {
    float x;
    float y;
    float w;
    float h;
} Hitbox;


Hitbox creer_hitbox(float x, float y, float w, float h, float marge_x, float marge_y);
void free_goats(Goat **goats_tab, int nb_goat);
void free_wolf(Wolf **wolf_tab, int nombre_wolf);
monde *ajouter_goat(monde *monde_courant, Goat *goat);
monde *ajouter_wolf(monde *monde_courant, Wolf *wolf);
monde *creer_monde(int largeur, int hauteur);
monde *generer_un_monde(monde *monde_courant);
monde *mis_à_jour_monde(monde *monde_courant, int tick_animation, int input_x, int input_y);
int check_collision_rect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
void afficher_monde(monde *monde_courant);

#endif