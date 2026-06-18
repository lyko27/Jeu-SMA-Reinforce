#include <stdio.h>
#include <stdlib.h>
#include "goat.h"
#include "utilisateur.h"
#include "affichage.h"
#include "fermier.h"

#define LARGEUR 200
#define HAUTEUR 200


int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    /* Création des tableaux_click de cases de taille TAILLE par TAILLE, soit 10px par case et 20 case par ligne */
    int *monde_courrant = calloc(HAUTEUR * LARGEUR, sizeof(int));

    int quiiter_le_programme = 0; // variable de gestion de la boucle
    int en_pause = 1;
    while (quiiter_le_programme == 0)
    {
        int *monde_suivant = calloc(HAUTEUR * LARGEUR, sizeof(int));

        if (!en_pause)
        {
        evenement_clique_pause *resultat = malloc(sizeof(evenement_clique_pause));
        if (resultat)
        {
            resultat->zoom = 0;
            resultat->quitter = 0;
            resultat->pause = 0;
            resultat->deplacement_x = 0;
            resultat->deplacement_y = 0;
            resultat->x_zoom = -1;
            resultat->y_zoom = -1;
            resultat = recuperer_entree_clique(resultat);
            quiiter_le_programme = resultat->quitter; // si quitter = 1 le programme prendra fin à a la fin de la boucle


            if(resultat->zoom !=0)
            {
                float old_zoom = zoom;
                zoom += resultat->zoom * 0.1f; // on zoom sur la fenêtre
                if (zoom <= 0.2f)
                    zoom = 0.2f; // on empêche le zoom de devenir nul ou négatif

                if (zoom != old_zoom && resultat->x_zoom >= 0 && resultat->y_zoom >= 0)
                {
                    float ratio = zoom / old_zoom;
                    offset_x = (resultat->x_zoom - 400.0f) - (resultat->x_zoom - 400.0f - offset_x) * ratio;
                    offset_y = (resultat->y_zoom - 400.0f) - (resultat->y_zoom - 400.0f - offset_y) * ratio;
                }
            }

            offset_x += resultat->deplacement_x * VITESSE_DE_MOUVEMENt;
            offset_y += resultat->deplacement_y * VITESSE_DE_MOUVEMENt;

            if (resultat->pause)
            {
                en_pause = !en_pause;
            }

            if (resultat->x_click != -1 && resultat->y_click != -1)
            {
                float TAILLE_CELLULE_ZOOM = 10.0f * zoom;
                float position_x = (800 - (TAILLE * TAILLE_CELLULE_ZOOM)) / 2.0f + offset_x;
                float position_y = (800 - (TAILLE * TAILLE_CELLULE_ZOOM)) / 2.0f + offset_y;

                if (resultat->x_click >= position_x && resultat->y_click >= position_y)
                {
                    int grid_x = (resultat->x_click - position_x) / TAILLE_CELLULE_ZOOM;
                    int grid_y = (resultat->y_click - position_y) / TAILLE_CELLULE_ZOOM;

                    if (grid_x >= 0 && grid_x < TAILLE && grid_y >= 0 && grid_y < TAILLE)
                    {
                        // On inverse l'état de la case au clic (vivante <-> morte)
                        monde_suivant[grid_x + TAILLE * grid_y] = !monde_suivant[grid_x + TAILLE * grid_y];
                    }
                }
            }
            /*intervertion des monde*/
            int *tmp = monde_courrant;
            monde_courrant = monde_suivant;
            free(tmp);

            dessiner_monde(monde_courrant, offset_x, offset_y, zoom);
            free(resultat);
            SDL_Delay(100);
        }
    }
    end_sdl(1, "Fin normale du programme");
    return 0;
}