/**
 * @file loup.h
 * @brief Déclarations des structures et fonctions de l'agent Loup.
 * @details Gère la définition de l'état, de la perception et des actions possibles pour le loup.
 */

#ifndef wolf_h
#define wolf_h

#include "goat.h"

#define DIMENSION_PHI_WOLF 7

/**
 * @enum ActionWolf
 * @brief Les actions possibles pour un loup dans la simulation.
 */
typedef enum
{
    ACTION_WOLF_ERRER,        ///< Se déplacer de façon aléatoire
    ACTION_WOLF_CHASSER,      ///< Chasser la chèvre la plus proche
    ACTION_WOLF_ARRET,        ///< Rester immobile
    ACTION_WOLF_FUIR_FERMIER, ///< Fuir dans la direction opposée au fermier
    NB_ACTIONS_WOLF           ///< Nombre total d'actions possibles (4)
} ActionWolf;

/**
 * @struct PerceptionWolf
 * @brief Structure décrivant ce que le loup perçoit
 */
typedef struct
{
    Goat **goats_tab;         ///< Tableau de pointeurs vers toutes les chèvres
    int nb_goat;              ///< Nombre total de chèvres dans le moonde
    float dist_goat_proche;   ///< Distance euclidienne vers la chèvre la plus proche
    int pos_x_goat;           ///< pos x de la chèvre la plus proche (-1 si aucune)
    int pos_y_goat;           ///< pos y de la chèvre la plus proche (-1 si aucune)
    int pos_x_fermier;        ///< pos x du fermier
    int pos_y_fermier;        ///< pos y du fermier
} PerceptionWolf;

/**
 * @struct Wolf
 * @brief Structure représentant l'agent Loup
 */
typedef struct
{
    // pos et mvt
    float x;             ///< pos x sur la carte
    float y;             ///< pos y sur la carte
    float speed;         ///< Vitesse de déplacement
    float dir_x;         ///< direction visée en x
    float dir_y;         ///< direction visée en y
    float angle_actuel;  ///< Angle de déplacement actuel (en radians)

    // Animation
    int frame;            ///< Frame d'animation actuelle pour le sprite
    int en_mouvement;     ///< Indicateur de mouvement (1 si en mouvement, 0 sinon)
    int direction_sprite; ///< Direction du sprite pour l'affichage (1 haut, 2 droite, 3 bas, 4 gauche)
    int timer_mouvement;  ///< Temps de déplacement restant dans la direction actuelle

    // Vie et décision
    int hp;                                            ///< Points de vie restants du loup
    int cooldown_dinvisibilite;                        ///< Temps restant d'invisibilité après avoir subi une attaque
    float table_interets[NB_ACTIONS_WOLF];             ///< Tableau des intérêts calculés pour chaque action possible
    float weights[NB_ACTIONS_WOLF][DIMENSION_PHI_WOLF]; ///< Matrice de poids de la politique pour l'apprentissage par renforcement
    int decision_cooldown;                             ///< Temps restant (en ticks) avant de prendre une décision
    ActionWolf action_choisi;                          ///< Action sélectionnée lors de la dernière décision
} Wolf;

/**
 * @brief Évalue les intérêts des différentes actions d'un loup en utilisant des règles heuristiques simples.
 * @param[in,out] wolf Pointeur vers la structure du loup à modifier.
 * @param[in] perception_wolf Perception contenant les positions des cibles (fermier et chèvres).
 */
void evaluer_interets_wolf(Wolf *wolf, PerceptionWolf perception_wolf);

/**
 * @brief Évalue les intérêts des différentes actions d'un loup à l'aide de sa politique apprise (mode RL).
 * @param[in,out] wolf Pointeur vers la structure du loup à modifier.
 * @param[in] perception_wolf Perception contenant les positions des cibles (fermier et chèvres).
 */
void evaluer_interets_wolf_rl(Wolf *wolf, PerceptionWolf perception_wolf);

#endif /* wolf_h */
