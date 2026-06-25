/**
 * @file fermier.h
 * @brief Déclarations des structures et fonctions de l'agent Fermier.
 * @details Gère l'état du fermier, ses structures de données, sa perception et les prototypes de fonctions associées.
 */

#ifndef fermier_h
#define fermier_h

#define DIMENSION_PHI_FERMIER 11

/**
 * @struct ActionFermier
 * @brief Représente un vecteur de déplacement élémentaire pour le fermier.
 */
typedef struct
{
    int dx; ///< Déplacement sur l'axe x
    int dy; ///< Déplacement sur l'axe y
} ActionFermier;

/**
 * @enum ActionFermierType
 * @brief les 8 actions de déplacement possibles pour le fermier dans REINFORCE.
 */
typedef enum
{
    ACTION_FERMIER_AVANCER,    
    ACTION_FERMIER_RECULER,     
    ACTION_FERMIER_DROITE,        
    ACTION_FERMIER_GAUCHE,       
    ACTION_FERMIER_HAUT_GAUCHE,   
    ACTION_FERMIER_HAUT_DROITE,   
    ACTION_FERMIER_BAS_GAUCHE,    
    ACTION_FERMIER_BAS_DROITE,    
    NB_ACTIONS_FERMIER            ///< Nombre total d'actions possibles (8)
} ActionFermierType;

/**
 * @struct PerceptionFermier
 * @brief Structure décrivant ce que le fermier perçoit dans l'environement.
 */
typedef struct
{
    int input_x;     ///< Déplacement x du clavier de l'utilisateur
    int input_y;     ///< Déplacement y du clavier de l'utilisateur
    float dist_wolf; ///< Distance euclidienne vers le loup le plus proche
    float dx_wolf;   ///< coordonée x de la direction vers le loup le plus proche
    float dy_wolf;   ///< coordonée y de la direction vers le loup le plus proche
    float dist_goat; ///< Distance euclidienne vers la chèvre la plus proche
    float dx_goat;   ///< coordonée x de la direction vers la chèvre la plus proche
    float dy_goat;   ///< coordonée y de la direction vers la chèvre la plus proche
    float dist_mur_gauche; ///< Distance au mur gauche
    float dist_mur_droite; ///< Distance au mur droit
    float dist_mur_haut;   ///< Distance au mur du haut
    float dist_mur_bas;    ///< Distance au mur du bas
} PerceptionFermier;

/**
 * @struct Fermier
 * @brief Structure représentant l'agent Fermier dans la simulation.
 */
typedef struct
{
    float x;                                                  ///< position x actuelle sur la carte
    float y;                                                  ///< position y actuelle sur la carte
    float speed;                                              ///< vitesse de déplacement
    float dir_x;                                              ///< direction en x de où il veut aller
    float dir_y;                                              ///< direction en y de où il veut aller
    int frame;                                                ///< Frame d'animation actuelle pour le sprite
    int direction_sprite;                                     ///< Direction du sprite pour l'affichage (1 haut, 2 droite, 3 bas, 4 gauche)
    float table_interets[NB_ACTIONS_FERMIER];                 ///< tableau des intérêts calculés pour chaque action possible
    float weights[NB_ACTIONS_FERMIER][DIMENSION_PHI_FERMIER]; ///< Matrice de poids de la politique pour l'apprentissage par renforcement (theta)
    int decision_cooldown;                                    ///< Temps restant avant de pouvoir prendre la prochaine décision
    ActionFermierType action_choisi;                          ///< Type d'action choisi lors de la dernière décision (mode RL)
    ActionFermier dirrection_choisi;                          ///< Action de déplacement contenant les deltas dx/dy (mode manuel)
    int action_id;                                            ///< id de l'action sélectionnée (pour les transitions d'entraînement)
} Fermier;

/**
 * @brief Décide de l'action de déplacement manuel du fermier à partir des entrées utilisateur.
 * @details Cette fonction est appelée en mode manuel. Elle récupère les inputs de direction (x et y)
 *          depuis la perception et les stockes comme la direction choisie par le fermier.
 * @param[in,out] fermier la structure de l'agent Fermier à modifier.
 * @param[in] perception_fermier Perception contenant les entrées utilisateur (clavier).
 * @return ActionFermier L'action de déplacement calculée dx et dy
 */
ActionFermier decider_action_fermier(Fermier *fermier, PerceptionFermier perception_fermier);

/**
 * @brief Évalue les intérêts de chaque action possible du fermier à l'aide de sa politique apprise.
 * @details Cette fonction calcule la valeur d'intérêt (logit) pour chacune des 8 actions de mouvement.
 *          Elle génère d'abord le vecteur de caractéristiques d'état (phi), puis effectue un produit
 *          scalaire entre ce vecteur et la matrice des poids du fermier pour chaque action. Les résultats
 *          sont stockés dans la table d'intérêts pour le tirage de la décision via Softmax.
 * @param[in,out] fermier la structure du Fermier dont on met à jour la table d'intérêts.
 * @param[in] perception_fermier Perception du fermier (positions du loup et de la chèvre la plus proche).
 */
void evaluer_interets_fermier(Fermier *fermier, PerceptionFermier perception_fermier);

#endif /* fermier_h */
