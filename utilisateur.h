#ifndef UTILISATEUR_H
#define UTILISATEUR_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#define DEAD_ZONE 1.7 // partie de marge lor du déplacement à la souris où on considère un mouvement horizontal ou vertical non parfait

typedef struct evenement
{
    int x_deplacement;
    int y_deplacement;
    int pause;
    int quitter;
} interaction_utilisateur;

interaction_utilisateur *recuperer_mouvement(interaction_utilisateur *resultat);

#endif