#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "affichage.h"
#include "fermier.h"
#include "goat.h"
#include "loup.h"
#include "monde.h"

#define LARGEUR 1024
#define HAUTEUR 1024
#define MARGE 90

// Zone du lac (obstacle)
#define LAC_X 713
#define LAC_Y 515
#define LAC_WIDTH 120
#define LAC_HEIGHT 110

/**
 * Synopsis : calcul la proba de chaque action et calcul un nombre aleatoire et
 * en fonction de ce nombre et des proba renvoie une action. Entrée   : le
 * tableau d interet qui contient l interet de chaque action et le nombre d
 * actions possible. Sortie   : le numero de l'action retenue.
 */
int choisir_action_softmax(float *tab_interet, int nb_actions)
{
    float max_interet = tab_interet[0];
    for (int i = 1; i < nb_actions;
         i++)
    { // ici on l'action avec le meilleur interet
        if (tab_interet[i] > max_interet)
        {
            max_interet = tab_interet[i];
        }
    }

    // on calcule exp(interet de chaque action) et on met ca dans le tableau de
    // probabilite
    float somme_exp = 0.0f;
    float probabilites[nb_actions];

    for (int i = 0; i < nb_actions; i++)
    {
        probabilites[i] = exp(tab_interet[i] - max_interet);
        somme_exp +=
            probabilites[i]; // on calcul la somme des probabilites en meme temps
    }

    float tirage = (float)rand() /
                   (float)RAND_MAX; // le tirage simule le de dont on a parle pour
                                    // le cas d un environnement stochastique
    float somme_cumulee = 0.0f;
    for (int i = 0; i < nb_actions; i++)
    {
        somme_cumulee +=
            (probabilites[i] / somme_exp); // on normalise la proba pour que proba
                                           // soit comprise entre 0 et 1
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
int check_collision_rect(float x1, float y1, float w1, float h1, float x2,
                         float y2, float w2, float h2)
{
    if (x1 + w1 < x2 || x2 + w2 < x1 || y1 + h1 < y2 || y2 + h2 < y1)
    {
        return 0; // Pas de collision
    }
    return 1; // Collision
}

/**
 * Synopsis : Crée et retourne la Hitbox à partir d'un rectangle de base.
 * Permet de réduire la taille de la zone sensible du sprite d'une entité en appliquant
 * des marges horizontales et verticales, afin d'ignorer les zones transparentes de l'image.
 * Entrée   : - x, y : Coordonnées du coin supérieur gauche du sprite d'origine.
 * - w, h : Largeur et hauteur totales du sprite d'origine.
 * - marge_x : Nombre de pixels à retirer à gauche et à droite.
 * - marge_y : Nombre de pixels à retirer en haut et en bas.
 * Sortie   : Une structure Hitbox contenant les nouvelles coordonnées (x, y) et dimensions (w, h) 
 * ajustées pour la logique de collision.
 */
Hitbox creer_hitbox(float x, float y, float w, float h, float marge_x, float marge_y) {
    Hitbox hb;
    hb.x = x + marge_x;
    hb.y = y + marge_y;
    hb.w = w - (2 * marge_x);
    hb.h = h - (2 * marge_y);
    return hb;
}
/* ========== Fermier ========== */

/**
 * Synopsis : Initialise le fermier devant sa maison.
 * Entrée   : Pointeur vers la structure Fermier à initialiser.
 * Sortie   : Pointeur vers la structure Fermier initialisée.
 */
Fermier *init_fermier(Fermier *fermier)
{
    fermier->x = 347;
    fermier->y = 185;
    fermier->speed = VITESSE_FERMIER;
    return fermier;
}

/* ========== Chèvres ========== */

/**
 * Synopsis : Initialise une chèvre avec des coordonnées aléatoires et une direction par défaut.
 * Entrée   : Pointeur vers la structure Goat à initialiser.
 * Sortie   : Pointeur vers la structure Goat initialisée.
 */
Goat *init_goat(Goat *goat)
{
    goat->x = rand() % (LARGEUR - WIDTH_GOAT);
    goat->y = rand() % (HAUTEUR - HEIGHT_GOAT);

    goat->dir_x = goat->x;
    goat->dir_y = goat->y;

    goat->speed = 0;
    goat->direction_sprite = 1 + (rand() % 4);
    goat->frame = 0;
    goat->timer_mouvement = 10;
    goat->en_mouvement = 0;
    goat->angle_actuel = 0.0f;

    // Initialisation de la table des intérêts
    for (int i = 0; i < NB_ACTIONS; i++)
    {
        goat->table_interets[i] = 0.0f;
    }

    return goat;
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

PerceptionGoat calculer_perception_goat(Goat *goat, monde *monde_courant)
{
    PerceptionGoat perception;
    perception.dist_wolf_proche = 200.0f; // Valeur par défaut
    perception.goats_tab = monde_courant->goats_tab;
    perception.nb_goat = monde_courant->nb_goat;
    perception.pos_x_fermier = monde_courant->fermiers->x;
    perception.pos_y_fermier = monde_courant->fermiers->y;

    for (int i = 0; i < monde_courant->nb_wolf; i++)
    {
        Wolf *current_wolf = monde_courant->wolfs_tab[i];
        float dist_x = current_wolf->x - goat->x;
        float dist_y = current_wolf->y - goat->y;
        float distance = sqrt(dist_x * dist_x + dist_y * dist_y);

        if (distance < perception.dist_wolf_proche)
        {
            perception.dist_wolf_proche = distance;
            perception.pos_x_wolf = current_wolf->x;
            perception.pos_y_wolf = current_wolf->y;
        }
    }
    return perception;
}

Goat *update_goat(monde *monde_courant, Goat *goat, ActionGoat action, int tick_animation, PerceptionGoat perception_goat)
{
    float next_x = goat->x;
    float next_y = goat->y;

    if (action == ACTION_ERRER || action == ACTION_FUIR_WOLF)
    {
        goat->speed = 2;

        if (action == ACTION_FUIR_WOLF)
        {
            if (perception_goat.pos_x_wolf != -1)
            {
                // Fuir dans la direction opposée
                goat->angle_actuel = atan2(goat->y - perception_goat.pos_y_wolf, goat->x - perception_goat.pos_x_wolf);
            }
        }
        next_x += cos(goat->angle_actuel) * goat->speed;
        next_y += sin(goat->angle_actuel) * goat->speed;

        if (fabs(next_x - goat->x) > fabs(next_y - goat->y))
        {
            goat->direction_sprite = (next_x > goat->x) ? 2 : 4;
        }
        else
        {
            goat->direction_sprite = (next_y > goat->y) ? 3 : 1;
        }
    }
    else if (action == ACTION_BROUTER)
    {
        goat->speed = 0;
    }

    if (next_x < MARGE)
        next_x = MARGE;
    if (next_y < MARGE)
        next_y = MARGE;
    if (next_x > LARGEUR - MARGE - WIDTH_GOAT)
        next_x = LARGEUR - MARGE - WIDTH_GOAT;
    if (next_y > HAUTEUR - MARGE - HEIGHT_GOAT)
        next_y = HAUTEUR - MARGE - HEIGHT_GOAT;

    int collision = 0;

    // 1. Création de la hitbox future de la chèvre (Ajuste les marges 15.0f et 10.0f selon ton sprite)
    Hitbox hb_future = creer_hitbox(next_x, next_y, WIDTH_GOAT, HEIGHT_GOAT, 15.0f, 10.0f);
    
    // 2. Hitbox du lac (pas de marge, il garde sa vraie taille)
    Hitbox hb_lac = creer_hitbox(LAC_X, LAC_Y, LAC_WIDTH, LAC_HEIGHT, 0.0f, 0.0f);

    // Collision avec le lac

    if (check_collision_rect(hb_future.x, hb_future.y, hb_future.w, hb_future.h, 
                             hb_lac.x, hb_lac.y, hb_lac.w, hb_lac.h)) {
        collision = 1;
        goat->timer_mouvement = 0;
    }

    // Collision avec les autres chèvres
    if (!collision) {
        for (int j = 0; j < monde_courant->nb_goat; j++) {
            if (monde_courant->goats_tab[j] != goat) {
                // Hitbox de l'autre chèvre déjà en place
                Hitbox hb_autre = creer_hitbox(monde_courant->goats_tab[j]->x, monde_courant->goats_tab[j]->y, 
                                               WIDTH_GOAT, HEIGHT_GOAT, 15.0f, 10.0f);

                if (check_collision_rect(hb_future.x, hb_future.y, hb_future.w, hb_future.h, 
                                         hb_autre.x, hb_autre.y, hb_autre.w, hb_autre.h)) {
                    collision = 1;
                    goat->timer_mouvement = 0;
                    break;
                }
            }
        }
    }
    
    if (tick_animation % 6 == 0) {
        if (goat->speed > 0 && !collision) {
            goat->frame = (goat->frame + 1) % 4;
        }
        else
        {
            goat->frame = 0;
        }
    }
    return goat;
}


Wolf * update_wolf(monde *monde_courant, Wolf *wolf, ActionWolf action, int tick_animation, PerceptionWolf perception_wolf)
{
    float next_x = wolf->x;
    float next_y = wolf->y;
    
    if (action == ACTION_WOLF_ERRER || action == ACTION_WOLF_CHASSER) {
        wolf->speed = 2;

        if (action == ACTION_WOLF_CHASSER) {
            if (perception_wolf.pos_x_goat != -1) {
                // Aller vers la chèvre
                wolf->angle_actuel = atan2(perception_wolf.pos_y_goat - wolf->y, perception_wolf.pos_x_goat - wolf->x);
            }
        }

        next_x += cos(wolf->angle_actuel) * wolf->speed;
        next_y += sin(wolf->angle_actuel) * wolf->speed;
        
        if (fabs(next_x - wolf->x) > fabs(next_y - wolf->y)) {
            wolf->direction_sprite = (next_x > wolf->x) ? 2 : 4;
        } else {
            wolf->direction_sprite = (next_y > wolf->y) ? 3 : 1;
        }
    } 
    else if (action == ACTION_WOLF_ARRET) {
        wolf->speed = 0;
    }

    if (next_x < MARGE) next_x = MARGE;
    if (next_y < MARGE) next_y = MARGE;
    if (next_x > LARGEUR - MARGE - WIDTH_WOLF) next_x = LARGEUR - MARGE - WIDTH_WOLF;
    if (next_y > HAUTEUR - MARGE - HEIGHT_WOLF) next_y = HAUTEUR - MARGE - HEIGHT_WOLF;

    int collision = 0;

    // Hitbox future du loup (Marges à ajuster, ex: 20px et 15px)
    Hitbox hb_future = creer_hitbox(next_x, next_y, WIDTH_WOLF, HEIGHT_WOLF, 20.0f, 15.0f);
    Hitbox hb_lac = creer_hitbox(LAC_X, LAC_Y, LAC_WIDTH, LAC_HEIGHT, 0.0f, 0.0f);

    if (check_collision_rect(hb_future.x, hb_future.y, hb_future.w, hb_future.h, 
                             hb_lac.x, hb_lac.y, hb_lac.w, hb_lac.h)) {
        collision = 1;
        wolf->timer_mouvement = 0;
    }

    if (!collision) {
        for (int j = 0; j < monde_courant->nb_wolf; j++) {
            if (monde_courant->wolfs_tab[j] != wolf) {
                Hitbox hb_autre = creer_hitbox(monde_courant->wolfs_tab[j]->x, monde_courant->wolfs_tab[j]->y, 
                                               WIDTH_WOLF, HEIGHT_WOLF, 20.0f, 15.0f);

                if (check_collision_rect(hb_future.x, hb_future.y, hb_future.w, hb_future.h, 
                                         hb_autre.x, hb_autre.y, hb_autre.w, hb_autre.h)) {
                    collision = 1;
                    wolf->timer_mouvement = 0;
                    break;
                }
            }
        }
    }
    
    if (tick_animation % 6 == 0) {
        if (wolf->speed > 0 && !collision) {
            wolf->frame = (wolf->frame + 1) % 4;
        } else {
            wolf->frame = 0;
        }
    }
    return wolf;
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

/* ========== Loups ========== */

/**
 * Synopsis : Initialise un loup avec des coordonnées aléatoires et une direction par défaut.
 * Entrée   : Pointeur vers la structure Wolf à initialiser.
 * Sortie   : Pointeur vers la structure Wolf initialisée.
 */
Wolf *init_wolf(Wolf *wolf)
{
    wolf->x = rand() % (LARGEUR - WIDTH_WOLF);
    wolf->y = rand() % (HAUTEUR - HEIGHT_WOLF);

    wolf->dir_x = wolf->x;
    wolf->dir_y = wolf->y;

    wolf->speed = 0;
    wolf->direction_sprite = 1 + (rand() % 4);
    wolf->frame = 0;
    wolf->timer_mouvement = 10;
    wolf->en_mouvement = 0;
    wolf->angle_actuel = 0.0f;

    for (int i = 0; i < NB_ACTIONS_WOLF; i++)
    {
        wolf->table_interets[i] = 0.0f;
    }

    return wolf;
}

/* ENtrées : le monde actuel dans sa structure et un wolf
    Sotie : Le monde mis a jour avec le nouveau wolf en plus
    Synopsis : prend un wolf et l'ajoute au monde*/
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

PerceptionWolf calculer_perception_wolf(Wolf *wolf, monde *monde_courant)
{
    PerceptionWolf perception;
    perception.dist_goat_proche = 300.0f; // Valeur par défaut
    perception.goats_tab = monde_courant->goats_tab;
    perception.nb_goat = monde_courant->nb_goat;
    perception.pos_x_fermier = monde_courant->fermiers->x;
    perception.pos_y_fermier = monde_courant->fermiers->y;

    for (int i = 0; i < monde_courant->nb_goat; i++)
    {
        Goat *current_goat = monde_courant->goats_tab[i];
        float dist_x = current_goat->x - wolf->x;
        float dist_y = current_goat->y - wolf->y;
        float distance = sqrt(dist_x * dist_x + dist_y * dist_y);

        if (distance < perception.dist_goat_proche)
        {
            perception.dist_goat_proche = distance;
            perception.pos_x_goat = current_goat->x;
            perception.pos_y_goat = current_goat->y;
        }
    }
    return perception;
}


Wolf *update_wolf(monde *monde_courant, Wolf *wolf, ActionWolf action, int tick_animation, PerceptionWolf perception_wolf)
{
    float next_x = wolf->x;
    float next_y = wolf->y;

    if (action == ACTION_WOLF_ERRER || action == ACTION_WOLF_CHASSER)
    {
        wolf->speed = 2;

        if (action == ACTION_WOLF_CHASSER)
        {
            if (perception_wolf.pos_x_goat != -1)
            {
                // Aller vers la chèvre
                wolf->angle_actuel = atan2(perception_wolf.pos_y_goat - wolf->y, perception_wolf.pos_x_goat - wolf->x);
            }
        }

        next_x += cos(wolf->angle_actuel) * wolf->speed;
        next_y += sin(wolf->angle_actuel) * wolf->speed;

        if (fabs(next_x - wolf->x) > fabs(next_y - wolf->y))
        {
            wolf->direction_sprite = (next_x > wolf->x) ? 2 : 4;
        }
        else
        {
            wolf->direction_sprite = (next_y > wolf->y) ? 3 : 1;
        }
    }
    else if (action == ACTION_WOLF_ARRET)
    {
        wolf->speed = 0;
    }

    if (next_x < MARGE)
        next_x = MARGE;
    if (next_y < MARGE)
        next_y = MARGE;
    if (next_x > LARGEUR - MARGE - WIDTH_WOLF)
        next_x = LARGEUR - MARGE - WIDTH_WOLF;
    if (next_y > HAUTEUR - MARGE - HEIGHT_WOLF)
        next_y = HAUTEUR - MARGE - HEIGHT_WOLF;

    int collision = 0;

    // Collision avec le lac
    if (check_collision_rect(next_x, next_y, WIDTH_WOLF, HEIGHT_WOLF,
                             LAC_X, LAC_Y, LAC_WIDTH, LAC_HEIGHT))
    {
        collision = 1;
        wolf->timer_mouvement = 0;
    }
    for (int j = 0; j < monde_courant->nb_wolf; j++)
    {
        if (monde_courant->wolfs_tab[j] != wolf)
        {
            if (check_collision_rect(next_x, next_y, WIDTH_WOLF, HEIGHT_WOLF,
                                     monde_courant->wolfs_tab[j]->x, monde_courant->wolfs_tab[j]->y,
                                     WIDTH_WOLF, HEIGHT_WOLF))
            {
                collision = 1;
                wolf->timer_mouvement = 0;
                break;
            }
        }
    }

    if (!collision)
    {
        wolf->x = next_x;
        wolf->y = next_y;
        wolf->dir_x = next_x;
        wolf->dir_y = next_y;

    }
    else
    {
        wolf->dir_x = wolf->x;
        wolf->dir_y = wolf->y;
    }

    if (tick_animation % 6 == 0)
    {
        if (wolf->speed > 0 && !collision)
        {
            wolf->frame = (wolf->frame + 1) % 4;
        }
        else
        {
            wolf->frame = 0;
        }
    }
    return wolf;
}

/* ENtrées : le tableau de wolf, nombre de wolf
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
        monde_courant->goats_tab =
            malloc(monde_courant->capacite_max_goat * sizeof(Goat *));
        monde_courant->wolfs_tab =
            malloc(monde_courant->capacite_max_wolf * sizeof(Wolf *));

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
    ActionFermier action_fermier;

    // Fermier - Décision
    PerceptionFermier perception_fermier;
    perception_fermier.input_x = input_x;
    perception_fermier.input_y = input_y;
    action_fermier = decider_action_fermier(monde_courant->fermiers, perception_fermier);

    // Chèvres
    for (int i = 0; i < monde_courant->nb_goat; i++)
    {
        Goat *goat = monde_courant->goats_tab[i];
        goat->decision_cooldown--;

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

        if (wolf->decision_cooldown <= 0)
        {
            PerceptionWolf perception_wolf = calculer_perception_wolf(wolf, monde_courant);
            evaluer_interets_wolf(wolf, perception_wolf);
            wolf->action_choisi = choisir_action_softmax(wolf->table_interets, NB_ACTIONS_WOLF);
            if (wolf->action_choisi == ACTION_WOLF_ERRER)
                wolf->angle_actuel = ((float)rand() / (float)RAND_MAX) * 2.0 * 3.14;
            wolf->decision_cooldown = 30 + (rand() % 45); // Entre 0.5s et 1.25s à 60 FPS
        }
        update_wolf(monde_courant, wolf, wolf->action_choisi, tick_animation, calculer_perception_wolf(wolf, monde_courant));
    }

    // ========
    // Action physique du Fermier
    // ========
    Fermier *fermier_actuel = monde_courant->fermiers;
    float next_fermier_x = fermier_actuel->x;
    float next_fermier_y = fermier_actuel->y;

    float vitesse_actuelle = fermier_actuel->speed;
    if (action_fermier.dx != 0 && action_fermier.dy != 0)
    {
        vitesse_actuelle /= sqrt(2);
    }

    if (action_fermier.dy == 1)
    {
        next_fermier_y -= vitesse_actuelle;
        fermier_actuel->direction_sprite = 1;
    }
    if (action_fermier.dy == -1)
    {
        next_fermier_y += vitesse_actuelle;
        fermier_actuel->direction_sprite = 3;
    }
    if (action_fermier.dx == 1)
    {
        next_fermier_x -= vitesse_actuelle;
        fermier_actuel->direction_sprite = 4;
    }
    if (action_fermier.dx == -1)
    {
        next_fermier_x += vitesse_actuelle;
        fermier_actuel->direction_sprite = 2;
    }

    if (action_fermier.dx == 0 && action_fermier.dy == 0)
    {
        fermier_actuel->frame = 5;
        fermier_actuel->direction_sprite = 0;
    }
    else
    {
        if (next_fermier_x < MARGE)
            next_fermier_x = MARGE;
        if (next_fermier_x > LARGEUR - MARGE - WIDTH_FERMIER)
            next_fermier_x = LARGEUR - MARGE - WIDTH_FERMIER;
        if (next_fermier_y < MARGE)
            next_fermier_y = MARGE;
        if (next_fermier_y > HAUTEUR - MARGE - HEIGHT_FERMIER)
            next_fermier_y = HAUTEUR - MARGE - HEIGHT_FERMIER;

        int collision_fermier = 0;

        // Hitbox future du fermier (Marges à ajuster, le fermier est souvent fin)
        Hitbox hb_future_fermier = creer_hitbox(next_fermier_x, next_fermier_y, WIDTH_FERMIER, HEIGHT_FERMIER, 10.0f, 10.0f);
        Hitbox hb_lac = creer_hitbox(LAC_X, LAC_Y, LAC_WIDTH, LAC_HEIGHT, 0.0f, 0.0f);

        // Collision avec le lac

        if (check_collision_rect(hb_future_fermier.x, hb_future_fermier.y, hb_future_fermier.w, hb_future_fermier.h, 
                                 hb_lac.x, hb_lac.y, hb_lac.w, hb_lac.h)) {
            collision_fermier = 1;
        }

        // Collision avec les chèvres
        if (!collision_fermier) {
            for (int j = 0; j < monde_courant->nb_goat; j++) {
                Hitbox hb_goat = creer_hitbox(monde_courant->goats_tab[j]->x, monde_courant->goats_tab[j]->y, 
                                              WIDTH_GOAT, HEIGHT_GOAT, 15.0f, 10.0f);

                if (check_collision_rect(hb_future_fermier.x, hb_future_fermier.y, hb_future_fermier.w, hb_future_fermier.h, 
                                         hb_goat.x, hb_goat.y, hb_goat.w, hb_goat.h)) {
                    collision_fermier = 1;
                    break;
                }
            }
        }

        if (!collision_fermier)
        {
            fermier_actuel->x = next_fermier_x;
            fermier_actuel->y = next_fermier_y;
        }

        if (tick_animation % 6 == 0)
        {
            fermier_actuel->frame = (fermier_actuel->frame + 1) % 9;
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
    if (monde_courant->fermiers)
    {
        dessiner_entite(2, monde_courant->fermiers->x, monde_courant->fermiers->y,
                        monde_courant->fermiers->frame,
                        monde_courant->fermiers->direction_sprite);
    }
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
    actualiser_ecran();
}
