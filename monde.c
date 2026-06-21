#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "goat.h"
#include "loup.h"
#include "fermier.h"
#include "monde.h"
#include "affichage.h"

#define LARGEUR 1024
#define HAUTEUR 1024

/**
 * Synopsis : Vérifie la collision entre deux objets rectangulaires.
 * Entrée   : Coordonnées (x1, y1) du premier objet, coordonnées (x2, y2) du deuxième objet.
 * Sortie   : 1 si collision, 0 sinon.
 */
int check_collision_rect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    if (x1 + w1 < x2 || x2 + w2 < x1 || y1 + h1 < y2 || y2 + h2 < y1)
    {
        return 0; // Pas de collision
    }
    return 1; // Collision
}

/* ENtrées : le tableau de chèvre, nombre de chèvre
    Sotie : aucune
    Synopsis : libère toute les goats du tableau*/
void free_goats(Goat **goats_tab, int nb_goat)
{
    if (goats_tab != NULL)
    {
        for (int i = 0; i < nb_goat; i++)
        {
            if (goats_tab[i] != NULL)
            {
                free(goats_tab[i]);
                goats_tab[i] = NULL;
            }
        }
    }
}

/* ENtrées : le tableau de loup, nombre de loup
    Sotie : aucune
    Synopsis : libère toute les wolfs du tableau*/
void free_wolf(Wolf **wolf_tab, int nombre_wolf)
{
    if (wolf_tab != NULL)
    {
        for (int i = 0; i < nombre_wolf; i++)
        {
            if (wolf_tab[i] != NULL)
            {
                free(wolf_tab[i]);
                wolf_tab[i] = NULL;
            }
        }
    }
}

/* ENtrées : le monde actuel dans sa structure et une chèvre
    Sotie : Le monde mis a jour avec la nouvelle chèvre en plus
    Synopsis : prend une chèvre et l'ajoute au monde*/
monde *ajouter_goat(monde *monde_courant, Goat *goat)
{
    if (monde_courant->nb_goat + 1 > monde_courant->capacite_max_goat)
    {
        monde_courant->goats_tab = realloc(monde_courant->goats_tab, monde_courant->capacite_max_goat * 2 * sizeof(Goat));
        monde_courant->capacite_max_goat *= 2;
    }
    monde_courant->goats_tab[monde_courant->nb_goat] = goat;
    monde_courant->nb_goat++;
    return monde_courant;
}

/* ENtrées : le monde actuel dans sa structure et un loup
    Sotie : Le monde mis a jour avec le nouveau loup en plus
    Synopsis : prend un loup et l'ajoute au monde*/
monde *ajouter_wolf(monde *monde_courant, Wolf *wolf)
{
    if (monde_courant->nb_wolf + 1 > monde_courant->capacite_max_wolf)
    {
        monde_courant->wolfs_tab = realloc(monde_courant->wolfs_tab, monde_courant->capacite_max_wolf * 2 * sizeof(Wolf));
        monde_courant->capacite_max_wolf *= 2;
    }
    monde_courant->wolfs_tab[monde_courant->nb_wolf] = wolf;
    monde_courant->nb_wolf++;
    return monde_courant;
}

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

/**
 * Synopsis : Initialise une chèvre avec des coordonnées aléatoires et une direction par défaut.
 * Entrée   : Pointeur vers la structure Goat à initialiser.
 * Sortie   : Pointeur vers la structure Goat initialisée.
 */
Goat *init_goat(Goat *g)
{
    g->x = rand() % (LARGEUR - WIDTH_GOAT);
    g->y = rand() % (HAUTEUR - HEIGHT_GOAT);

    g->dir_x = g->x;
    g->dir_y = g->y;

    g->speed = 0;
    g->direction_sprite = 1 + (rand() % 4);
    g->frame = 0;
    g->timer_mouvement = 10;
    g->en_mouvement = 0;
    g->angle_actuel = 0.0f;

    // Initialisation de la table des intérêts
    for (int i = 0; i < NB_ACTIONS; i++) {
        g->table_interets[i] = 0.0f;
    }

    return g;
}

Wolf *init_wolf(Wolf *l)
{
    l->x = rand() % (LARGEUR - WIDTH_WOLF);
    l->y = rand() % (HAUTEUR - HEIGHT_WOLF);

    l->dir_x = l->x;
    l->dir_y = l->y;

    l->speed = 0;
    l->direction_sprite = 1 + (rand() % 4);
    l->frame = 0;
    l->timer_mouvement = 10;
    l->en_mouvement = 0;
    l->angle_actuel = 0.0f;

    for (int i = 0; i < NB_ACTIONS_LOUP; i++) {
        l->table_interets[i] = 0.0f;
    }

    return l;
}

Fermier *init_fermier(Fermier *fermier) {
    fermier->x = 347;
    fermier->y = 185;
    fermier->speed = VITESSE_FERMIER;
    return fermier;
}

/* Entrée : le monde actuel, normalement complètement vide
    Sortie : le monde avec le fermier et 10 chèvre qui apparaisse aléatoirement
    Synopsis : remplie le monde de 10 chèvre à des endroits aléatoire et le fermier devant sa maison */
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
    for (int i = 0; i < 10; i++)
    {
        Goat *une_goat = malloc(sizeof(Goat));
        if (une_goat)
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
    Synopsis : Met à jour la position de chaque entité après déplacement dans notre jeu */
monde *mis_à_jour_monde(monde *monde_courant, int tick_animation, int input_x, int input_y)
{
    // ==========================================
    // PHASE 1 : Perception et Décision
    // ==========================================
    ActionGoat actions_chevres[monde_courant->nb_goat];
    ActionLoup actions_loups[monde_courant->nb_wolf];
    ActionFermier action_fermier;
    
    // Fermier
    PerceptionFermier pf;
    pf.input_x = input_x;
    pf.input_y = input_y;
    action_fermier = decider_action_fermier(monde_courant->fermiers, pf);

    // Chèvres
    for (int i = 0; i < monde_courant->nb_goat; i++) {
        Goat *current_goat = monde_courant->goats_tab[i];
        PerceptionGoat p;
        p.dist_loup_proche = 200.0f; 
        p.goats_tab = monde_courant->goats_tab;
        p.nb_goat = monde_courant->nb_goat;
        actions_chevres[i] = decider_action(current_goat, p);
    }

    // Loups
    for (int i = 0; i < monde_courant->nb_wolf; i++) {
        Wolf *current_wolf = monde_courant->wolfs_tab[i];
        PerceptionLoup pw;
        pw.goats_tab = monde_courant->goats_tab;
        pw.nb_goat = monde_courant->nb_goat;
        pw.pos_x_fermier = monde_courant->fermiers->x;
        pw.pos_y_fermier = monde_courant->fermiers->y;
        actions_loups[i] = decider_action_wolf(current_wolf, pw);
    }

    // ==========================================
    // PHASE 2 & 3 : Exécution et Résolution
    // ==========================================

    // ----- FERMIER -----
    Fermier *f = monde_courant->fermiers;
    float next_f_x = f->x;
    float next_f_y = f->y;
    if (action_fermier == ACTION_FERMIER_BOUGER_HAUT) {
        next_f_y -= f->speed;
        f->direction_sprite = 1;
    } else if (action_fermier == ACTION_FERMIER_BOUGER_BAS) {
        next_f_y += f->speed;
        f->direction_sprite = 3;
    } else if (action_fermier == ACTION_FERMIER_BOUGER_GAUCHE) {
        next_f_x -= f->speed;
        f->direction_sprite = 4;
    } else if (action_fermier == ACTION_FERMIER_BOUGER_DROITE) {
        next_f_x += f->speed;
        f->direction_sprite = 2;
    }

    if (action_fermier == ACTION_FERMIER_IMMOBILE) {
        f->frame = 5;
        f->direction_sprite = 0;
    } else {
        if (next_f_x < 0) next_f_x = 0;
        if (next_f_x > LARGEUR - WIDTH_FERMIER) next_f_x = LARGEUR - WIDTH_FERMIER;
        if (next_f_y < 0) next_f_y = 0;
        if (next_f_y > HAUTEUR - HEIGHT_FERMIER) next_f_y = HAUTEUR - HEIGHT_FERMIER;

        int collision_f = 0;
        for (int j = 0; j < monde_courant->nb_goat; j++) {
            if (check_collision_rect(next_f_x, next_f_y, WIDTH_FERMIER, HEIGHT_FERMIER, 
                                     monde_courant->goats_tab[j]->x, monde_courant->goats_tab[j]->y, 
                                     WIDTH_GOAT, HEIGHT_GOAT)) {
                collision_f = 1;
                break;
            }
        }
        
        if (!collision_f) {
            f->x = next_f_x;
            f->y = next_f_y;
        }

        if (tick_animation % 6 == 0) {
            f->frame = (f->frame + 1) % 9;
        }
    }

    // ----- CHEVRES -----
    for (int i = 0; i < monde_courant->nb_goat; i++) {
        Goat *current_goat = monde_courant->goats_tab[i];
        ActionGoat action = actions_chevres[i];
        
        float next_x = current_goat->x;
        float next_y = current_goat->y;
        
        if (action == ACTION_ERRER || action == ACTION_FUIR_LOUP) {
            current_goat->speed = 2;
            next_x += cos(current_goat->angle_actuel) * current_goat->speed;
            next_y += sin(current_goat->angle_actuel) * current_goat->speed;
            
            if (fabs(next_x - current_goat->x) > fabs(next_y - current_goat->y)) {
                current_goat->direction_sprite = (next_x > current_goat->x) ? 2 : 4;
            } else {
                current_goat->direction_sprite = (next_y > current_goat->y) ? 3 : 1;
            }
        } 
        else if (action == ACTION_BROUTER) {
            current_goat->speed = 0;
        }
        
        if (next_x < 0) next_x = 0;
        if (next_y < 0) next_y = 0;
        if (next_x > LARGEUR - WIDTH_GOAT) next_x = LARGEUR - WIDTH_GOAT;
        if (next_y > HAUTEUR - HEIGHT_GOAT) next_y = HAUTEUR - HEIGHT_GOAT;

        int collision = 0;
        for (int j = 0; j < monde_courant->nb_goat; j++) {
            if (i != j) {
                if (check_collision_rect(next_x, next_y, WIDTH_GOAT, HEIGHT_GOAT, 
                                         monde_courant->goats_tab[j]->x, monde_courant->goats_tab[j]->y, 
                                         WIDTH_GOAT, HEIGHT_GOAT)) {
                    collision = 1;
                    current_goat->timer_mouvement = 0;
                    break;
                }
            }
        }
        
        if (!collision) {
            current_goat->x = next_x;
            current_goat->y = next_y;
            current_goat->dir_x = next_x;
            current_goat->dir_y = next_y;
        } else {
            current_goat->dir_x = current_goat->x;
            current_goat->dir_y = current_goat->y;
        }
        
        if (tick_animation % 6 == 0) {
            if (current_goat->speed > 0 && !collision) {
                current_goat->frame = (current_goat->frame + 1) % 4;
            } else {
                current_goat->frame = 0;
            }
        }
    }

    // ----- LOUPS -----
    for (int i = 0; i < monde_courant->nb_wolf; i++) {
        Wolf *current_wolf = monde_courant->wolfs_tab[i];
        ActionLoup action = actions_loups[i];
        
        float next_x = current_wolf->x;
        float next_y = current_wolf->y;
        
        if (action == ACTION_LOUP_ERRER || action == ACTION_LOUP_CHASSER) {
            current_wolf->speed = 2;
            next_x += cos(current_wolf->angle_actuel) * current_wolf->speed;
            next_y += sin(current_wolf->angle_actuel) * current_wolf->speed;
            
            if (fabs(next_x - current_wolf->x) > fabs(next_y - current_wolf->y)) {
                current_wolf->direction_sprite = (next_x > current_wolf->x) ? 2 : 4;
            } else {
                current_wolf->direction_sprite = (next_y > current_wolf->y) ? 3 : 1;
            }
        } 
        else if (action == ACTION_LOUP_ARRET) {
            current_wolf->speed = 0;
        }

        if (next_x < 0) next_x = 0;
        if (next_y < 0) next_y = 0;
        if (next_x > LARGEUR - WIDTH_WOLF) next_x = LARGEUR - WIDTH_WOLF;
        if (next_y > HAUTEUR - HEIGHT_WOLF) next_y = HAUTEUR - HEIGHT_WOLF;

        int collision = 0;
        for (int j = 0; j < monde_courant->nb_wolf; j++) {
            if (i != j) {
                if (check_collision_rect(next_x, next_y, WIDTH_WOLF, HEIGHT_WOLF, 
                                         monde_courant->wolfs_tab[j]->x, monde_courant->wolfs_tab[j]->y, 
                                         WIDTH_WOLF, HEIGHT_WOLF)) {
                    collision = 1;
                    current_wolf->timer_mouvement = 0;
                    break;
                }
            }
        }
        
        if (!collision) {
            current_wolf->x = next_x;
            current_wolf->y = next_y;
            current_wolf->dir_x = next_x;
            current_wolf->dir_y = next_y;
        } else {
            current_wolf->dir_x = current_wolf->x;
            current_wolf->dir_y = current_wolf->y;
        }
        
        if (tick_animation % 6 == 0) {
            if (current_wolf->speed > 0 && !collision) {
                current_wolf->frame = (current_wolf->frame + 1) % 4;
            } else {
                current_wolf->frame = 0;
            }
        }
    }

    return monde_courant;
}

void afficher_monde(monde *monde_courant) {
    if (!monde_courant) return;
    dessiner_monde();
    if (monde_courant->fermiers) {
        dessiner_entite(2, monde_courant->fermiers->x, monde_courant->fermiers->y, monde_courant->fermiers->frame, monde_courant->fermiers->direction_sprite);
    }
    for (int i = 0; i < monde_courant->nb_goat; i++) {
        Goat *g = monde_courant->goats_tab[i];
        if (g) dessiner_entite(1, g->x, g->y, g->frame, g->direction_sprite);
    }
    for (int i = 0; i < monde_courant->nb_wolf; i++) {
        Wolf *w = monde_courant->wolfs_tab[i];
        if (w) dessiner_entite(4, w->x, w->y, w->frame, w->direction_sprite);
    }
    actualiser_ecran();
}
