#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>

#include "goat.h"
#include "utilisateur.h"
#include "affichage.h"
#include "fermier.h"
#include "monde.h"

#define LARGEUR 200
#define HAUTEUR 200

void free_goat(Goat ** goat)
{
    if(goat != NULL)
    {
        int i = 0;
        Goat * current_goat = goat[i];
        while(current_goat != NULL)
        {
            free(current_goat);
            i++;
            current_goat = goat[i];
        }
    }
}

/* ENtrées : le monde actuel dans sa structure et une chèvre
    Sotie : Le monde mis a jour avec la nouvelle chèvre en plus
    Synopsis : prend une chèvre et l'ajoute au monde*/
monde * ajouter_goat(monde * monde_courant, Goat * goat)
{
    if(monde_courant->nb_goat+1 > monde_courant->capacite_max_goat)
    {
        monde_courant->goats_tab = realloc(monde_courant->goats_tab, monde_courant->capacite_max_goat * 2 * sizeof(Goat));
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
        monde_courant->goats_tab = malloc(monde_courant->capacite_max_goat * sizeof(Goat*));
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
        monde_courant->fermiers = le_fermier;
    }
    else
    {
        free(le_fermier);
        return NULL;
    }
    for (int i = 0 ; i<1 ; i++)
    {
        Goat * une_goat = malloc(sizeof(Goat));
        if(une_goat)
        {
            une_goat->frame = 0;
            une_goat->direction_sprite = 2;
            une_goat = init_goat(une_goat);
            monde_courant = ajouter_goat(monde_courant, une_goat);
        }
        else
        {
            free(une_goat);
            return NULL;
        }
    }
    return monde_courant;
}

/* Entrée : le monde actuel
    Sortie : aucune
    Synopsis : prend le monde et pour chaque entité, demande à la SDL d'annimer 4 frame de l'entité pour qu'elle se déplacent sur l'écran*/
void afficher_monde(monde * monde_courant)
{
    int pas_goat = monde_courant->goats_tab[0]->speed/4;
    for(int j = 0; j<4; j++)
    {
        dessiner_monde();
        for(int w = 0; w<monde_courant->nb_goat; w++)
        {
            Goat * current_goat = monde_courant->goats_tab[w];
            dessiner_entite(1, current_goat->dir_x - pas_goat *4-j, current_goat->dir_y - pas_goat *4-j,current_goat->frame, current_goat->direction_sprite);
            if(current_goat->frame == 4) current_goat->frame = 0;
            else current_goat->frame++;
        }
        dessiner_entite(2, monde_courant->fermiers->x - pas_goat *4-j, monde_courant->fermiers->y-pas_goat *4-j, monde_courant->fermiers->frame, monde_courant->fermiers->direction_sprite);
        if(monde_courant->fermiers->frame == 9) monde_courant->fermiers->frame = 0;
        else monde_courant->fermiers->frame++;
        actualiser_ecran();
        SDL_Delay(200);
    }
}

/* Entrée : le monde actuel
    Sortie : le monde mis à jour
    Synopsis : Met à jour la position de chaque entité après déplacement dans notre jeu */
monde * mis_à_jour_monde(monde * monde_courant)
{
    for(int i = 0 ; i < monde_courant->nb_goat ; i++)
    {
        Goat * current_goat = monde_courant->goats_tab[i];
        current_goat = update_goat(current_goat, monde_courant->goats_tab, monde_courant->nb_goat);
        current_goat->x = current_goat->dir_x;
        current_goat->y = current_goat->dir_y;
    }
    monde_courant->fermiers = update_fermier(monde_courant->fermiers, monde_courant->fermiers->x, monde_courant->fermiers->y);
    return monde_courant;
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    srand(time(NULL));
    // création et initialisation du monde
    monde * monde_courrant = creer_monde(LARGEUR, HAUTEUR);
    monde_courrant = generer_un_monde(monde_courrant);
    int quiiter_le_programme = 0; // variable de gestion de la boucle
    int en_pause = 0;
    init_affichage();
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
                utilisateur->pause = 0;
                utilisateur->quitter = 0;
                utilisateur = recuperer_mouvement(utilisateur);
                quiiter_le_programme = utilisateur->quitter; // si quitter = 1 le programme prendra fin à a la fin de la boucle

                if (utilisateur->pause)
                {
                    en_pause = !en_pause;
                }
                // déplacement du fermier au cas où l'utilsateur avait appuyé sur une des flèche
                monde_courrant->fermiers = update_fermier(monde_courrant->fermiers, utilisateur->x_deplacement, utilisateur->y_deplacement);

                afficher_monde(monde_courrant);

                free(utilisateur);
                SDL_Delay(200);
            }
        }
    }
    quitter_affichage();
    free_goat(monde_courrant->goats_tab);
    free(monde_courrant->goats_tab);
    free(monde_courrant);
    return 0;
}