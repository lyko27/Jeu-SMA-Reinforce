#include "fermier.h"

 /**
 * Synopsis : Initialise le fermier devant sa maison.
 * Entrée   : Pointeur vers la structure Fermier à initialiser.
 * Sortie   : Pointeur vers la structure Fermier initialisée.
 */
Fermier * init_fermier(Fermier *fermier) {
    fermier->x = 347;
    fermier->y = 185;
    fermier->speed = VITESSE_FERMIER;
    return fermier;
}

 /**
 * Synopsis : Met à jour la position du fermier en fonction des flèches directionnelles saisies et gère les collisions avec les bords de la carte.
 * Entrée   : Pointeur vers le fermier à mettre à jour, direction de déplacement en x, direction de déplacement en y.
 * Sortie   : Pointeur vers le fermier mis à jour.
 */
Fermier *update_fermier(Fermier *fermier, int x, int y)
{
    float next_x = fermier->x;
    float next_y = fermier->y;
    if (x == 0)
    {
        if (y == 1)
        {
            next_y -= VITESSE_FERMIER;
            fermier->direction_sprite = 1;
        }
        if (y == -1)
        {
            next_y += VITESSE_FERMIER;
            fermier->direction_sprite = 3;
        }
    }
    if (y == 0)
    {
        if (x == 1)
        {
            next_x -= VITESSE_FERMIER;
            fermier->direction_sprite = 4;
        }
        if (x == -1)
        {
            next_x += VITESSE_FERMIER;
            fermier->direction_sprite = 2;
        }
    }
    if (x == 0 && y == 0)
    {
        fermier->frame = 5;
        fermier->direction_sprite = 0;
        return fermier;
    }

    if (next_x < 0)
        next_x = 0;
    if (next_x + WIDTH_FERMIER > TAILLE_MAP)
        next_x = TAILLE_MAP - WIDTH_FERMIER;
    if (next_y < 0)
        next_y = 0;
    if (next_y + HEIGHT_FERMIER > TAILLE_MAP)
        next_y = TAILLE_MAP - HEIGHT_FERMIER;

    fermier->x = next_x;
    fermier->y = next_y;

    return fermier;
}