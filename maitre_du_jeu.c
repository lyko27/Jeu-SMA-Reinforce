#include <stdio.h>
#include <stdlib.h>
#include "goat.h"
#include "utilisateur.h"
#include "affichage.h"
#include "fermier.h"
#include "monde.h"

#define LARGEUR 200
#define HAUTEUR 200


/* Entrée : deux entier la largeur et la hauteur du monde
   Sortie : le monde vide
   synopsis : créer un monde vide avec la structure monde*/
monde * creer_monde(int largeur, int hauteur)
{
    monde * monde_courant = malloc(sizeof(monde));
    if(monde_courant)
    {
        monde_courant->largeur = largeur;
        monde_courant->hauteur = hauteur;
        monde_courant->capacite_max = 100;
        monde_courant->nb_entites = 0;
        monde_courant->entites = malloc(monde_courant->capacite_max * sizeof(entitee_t));
        if(monde_courant->entites) return monde_courant;
        else
        {
            free(monde_courant->entites);
            free(monde_courant);
            return NULL
        }
    }
    else 
    {
        free(monde_courant);
        return NULL;
    }
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int quiiter_le_programme = 0; // variable de gestion de la boucle
    int en_pause = 1;
    while (quiiter_le_programme == 0)
    {
        

        if (!en_pause)
        {
            interaction_utilisateur * utilisateur = malloc(sizeof(interaction_utilisateur));
            if (utilisateur)
            {
                utilisateur->deplacement_x = 0;
                utilisateur->deplacement_y = 0;
                utilisateur = recuperer_mouvement(utilisateur);
                quiiter_le_programme = utilisateur->quitter; // si quitter = 1 le programme prendra fin à a la fin de la boucle

                if (utilisateur->pause)
                {
                    en_pause = !en_pause;
                }
                dessiner_monde(monde_courrant, offset_x, offset_y, zoom);
                free(utilisateur);
                SDL_Delay(100);
            }
        }
    return 0;
    }
}