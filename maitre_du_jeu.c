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

/* Entrée : le monde actuel, normalement complètement vide
    Sortie : le monde avec le fermier et 10 chèvre qui apparaisse aléatoirement
    Synopsis : remplie le monde de 10 chèvre à des endroits aléatoire et le fermier devant sa maison */
monde * generer_un_monde(monde * monde_courant)
{
    Fermier * le_fermier = malloc(sizeof(Fermier));
    if(le_fermier)
    {
        le_fermier->frame = 0;
        le_fermier->direction_sprite = 2;
        le_fermier = init_fermier(le_fermier);
        monde_courant = ajouter_entitee(monde_courant, NULL, le_fermier);
    }
    else
    {
        free(le_fermier);
        return NULL;
    }
    for (int i = 0 ; i<10 ; i++)
    {
        Goat * une_goat = malloc(sizeof(Goat));
        if(une_goat)
        {
            une_goat->frame = 0;
            une_goat->direction_sprite = 2;
            une_goat = init_goat(une_goat);
            monde_courant = ajouter_entitee(monde_courant, une_goat, NULL);
        }
        else
        {
            free(une_goat);
            return NULL;
        }
    }
    return monde_courant;
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    // création et initialisation du monde
    monde * monde_courrant = creer_monde(LARGEUR, HAUTEUR);
    monde_courrant = generer_un_monde(monde_courrant);
    int quiiter_le_programme = 0; // variable de gestion de la boucle
    int en_pause = 1;
    while (quiiter_le_programme == 0)
    {
        if (!en_pause)
        {
            /* recuperation des  input utilisateur */
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

                free(utilisateur);
                SDL_Delay(100);
            }
        }
    return 0;
    }
}