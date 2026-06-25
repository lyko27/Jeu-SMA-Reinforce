/**
 * @file monde.c
 * @brief Implémentation des fonctions de gestion globale du monde de simulation
 * @details Contient la sélection d'actions par softmax, les fonctions de collision et d'ajustement des hitboxes,
 * l'allocation/initialisation/mise à jour du monde ainsi que l'affichage des entités
 */

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
 * @brief Sélectionne une action de manière probabiliste selon la méthode Softmax sur un tableau d'intérêts
 * @details Applique la fonction exponentielle sur chaque intérêt après soustraction du maximum
 * pour éviter le dépassement de capacité, normalise les valeurs puis effectue un tirage aléatoire
 * @param[in] tableau_interets Tableau contenant les valeurs d'intérêt pour chaque action possible
 * @param[in] nombre_actions Nombre d'actions possibles (taille du tableau)
 * @return Indice de l'action choisie
 */
int choisir_action_softmax(float *tableau_interets, int nombre_actions)
{
    float maximum_interet = tableau_interets[0];
    for (int index_action = 1; index_action < nombre_actions; index_action++)
    {
        // ici on trouve l'action avec le meilleur intérêt
        if (tableau_interets[index_action] > maximum_interet)
        {
            maximum_interet = tableau_interets[index_action];
        }
    }

    // on calcule exp(intérêt de chaque action) et on met ça dans le tableau de probabilité
    float somme_exponentielles = 0.0f;
    float tableau_probabilites[nombre_actions];

    for (int index_action = 0; index_action < nombre_actions; index_action++)
    {
        tableau_probabilites[index_action] = exp(tableau_interets[index_action] - maximum_interet);
        somme_exponentielles += tableau_probabilites[index_action]; // on calcule la somme des probabilités en même temps
    }

    float tirage_aleatoire = (float)rand() / (float)RAND_MAX;
    float somme_cumulee = 0.0f;
    for (int index_action = 0; index_action < nombre_actions; index_action++)
    {
        somme_cumulee += (tableau_probabilites[index_action] / somme_exponentielles); // on normalise la probabilité pour qu'elle soit comprise entre 0 et 1
        if (tirage_aleatoire <= somme_cumulee)
        {
            return index_action;
        }
    }
    return 0; // sécurité
}

/**
 * @brief Vérifie la collision AABB entre deux rectangles voir 2.10.2 du cours
 * @param[in] x_premier Position horizontale du premier rectangle
 * @param[in] y_premier Position verticale du premier rectangle
 * @param[in] largeur_premier Largeur du premier rectangle
 * @param[in] hauteur_premier Hauteur du premier rectangle
 * @param[in] x_second Position horizontale du second rectangle
 * @param[in] y_second Position verticale du second rectangle
 * @param[in] largeur_second Largeur du second rectangle
 * @param[in] hauteur_second Hauteur du second rectangle
 * @return 1 s'il y a collision, 0 sinon
 */
int check_collision_rect(float x_premier, float y_premier, float largeur_premier, float hauteur_premier, float x_second, float y_second, float largeur_second, float hauteur_second)
{
    if (x_premier + largeur_premier < x_second || x_second + largeur_second < x_premier || y_premier + hauteur_premier < y_second || y_second + hauteur_second < y_premier)
    {
        return 0; // Pas de collision
    }
    return 1; // Collision
}

/// ========== Hitbox ==========

/**
 * @brief Crée une hitbox en fonction des maramètre d'entré
 * @details Permet d'ajuster la hitbox par rapport au sprite d'une entité en ignorant les marges transparentes
 * @param[in] position_x Position horizontale de l'entité
 * @param[in] position_y Position verticale de l'entité
 * @param[in] largeur Largeur de l'entité
 * @param[in] hauteur Hauteur de l'entité
 * @param[in] marge_gauche Ajustement de la marge gauche
 * @param[in] marge_droite Ajustement de la marge droite
 * @param[in] marge_haut Ajustement de la marge haute
 * @param[in] marge_bas Ajustement de la marge basse
 * @return Hitbox configurée avec les marges appliquées
 */
Hitbox creer_hitbox(float position_x, float position_y, float largeur, float hauteur, float marge_gauche, float marge_droite, float marge_haut, float marge_bas)
{
    Hitbox hitbox;
    hitbox.x = position_x + marge_gauche;
    hitbox.y = position_y + marge_haut;
    hitbox.w = largeur - (marge_gauche + marge_droite);
    hitbox.h = hauteur - (marge_haut + marge_bas);

    // Sécurité : éviter d'avoir une largeur ou hauteur négative ou nulle
    if (hitbox.w <= 0.0f)
        hitbox.w = 1.0f;
    if (hitbox.h <= 0.0f)
        hitbox.h = 1.0f;

    return hitbox;
}

/**
 * @brief Calcule et retourne la hitbox du fermier
 * @param[in] position_x Position horizontale actuelle du fermier
 * @param[in] position_y Position verticale actuelle du fermier
 * @return Hitbox associée au fermier
 */
Hitbox get_hitbox_fermier(float position_x, float position_y)
{
    return creer_hitbox(position_x, position_y, WIDTH_FERMIER, HEIGHT_FERMIER, 10.0f, 1.0f, 10.0f, 10.0f);
}

/**
 * @brief Calcule et retourne la hitbox d'une chèvre
 * @param[in] position_x Position horizontale actuelle de la chèvre
 * @param[in] position_y Position verticale actuelle de la chèvre
 * @return Hitbox associée à la chèvre
 */
Hitbox get_hitbox_goat(float position_x, float position_y)
{
    return creer_hitbox(position_x, position_y, WIDTH_GOAT, HEIGHT_GOAT, 0.5f, 28.0f, 10.0f, 10.0f);
}

/**
 * @brief Calcule et retourne la hitbox d'un loup
 * @param[in] position_x Position horizontale actuelle du loup
 * @param[in] position_y Position verticale actuelle du loup
 * @return Hitbox associée au loup
 */
Hitbox get_hitbox_wolf(float position_x, float position_y)
{
    return creer_hitbox(position_x, position_y, WIDTH_WOLF, HEIGHT_WOLF, 10.0f, 10.0f, 5.0f, 5.0f);
}

/**
 * @brief Vérifie si une hitbox est en intersection avec l'un des obstacles statiques de la carte (lac, maison)
 * @param[in] hitbox hitboxà vérifier
 * @return 1 s'il y a collision avec un obstacle, 0 sinon
 */
int check_collision_obstacles(Hitbox hitbox)
{
    // Le Lac
    Hitbox hitbox_lac = creer_hitbox(LAC_X, LAC_Y, LAC_WIDTH, LAC_HEIGHT, 0.0f, 0.0f, 0.0f, 0.0f);
    if (check_collision_rect(hitbox.x, hitbox.y, hitbox.w, hitbox.h, hitbox_lac.x, hitbox_lac.y, hitbox_lac.w, hitbox_lac.h))
    {
        return 1;
    }

    // La Maison
    Hitbox hitbox_maison = creer_hitbox(MAISON_X, MAISON_Y, MAISON_WIDTH, MAISON_HEIGHT, 0.0f, 0.0f, 0.0f, 0.0f);
    if (check_collision_rect(hitbox.x, hitbox.y, hitbox.w, hitbox.h, hitbox_maison.x, hitbox_maison.y, hitbox_maison.w, hitbox_maison.h))
    {
        return 1;
    }

    return 0;
}

/* ========= Monde ========= */

/**
 * @brief Crée et alloue la structure du monde
 * @details Alloue la structure monde ainsi que les tableaux de pointeurs de chèvres et loups
 * Gère de manière sécurisée les échecs d'allocations pour éviter les fuites de mémoire
 * @param[in] largeur Largeur souhaitée pour la simulation
 * @param[in] hauteur Hauteur souhaitée pour la simulation
 * @return Pointeur vers le monde créé et initialisé, ou NULL en cas d'erreur
 */
monde *creer_monde(int largeur, int hauteur)
{
    monde *monde_courant = malloc(sizeof(monde));
    if (!monde_courant)
    {
        return NULL;
    }

    monde_courant->largeur = largeur;
    monde_courant->hauteur = hauteur;
    monde_courant->capacite_max_goat = 100;
    monde_courant->nb_goat = 0;
    monde_courant->capacite_max_wolf = 100;
    monde_courant->nb_wolf = 0;
    monde_courant->mode = 0;
    monde_courant->en_pause = 0;

    monde_courant->goats_tab = malloc(monde_courant->capacite_max_goat * sizeof(Goat *));
    monde_courant->wolfs_tab = malloc(monde_courant->capacite_max_wolf * sizeof(Wolf *));

    if (!monde_courant->goats_tab || !monde_courant->wolfs_tab)
    {
        free(monde_courant->goats_tab);
        free(monde_courant->wolfs_tab);
        free(monde_courant);
        return NULL;
    }

    return monde_courant;
}

/**
 * @brief Génère les entités (fermier, chèvres, loups) dans le monde de la simulation
 * @details Alloue et place le fermier devant sa maison, place 20 chèvres en grille et génère 4 loups
 * @param[in,out] monde_courant Pointeur vers le monde dans lequel générer les entités
 * @return Pointeur vers le monde mis à jour, ou NULL en cas d'échec d'allocation
 */
monde *generer_un_monde(monde *monde_courant)
{
    Fermier *fermier_genere = malloc(sizeof(Fermier));
    if (fermier_genere)
    {
        fermier_genere->frame = 0;
        fermier_genere->direction_sprite = 2;
        fermier_genere = init_fermier(fermier_genere);
        monde_courant->fermiers = fermier_genere;
    }
    else
    {
        return NULL;
    }

    int position_depart_x = 1000;
    int position_depart_y = 300;
    int ecart_x = 120;
    int ecart_y = 100;
    int nombre_colonnes = 4;

    for (int index_goat = 0; index_goat < NB_CHEVRES; index_goat++)
    {
        Goat *goat_generee = malloc(sizeof(Goat));
        if (goat_generee)
        {
            goat_generee->frame = 0;
            goat_generee->direction_sprite = 2;
            
            // rectangle de colonne de 5 chèvre
             int index_colonne = index_goat % nombre_colonnes;
            int index_ligne = index_goat / nombre_colonnes;
            int position_spawn_x = position_depart_x + index_colonne * ecart_x;
            int position_spawn_y = position_depart_y + index_ligne * ecart_y;
            
            goat_generee = init_goat(goat_generee, position_spawn_x, position_spawn_y);
            monde_courant = ajouter_goat(monde_courant, goat_generee);
        }
        else
        {
            return NULL;
        }
    }

    for (int index_wolf = 0; index_wolf < NB_LOUPS; index_wolf++)
    {
        Wolf *wolf_genere = malloc(sizeof(Wolf));
        if (wolf_genere)
        {
            wolf_genere->frame = 0;
            wolf_genere->direction_sprite = 2;
            wolf_genere = init_wolf(wolf_genere);
            monde_courant = ajouter_wolf(monde_courant, wolf_genere);
        }
        else
        {
            return NULL;
        }
    }
    return monde_courant;
}

/**
 * @brief Met à jour l'ensemble des entités et l'état global du monde pour un tick de simulation
 * @details Gère la prise de décision, les déplacements, les cooldowns d'invisibilité,
 * les collisions et les attaques entre les agents (Fermier, Loups, Chèvres)
 * @param[in,out] monde_courant Pointeur vers le monde à mettre à jour
 * @param[in] tick_animation Indice du tick d'animation actuel
 * @param[in] input_x Entrée utilisateur horizontale (clavier) pour le fermier
 * @param[in] input_y Entrée utilisateur verticale (clavier) pour le fermier
 * @return Pointeur vers le monde mis à jour
 */
monde *mis_à_jour_monde(monde *monde_courant, int tick_animation, int input_x, int input_y)
{
    // Chèvres
    for (int index_goat = 0; index_goat < monde_courant->nb_goat; index_goat++)
    {
        Goat *goat_courant = monde_courant->goats_tab[index_goat];
        goat_courant->decision_cooldown--;
        if (goat_courant->cooldown_dinvisibilite > 0)
        {
            goat_courant->cooldown_dinvisibilite--; // on descend l'invisibilité si la chèvre est en cooldown
        }

        // On calcule la perception
        PerceptionGoat perception_goat_courant = calculer_perception_goat(goat_courant, monde_courant);

        // Prise de décision (seulement à l'expiration du cooldown)
        if (goat_courant->decision_cooldown <= 0)
        {
            evaluer_interets_goat(goat_courant, perception_goat_courant);
            goat_courant->action_choisi = choisir_action_softmax(goat_courant->table_interets, NB_ACTIONS);
            if (goat_courant->action_choisi == ACTION_ERRER)
            {
                goat_courant->angle_actuel = ((float)rand() / (float)RAND_MAX) * 2.0f * 3.14159265f;
            }
            goat_courant->decision_cooldown = 30 + (rand() % 45); // Entre 0.5s et 1.25s à 60 FPS
        }
        update_goat(monde_courant, goat_courant, goat_courant->action_choisi, tick_animation, perception_goat_courant);
    }

    // Loups
    for (int index_wolf = 0; index_wolf < monde_courant->nb_wolf; index_wolf++)
    {
        Wolf *wolf_courant = monde_courant->wolfs_tab[index_wolf];
        wolf_courant->decision_cooldown--;
        if (wolf_courant->cooldown_dinvisibilite > 0)
        {
            wolf_courant->cooldown_dinvisibilite--; // on descend l'invisibilité si le loup est en cooldown
        }

        // On calcule la perception
        PerceptionWolf perception_wolf_courant = calculer_perception_wolf(wolf_courant, monde_courant);

        if (wolf_courant->decision_cooldown <= 0)
        {
            evaluer_interets_wolf_rl(wolf_courant, perception_wolf_courant);
            wolf_courant->action_choisi = choisir_action_softmax(wolf_courant->table_interets, NB_ACTIONS_WOLF);
            if (wolf_courant->action_choisi == ACTION_WOLF_ERRER)
            {
                wolf_courant->angle_actuel = ((float)rand() / (float)RAND_MAX) * 2.0f * 3.14159265f;
            }
            wolf_courant->decision_cooldown = 30 + (rand() % 45); // Entre 0.5s et 1.25s à 60 FPS
        }
        update_wolf(monde_courant, wolf_courant, wolf_courant->action_choisi, tick_animation, perception_wolf_courant);
    }

    // Action du Fermier
    monde_courant = mis_a_jour_fermier(monde_courant, tick_animation, input_x, input_y);

    // attaque loup chèvre
    for (int index_wolf = 0; index_wolf < monde_courant->nb_wolf; index_wolf++)
    {
        Wolf *wolf_courant = monde_courant->wolfs_tab[index_wolf];
        Hitbox hitbox_wolf = get_hitbox_wolf(wolf_courant->x, wolf_courant->y);

        for (int index_goat_cible = 0; index_goat_cible < monde_courant->nb_goat; index_goat_cible++)
        {
            Goat *goat_courant = monde_courant->goats_tab[index_goat_cible];
            if (goat_courant->cooldown_dinvisibilite == 0)
            {
                Hitbox hitbox_goat = get_hitbox_goat(goat_courant->x, goat_courant->y);

                if (check_collision_rect(hitbox_wolf.x, hitbox_wolf.y, hitbox_wolf.w, hitbox_wolf.h, hitbox_goat.x, hitbox_goat.y, hitbox_goat.w, hitbox_goat.h))
                {
                    goat_courant->hp--;
                    if (goat_courant->hp <= 0)
                    {
                        mourrir_goat(monde_courant, index_goat_cible);
                        index_goat_cible--;
                    }
                    else
                    {
                        goat_courant->cooldown_dinvisibilite = 180; // 3 secondes d'invisibilité après avoir été attaquée
                    }
                }
            }
        }
    }
    // attaque fermier loup
    for (int index_wolf = 0; index_wolf < monde_courant->nb_wolf; index_wolf++)
    {
        Wolf *wolf_courant = monde_courant->wolfs_tab[index_wolf];
        if (wolf_courant->cooldown_dinvisibilite == 0)
        {
            Hitbox hitbox_wolf = get_hitbox_wolf(wolf_courant->x, wolf_courant->y);
            Hitbox hitbox_fermier = get_hitbox_fermier(monde_courant->fermiers->x, monde_courant->fermiers->y);

            if (check_collision_rect(hitbox_fermier.x, hitbox_fermier.y, hitbox_fermier.w, hitbox_fermier.h, hitbox_wolf.x, hitbox_wolf.y, hitbox_wolf.w, hitbox_wolf.h))
            {
                // Le fermier élimine le loup
                wolf_courant->hp--;
                if (wolf_courant->hp <= 0)
                {
                    mourrir_wolf(monde_courant, index_wolf);
                    index_wolf--;
                }
                else
                {
                    wolf_courant->cooldown_dinvisibilite = 180; // 3 secondes d'invisibilité après avoir été attaqué
                }
            }
        }
    }

    return monde_courant;
}

/* ========= Affichage ========= */

/**
 * @brief Affiche le monde courant, incluant le terrain, le fermier, les chèvres et les loups
 * @param[in] monde_courant Pointeur vers le monde à afficher
 */
void afficher_monde(monde *monde_courant)
{
    if (!monde_courant)
        return;
    dessiner_monde();

    for (int index_goat = 0; index_goat < monde_courant->nb_goat; index_goat++)
    {
        Goat *goat_courant = monde_courant->goats_tab[index_goat];
        if (goat_courant)
            dessiner_entite(1, goat_courant->x, goat_courant->y, goat_courant->frame, goat_courant->direction_sprite);
    }
    for (int index_wolf = 0; index_wolf < monde_courant->nb_wolf; index_wolf++)
    {
        Wolf *wolf_courant = monde_courant->wolfs_tab[index_wolf];
        if (wolf_courant)
            dessiner_entite(4, wolf_courant->x, wolf_courant->y, wolf_courant->frame, wolf_courant->direction_sprite);
    }
    if (monde_courant->fermiers)
    {
        dessiner_entite(2, monde_courant->fermiers->x, monde_courant->fermiers->y, monde_courant->fermiers->frame, monde_courant->fermiers->direction_sprite);
    }

    afficher_planche(monde_courant->nb_goat, monde_courant->nb_wolf);
    afficher_mode(monde_courant->mode);
    if(monde_courant->en_pause == 1 && monde_courant->nb_goat > 0 && monde_courant->nb_wolf > 0)
    {
        afficher_pause();
    } 
    // Vérifier si le jeu est fini 
    if (monde_courant->nb_goat <= 0) 
    {
        afficher_fin(0);
    }
    else if (monde_courant->nb_wolf <= 0) 
    {
        afficher_fin(1);
    }
    actualiser_ecran();
}
