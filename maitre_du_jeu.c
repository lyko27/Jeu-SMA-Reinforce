#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "goat.h"
#include "loup.h"
#include "utilisateur.h"
#include "affichage.h"
#include "fermier.h"
#include "monde.h"

#define LARGEUR 1600
#define HAUTEUR 1600

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    srand(time(NULL));
    // création et initialisation du monde
    monde *monde_courrant = creer_monde(LARGEUR, HAUTEUR);
    monde_courrant = generer_un_monde(monde_courrant);
    int quiiter_le_programme = 0; // variable de gestion de la boucle
    int en_pause = 0;
    int tick_animation = 0;
    init_affichage();
    while (quiiter_le_programme == 0)
    {
        /* recuperation des  input utilisateur */
        interaction_utilisateur *utilisateur = malloc(sizeof(interaction_utilisateur));
        if (utilisateur)
        {
            utilisateur->x_deplacement = 0;
            utilisateur->y_deplacement = 0;
            utilisateur->pause = 0;
            utilisateur->quitter = 0;
            utilisateur = recuperer_mouvement(utilisateur);
            quiiter_le_programme = utilisateur->quitter; // si quitter = 1 le programme prendra fin à a la fin de la boucle

            if (utilisateur->pause)
            {
                en_pause = !en_pause;
            }
            if (!en_pause)
            {
                tick_animation++;
                // Toute la logique de mise à jour (Fermier, Wolf, Chèvre) est maintenant gérée par le monde
                monde_courrant = mis_à_jour_monde(monde_courrant, tick_animation, utilisateur->x_deplacement, utilisateur->y_deplacement);
            }
            afficher_monde(monde_courrant);
            free(utilisateur);
        }
        SDL_Delay(16);
    }
    quitter_affichage();
    free_goats(monde_courrant->goats_tab, monde_courrant->nb_goat);
    free_wolf(monde_courrant->wolfs_tab, monde_courrant->nb_wolf);
    free(monde_courrant->goats_tab);
    free(monde_courrant->fermiers);
    free(monde_courrant);
    return 0;
}