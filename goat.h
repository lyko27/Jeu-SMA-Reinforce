/**
 * @file goat.h
 * @brief Déclarations des structures et types de l'agent chèvre
 * @details Gère la définition de l'état, de la perception et des actions possibles pour la chèvre
 */

#ifndef goat_h
#define goat_h

/**
 * @enum ActionGoat
 * @brief Actions possibles pour une chèvre
 */
typedef enum
{
    ACTION_FUIR_WOLF, ///< Fuir dans la direction opposée au loup le plus proche (angle oposé).
    ACTION_ERRER,     ///< Se déplacer de façon aléatoire sur la carte.
    ACTION_BROUTER,   ///< Rester sur place pour brouter l'herbe (arrêt).
    NB_ACTIONS        ///< Nombre total d'actions disponibles pour la chèvre.
} ActionGoat;

/**
 * @struct Goat
 * @brief Structure représentant l'agent Chèvre dans la simulation
 */
typedef struct
{
    // var de position
    float x;             ///< Position x actuelle de la chèvre.
    float y;             ///< Position y actuelle de la chèvre.
    float speed;         ///< Vitesse de déplacement.
    float dir_x;         ///< Coordonnée direction en X.
    float dir_y;         ///< Coordonnée de direction en Y.
    float angle_actuel;  ///< Angle de déplacement actuel (en radians).

    // var d'animation
    int frame;            ///< Frame d'animation actuelle pour le sprite.
    int direction_sprite; ///< Direction du sprite (1 haut, 2 droite, 3 bas, 4 gauche).
    int en_mouvement;     ///< Indicateur d'état de mouvement (1 si bouge, 0 sinon).
    int timer_mouvement;  ///< Temps de déplacement restant dans la direction actuelle.

    // decision / vie
    int hp;                      ///< Points de vie restants de la chèvre.
    int cooldown_dinvisibilite;  ///< Temps restant d'invisibilité après une attaque.

    float table_interets[NB_ACTIONS]; ///< Tableau des valeurs d'intérêts pour chaque action.
    int decision_cooldown;            ///< Temps restant avant la prochaine prise de décision.
    ActionGoat action_choisi;         ///< Action sélectionnée lors de la dernière décision.
} Goat;

/**
 * @struct PerceptionGoat
 * @brief Structure de ce que qu'une chèvre perçoit
 */
typedef struct
{
    float dist_wolf_proche; ///< Distance euclidienne vers le loup le plus proche.
    int pos_x_wolf;         ///< Position x loup le plus proche.
    int pos_y_wolf;         ///< Position y loup le plus proche.
    Goat **goats_tab;       ///< Pointeur vers le tableau de toutes les chèvres.
    int capacite_max_goat;  ///< Capacité maximale d'allocation du tableau de chèvres.
    int nb_goat;            ///< Nombre total de chèvres en vie.
    int pos_x_fermier;      ///< Position x du fermier.
    int pos_y_fermier;      ///< Position y du fermier.
} PerceptionGoat;

/**
 * @brief Évalue les intérêts des différentes actions pour une chèvre donnée en fonction de sa perception.
 * @param[in,out] goat Pointeur vers la structure de la chèvre.
 * @param[in] perception_goat Perception vu par cette chèvre.
 */
void evaluer_interets_goat(Goat *goat, PerceptionGoat perception_goat);

#endif /* goat_h */
