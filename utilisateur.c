
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
                resultat->y_deplacement = 1;
            }
            else if (event.key.keysym.sym == SDLK_DOWN)
            {
                resultat->y_deplacement = -1;
            }
            else if (event.key.keysym.sym == SDLK_LEFT)
            {
                resultat->x_deplacement = 1;
            }
            else if (event.key.keysym.sym == SDLK_RIGHT)
            {
                resultat->x_deplacement = -1;
            }
            break;
        default:
            break;
        }
    }
    return resultat;
}
