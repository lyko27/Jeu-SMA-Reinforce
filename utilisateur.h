/**
 * @file utilisateur.h
 * @brief Déclarations des structures et fonctions d'interactions avec l'utilisateurs.
 * @details Gestion des boutons de deplacements, de pause, de mode de jeu.
 * @author Sohail
 * @date 18 Juin 2026
 */
#ifndef UTILISATEUR_H
#define UTILISATEUR_H

#include <SDL2/SDL.h>
#include <stdbool.h>

/**
 * @struct interaction_utilisateur
 * @brief Structure qui gere le deplacement, la mise en pause du jeu, quitter le jeu, changer de mode suivant les entrees de l'utilisateur.
 * */
typedef struct evenement
{
    int x_deplacement;
    int y_deplacement;
    int pause;
    int quitter;
    int switch_mode;
} interaction_utilisateur;

interaction_utilisateur *recuperer_mouvement(interaction_utilisateur *resultat);

#endif