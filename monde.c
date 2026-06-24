#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "affichage.h"
#include "fermier.h"
#include "goat.h"
#include "loup.h"
#include "monde.h"
#include "reinforce.h"
#include "monde_goat.h"
#include "monde_wolf.h"

/**
 * Synopsis : calcul la proba de chaque action et calcul un nombre aleatoire et
 * en fonction de ce nombre et des proba renvoie une action. Entrée   : le
 * tableau d interet qui contient l interet de chaque action et le nombre d
 * actions possible. Sortie   : le numero de l'action retenue.
 */
int choisir_action_softmax(float *tab_interet, int nb_actions)
{
    float max_interet = tab_interet[0];
    for (int i = 1; i < nb_actions; i++)
    { // ici on l'action avec le meilleur interet
        if (tab_interet[i] > max_interet)
        {
            max_interet = tab_interet[i];
        }
    }

    // on calcule exp(interet de chaque action) et on met ca dans le tableau de probabilite
    float somme_exp = 0.0f;
    float probabilites[nb_actions];

    for (int i = 0; i < nb_actions; i++)
    {
        probabilites[i] = exp(tab_interet[i] - max_interet);
        somme_exp += probabilites[i]; // on calcul la somme des probabilites en meme temps
    }

    float tirage = (float)rand() / (float)RAND_MAX;
    float somme_cumulee = 0.0f;
    for (int i = 0; i < nb_actions; i++)
    {
        somme_cumulee += (probabilites[i] / somme_exp); // on normalise la proba pour que la proba soit comprise entre 0 et 1
        if (tirage <= somme_cumulee)
        {
            return i;
        }
    }
    return 0; // securite
}

/**
 * Synopsis : Vérifie la collision entre deux objets rectangulaires.
 * Entrée   : Coordonnées (x1, y1) du premier objet, coordonnées (x2, y2) du
 * deuxième objet. Sortie   : 1 si collision, 0 sinon.
 */
int check_collision_rect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    if (x1 + w1 < x2 || x2 + w2 < x1 || y1 + h1 < y2 || y2 + h2 < y1)
    {
        return 0; // Pas de collision
    }
    return 1; // Collision
}

/**
 * Synopsis : Crée et retourne la Hitbox à partir d'un rectangle de base.
 * Permet de réduire la taille de la zone sensible du sprite d'une entité en
 * appliquant des marges horizontales et verticales, afin d'ignorer les zones
 * transparentes de l'image. Entrée   : - x, y : Coordonnées du coin supérieur
 * gauche du sprite d'origine.
 * - w, h : Largeur et hauteur totales du sprite d'origine.
 * - marge_x : Nombre de pixels à retirer à gauche et à droite.
 * - marge_y : Nombre de pixels à retirer en haut et en bas.
 * Sortie   : Une structure Hitbox contenant les nouvelles coordonnées (x, y) et
 * dimensions (w, h) ajustées pour la logique de collision.
 */
Hitbox creer_hitbox(float x, float y, float w, float h, float marge_gauche, float marge_droite, float marge_haut, float marge_bas)
{
    Hitbox hb;
    hb.x = x + marge_gauche;
    hb.y = y + marge_haut;
    hb.w = w - (marge_gauche + marge_droite);
    hb.h = h - (marge_haut + marge_bas);

    // Sécurité : éviter d'avoir une largeur ou hauteur négative ou nulle
    if (hb.w <= 0.0f)
        hb.w = 1.0f;
    if (hb.h <= 0.0f)
        hb.h = 1.0f;

    return hb;
}

Hitbox get_hitbox_fermier(float x, float y)
{
    return creer_hitbox(x, y, WIDTH_FERMIER, HEIGHT_FERMIER, 10.0f, 1.0f, 10.0f, 10.0f);
}

Hitbox get_hitbox_goat(float x, float y)
{
    return creer_hitbox(x, y, WIDTH_GOAT, HEIGHT_GOAT, 0.5f, 28.0f, 10.0f, 10.0f);
}

Hitbox get_hitbox_wolf(float x, float y)
{
    return creer_hitbox(x, y, WIDTH_WOLF, HEIGHT_WOLF, 10.0f, 10.0f, 5.0f, 5.0f);
}

/**
 * Synopsis : Vérifie si une hitbox entre en collision avec les obstacles du terrain (le lac ou la maison).
 * Entrée   : La hitbox à tester.
 * Sortie   : 1 si collision avec un obstacle, 0 sinon.
 */
int check_collision_obstacles(Hitbox hb)
{
    // Obstacle : Le Lac
    Hitbox hb_lac = creer_hitbox(LAC_X, LAC_Y, LAC_WIDTH, LAC_HEIGHT, 0.0f, 0.0f, 0.0f, 0.0f);
    if (check_collision_rect(hb.x, hb.y, hb.w, hb.h, hb_lac.x, hb_lac.y, hb_lac.w, hb_lac.h))
    {
        return 1;
    }

    // Obstacle : La Maison
    Hitbox hb_maison = creer_hitbox(MAISON_X, MAISON_Y, MAISON_WIDTH, MAISON_HEIGHT, 0.0f, 0.0f, 0.0f, 0.0f);
    if (check_collision_rect(hb.x, hb.y, hb.w, hb.h, hb_maison.x, hb_maison.y, hb_maison.w, hb_maison.h))
    {
        return 1;
    }

    return 0;
}

/* ========= Monde ========= */

/* Entrée : deux entier la largeur et la hauteur du monde
   Sortie : le monde vide
   synopsis : créer un monde vide avec la structure monde*/
monde *creer_monde(int largeur, int hauteur)
{
    monde *monde_courant = malloc(sizeof(monde));
    if (monde_courant)
    {
        monde_courant->largeur = largeur;
        monde_courant->hauteur = hauteur;
        monde_courant->capacite_max_goat = 100;
        monde_courant->nb_goat = 0;
        monde_courant->capacite_max_wolf = 100;
        monde_courant->nb_wolf = 0;
        monde_courant->mode = 0;
        monde_courant->goats_tab = malloc(monde_courant->capacite_max_goat * sizeof(Goat *));
        monde_courant->wolfs_tab = malloc(monde_courant->capacite_max_wolf * sizeof(Wolf *));

        if (monde_courant->wolfs_tab)
            return monde_courant;
        else
        {
            free(monde_courant->wolfs_tab);
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
    Synopsis : remplie le monde de 10 chèvre à des endroits aléatoire et le
   fermier devant sa maison */
monde *generer_un_monde(monde *monde_courant)
{
    Fermier *le_fermier = malloc(sizeof(Fermier));
    if (le_fermier)
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
    int start_x = 1500;
    int start_y = 300;
    int écart_x = 120;
    int écart_y = 100;
    int nbre_collonne = 2;
    for (int i = 0; i < 10; i++)
    {
        Goat *une_goat = malloc(sizeof(Goat));
        if (une_goat)
        {
            une_goat->frame = 0;
            une_goat->direction_sprite = 2;
            
            // Calcul du placement en rectangle (grille de 2 colonnes par 5 lignes)
            int colonne = i % nbre_collonne;
            int ligne = i / nbre_collonne;
            int spawn_x = start_x + colonne * écart_x;
            int spawn_y = start_y + ligne * écart_y;
            
            une_goat = init_goat(une_goat, spawn_x, spawn_y);
            monde_courant = ajouter_goat(monde_courant, une_goat);
        }
        else
        {
            free(une_goat);
            return NULL;
        }
    }
    for (int i = 0; i < 3; i++)
    {
        Wolf *un_wolf = malloc(sizeof(Wolf));
        if (un_wolf)
        {
            un_wolf->frame = 0;
            un_wolf->direction_sprite = 2;
            un_wolf = init_wolf(un_wolf);
            monde_courant = ajouter_wolf(monde_courant, un_wolf);
        }
        else
        {
            free(un_wolf);
            return NULL;
        }
    }
    return monde_courant;
}

/* Entrée : le monde actuel
    Sortie : le monde mis à jour
    Synopsis : Met à jour la position de chaque entité après déplacement dans
   notre jeu */
monde *mis_à_jour_monde(monde *monde_courant, int tick_animation, int input_x, int input_y)
{
    // economie :
    if (tick_animation % 60 == 0)
    {
        monde_courant->fermiers->or += monde_courant->nb_goat * 2; // +2 pièces par chèvre vivante par seconde
    }

    // Fermier
    if(monde_courant->mode)
    {
        Fermier *fermier = monde_courant->fermiers;
        fermier->decision_cooldown--;
        // Prise de décision (seulement à l'expiration du cooldown)
        if (fermier->decision_cooldown <= 0)
        {
            PerceptionFermier perception_fermier = calculer_perception_fermier(fermier, monde_courant);
            evaluer_interets_fermier(fermier, perception_fermier);
            fermier->action_choisi = choisir_action_softmax(fermier->table_interets, NB_ACTIONS);
            fermier->decision_cooldown = 30 + (rand() % 45); // Entre 0.5s et 1.25s à 60 FPS
        }
        update_fermier(monde_courant, fermier, fermier->dirrection_choisi, tick_animation, calculer_perception_fermier(fermier, monde_courant));
    }

    // Chèvres
    for (int i = 0; i < monde_courant->nb_goat; i++)
    {
        Goat *goat = monde_courant->goats_tab[i];
        goat->decision_cooldown--;
        if (goat->cooldown_dinvisibilite > 0)
            goat->cooldown_dinvisibilite--; // on descend l'invisibilité si la chèvre est en cooldown, sinon on ne fait rien
        else
            goat->cooldown_dinvisibilite = 0;

        // Prise de décision (seulement à l'expiration du cooldown)
        if (goat->decision_cooldown <= 0)
        {
            PerceptionGoat perception_goat = calculer_perception_goat(goat, monde_courant);
            evaluer_interets_goat(goat, perception_goat);
            goat->action_choisi = choisir_action_softmax(goat->table_interets, NB_ACTIONS);
            if (goat->action_choisi == ACTION_ERRER)
                goat->angle_actuel = ((float)rand() / (float)RAND_MAX) * 2.0 * 3.14;
            goat->decision_cooldown = 30 + (rand() % 45); // Entre 0.5s et 1.25s à 60 FPS
        }
        update_goat(monde_courant, goat, goat->action_choisi, tick_animation, calculer_perception_goat(goat, monde_courant));
    }

    // Loups
    for (int i = 0; i < monde_courant->nb_wolf; i++)
    {
        Wolf *wolf = monde_courant->wolfs_tab[i];
        wolf->decision_cooldown--;
        if (wolf->cooldown_dinvisibilite > 0)
            wolf->cooldown_dinvisibilite--; // on descend l'invisibilité si le loup est en cooldown, sinon on ne fait rien
        else
            wolf->cooldown_dinvisibilite = 0;

        if (wolf->decision_cooldown <= 0)
        {
            PerceptionWolf perception_wolf = calculer_perception_wolf(wolf, monde_courant);
            if (monde_courant->mode)
            {
                evaluer_interets_wolf_rl(wolf, perception_wolf);
            }
            else
            {
                evaluer_interets_wolf(wolf, perception_wolf);
            }
            wolf->action_choisi =
                choisir_action_softmax(wolf->table_interets, NB_ACTIONS_WOLF);
            if (wolf->action_choisi == ACTION_WOLF_ERRER)
                wolf->angle_actuel = ((float)rand() / (float)RAND_MAX) * 2.0 * 3.14;
            wolf->decision_cooldown = 30 + (rand() % 45); // Entre 0.5s et 1.25s à 60 FPS
        }
        update_wolf(monde_courant, wolf, wolf->action_choisi, tick_animation, calculer_perception_wolf(wolf, monde_courant));
    }

    // Action du Fermier
    monde_courant = mis_a_jour_fermier(monde_courant, tick_animation, input_x, input_y);

    // attaque loup chèvre
    for (int i = 0; i < monde_courant->nb_wolf; i++)
    {
        Wolf *wolf = monde_courant->wolfs_tab[i];
        Hitbox hb_wolf = get_hitbox_wolf(wolf->x, wolf->y);

        for (int j = 0; j < monde_courant->nb_goat; j++)
        {
            if (monde_courant->goats_tab[j]->cooldown_dinvisibilite == 0)
            {
                Goat *goat = monde_courant->goats_tab[j];
                Hitbox hb_goat = get_hitbox_goat(goat->x, goat->y);

                if (check_collision_rect(hb_wolf.x, hb_wolf.y, hb_wolf.w, hb_wolf.h, hb_goat.x, hb_goat.y, hb_goat.w, hb_goat.h))
                {
                    goat->hp--;
                    if (goat->hp <= 0)
                    {
                        mourrir_goat(monde_courant, j);
                        j--;
                    }
                    else
                        monde_courant->goats_tab[j]->cooldown_dinvisibilite = 180; // 3 secondes d'invisibilité après avoir été attaquée
                }
            }
        }
    }
    // attaque fermier loup
    for (int i = 0; i < monde_courant->nb_wolf; i++)
    {
        if (monde_courant->wolfs_tab[i]->cooldown_dinvisibilite == 0)
        {
            Wolf *wolf = monde_courant->wolfs_tab[i];
            Hitbox hb_wolf = get_hitbox_wolf(wolf->x, wolf->y);
            Hitbox hb_fermier = get_hitbox_fermier(monde_courant->fermiers->x, monde_courant->fermiers->y);

            if (check_collision_rect(hb_fermier.x, hb_fermier.y, hb_fermier.w, hb_fermier.h, hb_wolf.x, hb_wolf.y, hb_wolf.w, hb_wolf.h))
            {
                // Le fermier élimine le loup
                wolf->hp--;
                if (wolf->hp <= 0)
                {
                    mourrir_wolf(monde_courant, i);
                    i--;
                }
                else
                    monde_courant->wolfs_tab[i]->cooldown_dinvisibilite = 180; // 3 secondes d'invisibilité après avoir été attaqué
            }
        }
    }

    return monde_courant;
}

/* ========= Affichage ========= */

/**
 * Synopsis : Affiche le monde courant, incluant le terrain, le fermier, les
 * chèvres et les loups. Entrée   : Pointeur vers le monde à afficher. Sortie :
 * Aucune.
 */
void afficher_monde(monde *monde_courant)
{
    if (!monde_courant)
        return;
    dessiner_monde();

    for (int i = 0; i < monde_courant->nb_goat; i++)
    {
        Goat *g = monde_courant->goats_tab[i];
        if (g)
            dessiner_entite(1, g->x, g->y, g->frame, g->direction_sprite);
    }
    for (int i = 0; i < monde_courant->nb_wolf; i++)
    {
        Wolf *w = monde_courant->wolfs_tab[i];
        if (w)
            dessiner_entite(4, w->x, w->y, w->frame, w->direction_sprite);
    }
    if (monde_courant->fermiers)
    {
        dessiner_entite(2, monde_courant->fermiers->x, monde_courant->fermiers->y, monde_courant->fermiers->frame, monde_courant->fermiers->direction_sprite);
    }
    afficher_planche(monde_courant->nb_goat, monde_courant->nb_wolf);
    actualiser_ecran();
}
