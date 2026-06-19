#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "loup.h"

 /**
 * Synopsis : Vérifie la collision entre deux objets rectangulaires.
 * Entrée   : Coordonnées (x1, y1) du premier objet, coordonnées (x2, y2) du deuxième objet.
 * Sortie   : 1 si collision, 0 sinon.
 */
int check_collision_wolf(float x1, float y1, float x2, float y2)
{
    if (x1 + WIDTH_WOLF < x2 || x2 + WIDTH_WOLF < x1 || y1 + HEIGHT_WOLF < y2 || y2 + HEIGHT_WOLF < y1)
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
Wolf *init_wolf(Wolf *l)
{
    l->x = rand() % (TAILLE_MAP - WIDTH_WOLF);
    l->y = rand() % (TAILLE_MAP - HEIGHT_WOLF);

    l->dir_x = l->x;// Initialisation, pas encore de direction
    l->dir_y = l->y;// Initialisation, pas encore de direction

    l->speed = 0;
    l->direction_sprite = 1 + (rand() % 4);
    l->frame = 0;
    l->timer_mouvement=10;

    return l;
}

 /**
 * Synopsis : Met à jour la position et la direction d'une chèvre en fonction d'un déplacement aléatoire et des collisions.
 * Entrée   : Pointeur vers la chèvre à mettre à jour, tableau de toutes les chèvres, nombre total de chèvres.
 * Sortie   : Pointeur vers la chèvre mise à jour.
 */
Wolf* update_wolf(Wolf *l, Wolf **all_wolfs, int nb_wolfs) {

    l->timer_mouvement--;

    if (l->timer_mouvement <= 0) {
        int chance = rand() % 100;

        if (chance < 80) { // 80% de chance de s'arrêter
            l->en_mouvement = 0;
            l->speed = 0;
            l->timer_mouvement = 30 + (rand() % 90);  
        } 
        else { // 20% de chance de commencer à marcher
            l->en_mouvement = 1;
            // Vitesse du loup (tu peux l'augmenter si tu veux qu'il aille plus vite que les chèvres)
            l->speed = 2; 
            l->timer_mouvement = 30 + (rand() % 90);
            l->angle_actuel = ((float)rand()/(float)RAND_MAX) * 2.0 * 3.14;  // On génère un angle entre 0 et 2Pi
        }
    }
    if (l->en_mouvement == 0) { 
        l->dir_x = l->x;
        l->dir_y = l->y;
        return l; 
    }
    

    float dx = cos(l->angle_actuel) * l->speed;
    float dy = sin(l->angle_actuel) * l->speed;

    float next_x = l->x + dx;
    float next_y = l->y + dy;

    // Choix du sprite le plus adapté
    if (fabs(dx) > fabs(dy)) {
        if (dx > 0) {
            l->direction_sprite = 2; // Droite
        } else {
            l->direction_sprite = 4; // Gauche
        }
    } else {
        if (dy > 0) {
            l->direction_sprite = 3; // Bas
        } else {
            l->direction_sprite = 1; // Haut
        }
    }

    // Collision avec les bords
    if (next_x < 0) next_x = 0;
    if (next_y < 0) next_y = 0;
    if (next_x > TAILLE_MAP - WIDTH_WOLF) next_x = TAILLE_MAP - WIDTH_WOLF;
    if (next_y > TAILLE_MAP - HEIGHT_WOLF) next_y = TAILLE_MAP - HEIGHT_WOLF;

    int collision = 0;
    for (int i = 0; i < nb_wolfs; i++) {
        if (all_wolfs[i] != l) {
            if (check_collision_wolf(next_x, next_y, all_wolfs[i]->dir_x, all_wolfs[i]->dir_y)) {
                collision = 1;
            }
        }
    }

    if (!collision) {
        l->dir_x = next_x;
        l->dir_y = next_y;
    }else {
        // Si collision, la chèvre reste sur place
        l->dir_x = l->x;
        l->dir_y = l->y;
    }

    return l;
}