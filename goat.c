#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "goat.h"

 /**
 * Synopsis : Vérifie la collision entre deux objets rectangulaires.
 * Entrée   : Coordonnées (x1, y1) du premier objet, coordonnées (x2, y2) du deuxième objet.
 * Sortie   : 1 si collision, 0 sinon.
 */
int check_collision(float x1, float y1, float x2, float y2)
{
    if (x1 + WIDTH_GOAT < x2 || x2 + WIDTH_GOAT < x1 || y1 + HEIGHT_GOAT < y2 || y2 + HEIGHT_GOAT < y1)
    {
        return 0; // Pas de collision
    }
    return 1; // Collision
}

 /**
 * Synopsis : Initialise une chèvre avec des coordonnées aléatoires et une direction par défaut.
 * Entrée   : Pointeur vers la structure Goat à initialiser.
 * Sortie   : Pointeur vers la structure Goat initialisée.
 */
Goat *init_goat(Goat *g)
{
    g->x = rand() % (TAILLE_MAP - WIDTH_GOAT);
    g->y = rand() % (TAILLE_MAP - HEIGHT_GOAT);

    g->dir_x = g->x;// Initialisation, pas encore de direction
    g->dir_y = g->y;// Initialisation, pas encore de direction

    g->speed = 20;
    g->direction_sprite = 1 + (rand() % 4);
    g->frame = 0;

    return g;
}

 /**
 * Synopsis : Met à jour la position et la direction d'une chèvre en fonction d'un déplacement aléatoire et des collisions.
 * Entrée   : Pointeur vers la chèvre à mettre à jour, tableau de toutes les chèvres, nombre total de chèvres.
 * Sortie   : Pointeur vers la chèvre mise à jour.
 */
Goat* update_goat(Goat *g, Goat **all_goats, int nb_goats) {

    // On génère un angle entre 0 et 2Pi
    float angle = ((float)rand()/(float)RAND_MAX) * 2.0 * 3.14;

    float dx = cos(angle) * g->speed;
    float dy = sin(angle) * g->speed;

    float next_x = g->x + dx;
    float next_y = g->y + dy;

    // Choix du sprite le plus adapté
    if (fabs(dx) > fabs(dy)) {
        if (dx > 0) {
            g->direction_sprite = 2; // Droite
        } else {
            g->direction_sprite = 4; // Gauche
        }
    } else {
        if (dy > 0) {
            g->direction_sprite = 3; // Bas
        } else {
            g->direction_sprite = 1; // Haut
        }
    }

    // Collision avec les bords
    if (next_x < 0) next_x = 0;
    if (next_y < 0) next_y = 0;
    if (next_x > TAILLE_MAP - WIDTH_GOAT) next_x = TAILLE_MAP - WIDTH_GOAT;
    if (next_y > TAILLE_MAP - HEIGHT_GOAT) next_y = TAILLE_MAP - HEIGHT_GOAT;

    int collision = 0;
    for (int i = 0; i < nb_goats; i++) {
        if (all_goats[i] != g) {
            if (check_collision(next_x, next_y, all_goats[i]->dir_x, all_goats[i]->dir_y)) {
                collision = 1;
            }
        }
    }

    if (!collision) {
        g->dir_x = next_x;
        g->dir_y = next_y;
    }else {
        // Si collision, la chèvre reste sur place
        g->dir_x = g->x;
        g->dir_y = g->y;
    }

    return g;
}