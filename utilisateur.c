
#include "utilisateur.h"

interaction_utilisateur * recupere_mouvement(interaction_utilisateur *resultat)
{
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
            else if (event.key.keysym.sym == SDLK_UP)
            {
                resultat->deplacement_y = 1;
            }
            else if (event.key.keysym.sym == SDLK_DOWN)
            {
                resultat->deplacement_y = -1;
            }
            else if (event.key.keysym.sym == SDLK_LEFT)
            {
                resultat->deplacement_x = 1;
            }
            else if (event.key.keysym.sym == SDLK_RIGHT)
            {
                resultat->deplacement_x = -1;
            }
            break;
        default:
            break;
        }
    }
    return resultat;
}
