/**
 * @file monde.h
 * @brief Déclarations globales et structures principales du monde
 * @details Définit les constantes de la carte, les obstacles, les structures de données principales (monde, Hitbox) et les prototypes de fonctions générales
 */

#ifndef MONDE_H
#define MONDE_H

#include <stdlib.h>
#include <time.h>
#include <stdint.h>

/**
 * @brief Version thread-safe de rand() avec graine locale par thread (__thread)
 * @return Valeur aléatoire générée
 */
static inline int thread_safe_rand(void)
{
    static __thread unsigned int seed = 0;
    if (seed == 0)
    {
        // Graine initiale basée sur le temps et l'adresse mémoire du thread
        seed = (unsigned int)time(NULL) ^ (unsigned int)(uintptr_t)&seed;
    }
    return rand_r(&seed);
}

// Redéfinition de rand()
#ifdef rand
#undef rand
#endif
#define rand() thread_safe_rand()

#include "goat.h"
#include "loup.h"
#include "fermier.h"

// entite
#define NB_LOUPS 4
#define NB_CHEVRES 20

// map
#define LARGEUR 2048
#define HAUTEUR 1152
#define MARGE 90

// Zone du lac (obstacle)
#define LAC_X 1050
#define LAC_Y 803
#define LAC_WIDTH 195
#define LAC_HEIGHT 300

// Zone de la maison (obstacle)
#define MAISON_X 738
#define MAISON_Y 0
#define MAISON_WIDTH 175
#define MAISON_HEIGHT 195

// taille des hitbox des entités
#define WIDTH_GOAT 50
#define HEIGHT_GOAT 30
#define WIDTH_WOLF 50
#define HEIGHT_WOLF 60
#define WIDTH_FERMIER 30
#define HEIGHT_FERMIER 60
#define VITESSE_FERMIER 5

/**
 * @struct monde
 * @brief Structure représentant l'ensemble du monde de la simulation
 */
typedef struct monde_t
{
    int largeur;              ///< Largeur de la map
    int hauteur;              ///< Hauteur de la mzp
    Fermier *fermiers;        ///< Pointeur vers l'agent Fermier
    Goat **goats_tab;         ///< Tableau de pointeurs vers les chèvres
    int capacite_max_goat;    ///< Capacité d'allocation actuelle pour le tableau des chèvres
    int nb_goat;              ///< Nombre de chèvres actuellement en vie
    Wolf **wolfs_tab;         ///< Tableau de pointeurs vers les loups
    int capacite_max_wolf;    ///< Capacité d'allocation actuelle pour le tableau des loups
    int nb_wolf;              ///< Nombre de loups actuellement en vie
    int mode;                 ///< Mode de contrôle du fermier (0 = clavier/manuel, 1 = Automatique/Reinforce)
    int en_pause;             ///< Indicateur d'état de pause (1 si en pause, 0 sinon)
} monde;

/**
 * @struct Hitbox
 * @brief Structure représentant une boîte de collision rectangulaire
 */
typedef struct
{
    float x; ///< Position x du coin supérieur gauche de la hitbox
    float y; ///< Position y du coin supérieur gauche de la hitbox
    float w; ///< Largeur de la hitbox
    float h; ///< Hauteur de la hitbox
} Hitbox;

/**
 * @brief Crée une hitbox avec des marges
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
Hitbox creer_hitbox(float position_x, float position_y, float largeur, float hauteur, float marge_gauche, float marge_droite, float marge_haut, float marge_bas);

/**
 * @brief Calcule et retourne hitbox du fermier
 * @param[in] position_x Position horizontale actuelle du fermier
 * @param[in] position_y Position verticale actuelle du fermier
 * @return Hitbox associée au fermier
 */
Hitbox get_hitbox_fermier(float position_x, float position_y);

/**
 * @brief Calcule et retourne la hitbox d'une chèvre
 * @param[in] position_x Position horizontale actuelle de la chèvre
 * @param[in] position_y Position verticale actuelle de la chèvre
 * @return Hitbox associée à la chèvre
 */
Hitbox get_hitbox_goat(float position_x, float position_y);

/**
 * @brief Calcule et retourne la hitbox d'un loup
 * @param[in] position_x Position horizontale actuelle du loup
 * @param[in] position_y Position verticale actuelle du loup
 * @return Hitbox associée au loup
 */
Hitbox get_hitbox_wolf(float position_x, float position_y);

/**
 * @brief Crée et alloue la structure du monde avec les dimensions spécifiées
 * @param[in] largeur Largeur souhaitée pour la simulation
 * @param[in] hauteur Hauteur souhaitée pour la simulation
 * @return Pointeur vers le monde créé et initialisé
 */
monde *creer_monde(int largeur, int hauteur);

/**
 * @brief Génère les entités (fermier, chèvres, loups) dans le monde de la simulation
 * @param[in,out] monde_courant Pointeur vers le monde dans lequel générer les entités
 * @return Pointeur vers le monde mis à jour
 */
monde *generer_un_monde(monde *monde_courant);

/**
 * @brief Met à jour l'ensemble des entités et l'état global du monde pour un tick de simulation
 * @param[in,out] monde_courant Pointeur vers le monde à mettre à jour
 * @param[in] tick_animation Indice du tick d'animation actuel
 * @param[in] input_x Entrée utilisateur horizontale (clavier) pour le fermier
 * @param[in] input_y Entrée utilisateur verticale (clavier) pour le fermier
 * @return Pointeur vers le monde mis à jour
 */
monde *mis_à_jour_monde(monde *monde_courant, int tick_animation, int input_x, int input_y);

/**
 * @brief Sélectionne une action de manière probabiliste selon la méthode Softmax sur un tableau d'intérêts
 * @param[in] tableau_interets Tableau contenant les valeurs d'intérêt pour chaque action possible
 * @param[in] nombre_actions Nombre d'actions possibles (taille du tableau)
 * @return Indice de l'action choisie
 */
int choisir_action_softmax(float *tableau_interets, int nombre_actions);

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
int check_collision_rect(float x_premier, float y_premier, float largeur_premier, float hauteur_premier, float x_second, float y_second, float largeur_second, float hauteur_second);

/**
 * @brief Vérifie si une boîte de collision est en intersection avec l'un des obstacles statiques de la carte (lac, maison)
 * @param[in] hitbox Boîte de collision à vérifier
 * @return 1 s'il y a collision avec un obstacle, 0 sinon
 */
int check_collision_obstacles(Hitbox hitbox);

/**
 * @brief Affiche l'état graphique du monde
 * @param[in] monde_courant Pointeur vers le monde à afficher
 */
void afficher_monde(monde *monde_courant);

// Inclusion des modules qui dépendes des fonctions de monde au début donc on les met à la fin
#include "monde_goat.h"
#include "monde_wolf.h"
#include "monde_fermier.h"

#endif // MONDE_H