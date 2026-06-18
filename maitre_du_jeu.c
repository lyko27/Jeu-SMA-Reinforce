#include <stdio.h>
#include <stdlib.h>
#include "goat.h"
#include "utilisateur.h"
#include "affichage.h"
#include "fermier.h"
#include "monde.h"

#define LARGEUR 200
#define HAUTEUR 200

/* ENtrées : le monde actuel dans sa structure
    Sotie : Le monde mis a jour avec la nouvelle entitée en plus
    Synopsis : prend une entitée et l'ajoute au monde*/
monde * ajouter_entitee(monde * monde_courant, Goat * goat, Fermier * fermier)
{
    if(monde_courant->nb_goat+1 > monde_courant->capacite_max_goat)
    {
        realloc(monde_courant->goats_tab, monde_courant->capacite_max_goat * 2 * sizeof(Goat));
        monde_courant->capacite_max_goat *= 2;
    }
    monde_courant->goats_tab[monde_courant->nb_goat] = goat;
    monde_courant->nb_goat++;
    return monde_courant;
}

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
        monde_courant->capacite_max_goat = 100;
        monde_courant->nb_goat = 0;
        monde_courant->goats_tab = malloc(monde_courant->capacite_max_goat * sizeof(Goat));
        if(monde_courant->goats_tab) return monde_courant;
        else
        {
            free(monde_courant->goats_tab);
            free(monde_courant);
            return NULL;
        }
    }
    else 
    {
        free(monde_courant);
        return NULL;
    }
}

monde * generer_un_monde(monde * monde_courant)
{


}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    // création et initialisation du monde
    monde * monde_courrant = creer_monde(LARGEUR, HAUTEUR);

    int quiiter_le_programme = 0; // variable de gestion de la boucle
    int en_pause = 1;
    while (quiiter_le_programme == 0)
    {
        

        if (!en_pause)
        {

            interaction_utilisateur * utilisateur = malloc(sizeof(interaction_utilisateur));
            if (utilisateur)
            {
                utilisateur->x_deplacement = 0;
                utilisateur->y_deplacement = 0;
                utilisateur = recuperer_mouvement(utilisateur);
                quiiter_le_programme = utilisateur->quitter; // si quitter = 1 le programme prendra fin à a la fin de la boucle

                if (utilisateur->pause)
                {
                    en_pause = !en_pause;
                }
                //dessiner_monde(monde_courrant, offset_x, offset_y, zoom);
                free(utilisateur);
                SDL_Delay(100);
            }
        }
    return 0;
    }
}