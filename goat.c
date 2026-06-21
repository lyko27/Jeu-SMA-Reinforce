#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "goat.h"

 /**
 * Synopsis : Vérifie la collision entre deux objets rectangulaires.
 * Entrée   : Coordonnées (x1, y1) du premier objet, coordonnées (x2, y2) du deuxième objet.
 * Sortie   : 1 si collision, 0 sinon.
 */
int check_collision_rect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    if (x1 + w1 < x2 || x2 + w2 < x1 || y1 + h1 < y2 || y2 + h2 < y1)
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

    g->speed = 0; // on initialise la vitesse des chevres a 0
    g->direction_sprite = 1 + (rand() % 4);
    g->frame = 0;
    g->timer_mouvement=10;
    g->en_mouvement=0;
    g->angle_actuel=0.0f;

    return g;
}

 /**
 * Synopsis : Met à jour la position et la direction d'une chèvre en fonction d'un déplacement aléatoire et des collisions.
 * Entrée   : Pointeur vers la chèvre à mettre à jour, tableau de toutes les chèvres, nombre total de chèvres.
 * Sortie   : Pointeur vers la chèvre mise à jour.
 */
Goat* update_goat(Goat *g, Goat **all_goats, int nb_goats) {

    //gestion du timer de mouvement on decremente pui on verifie si le timer est inferieur ou egale a 0 si oui on reinitialise la valeur a 10
    g->timer_mouvement--;

    if (g->timer_mouvement <= 0) {
        int chance = rand() % 100;

        if (chance < 80) { // 80% de chance de s'arrêter pour brouter
            g->en_mouvement = 0;
            // Elle restera immobile entre 30 et 120 frames (0.5s à 2s si le jeu tourne à 60 FPS)
            g->speed = 0;
            g->timer_mouvement = 30 + (rand() % 90);  //on genere un nombre aleatoire entre 0 et 120 apres l'arret pour brouter
        } 
        else { // 20% de chance de commencer à marcher
            g->en_mouvement = 1;
            g->speed=2;
            g->timer_mouvement = 30 + (rand() % 90);
            g->angle_actuel = ((float)rand()/(float)RAND_MAX) * 2.0 * 3.14;  //generation d'un nouvel angle lorsqu on commence a marcher a nouveau
        }
    }
    //pourcentage de chances que le goat ne bouge pas 80 % de chances
    if (g->en_mouvement==0) { 
        g->dir_x = g->x;
        g->dir_y = g->y;
        return g; 
    }
    // On génère un angle entre 0 et 2Pi

    float dx = cos(g->angle_actuel) * g->speed;
    float dy = sin(g->angle_actuel) * g->speed;

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
            if (check_collision_rect(next_x, next_y, WIDTH_GOAT, HEIGHT_GOAT, all_goats[i]->dir_x, all_goats[i]->dir_y, WIDTH_GOAT, HEIGHT_GOAT))
            {
                collision = 1;
                g->timer_mouvement = 0; // s'il ya collision la chevre arrete de marcher
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

void evaluer_interets(Goat *current_goat, Perception perception) 
{
    // intérêts par défaut
    current_goat->table_interets[ACTION_ERRER] = 0.0;
    current_goat->table_interets[ACTION_FUIR_LOUP] = 0.0;

    // modif selon la Perception
    if (perception.dist_loup_proche < 150.0) 
    {
        // on fuit si le loup est très proche
        current_goat->table_interets[ACTION_FUIR_LOUP] = 10.0 - (perception.dist_loup_proche / 15.0);
        current_goat->table_interets[ACTION_ERRER] = -10.0f;
    }
    else
    {
        // sinon la chèvre chill
        current_goat->table_interets[ACTION_FUIR_LOUP] = -10.0;
        current_goat->table_interets[ACTION_ERRER] = 10.0;
    }
}