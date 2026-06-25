/**
 * @file utilisateur.c
 * @brief Fonction pour la gestion des interactions de l'utilisateur avec le jeu.
 * @details Gestion des boutons de deplacements, de pause, de mode de jeu.
 * @author Sohail
 * @date 18 Juin 2026
 */
#include "utilisateur.h"

/**
 * @brief Récupère les entrées de l'utilisateur (clavier et fermeture de fenêtre).
 * @param[in,out] resultat Structure qui contient toutes les interactions possibles avec le monde.
 * @return interaction_utilisateur* Un pointeur vers la structure mise à jour.
 */
interaction_utilisateur *recuperer_mouvement(interaction_utilisateur *resultat)
{
    const Uint8 *Keyborad = SDL_GetKeyboardState(NULL);
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            resultat->quitter = 1;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_SPACE)
            {
                resultat->pause = 1;
            }
            if (event.key.keysym.sym == SDLK_m)
            {
                resultat->switch_mode = 1;
            }
        default:
            break;
        }
    }
    if (Keyborad[SDL_SCANCODE_UP] || Keyborad[SDL_SCANCODE_Z] || Keyborad[SDL_SCANCODE_W])
    {
        resultat->y_deplacement = 1;
    }
    if (Keyborad[SDL_SCANCODE_DOWN] || Keyborad[SDL_SCANCODE_S])
    {
        resultat->y_deplacement = -1;
    }
    if (Keyborad[SDL_SCANCODE_LEFT] || Keyborad[SDL_SCANCODE_Q] || Keyborad[SDL_SCANCODE_A])
    {
        resultat->x_deplacement = 1;
    }
    if (Keyborad[SDL_SCANCODE_RIGHT] || Keyborad[SDL_SCANCODE_D])
    {
        resultat->x_deplacement = -1;
    }
    return resultat;
}
