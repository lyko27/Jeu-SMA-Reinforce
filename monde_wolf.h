/**
 * @file monde_wolf.h
 * @brief Déclarations des fonctions pour la gestion du monde des loups
 * @details Gère l'initialisation, le déplacement, la perception et le cycle de vie des loups dans la simulation
 */

#ifndef wolf_monde_h
#define wolf_monde_h

#include "monde.h"

/**
 * @brief Initialise un wolf avec des coordonnées de spawn valides (hors obstacles) et des attributs par défaut
 * @param[in,out] wolf Pointeur vers la structure du wolf à initialiser
 * @return Pointeur vers le wolf initialisé
 */
Wolf *init_wolf(Wolf *wolf);

/**
 * @brief Ajoute un wolf dans le tableau des wolfs du monde
 * @param[in,out] monde_courant Pointeur vers le monde de la simulation
 * @param[in] wolf Pointeur vers le wolf à ajouter au monde
 * @return Pointeur vers le monde mis à jour
 */
monde *ajouter_wolf(monde *monde_courant, Wolf *wolf);

/**
 * @brief Calcule la perception d'un wolf
 * @param[in] wolf Pointeur vers le wolf concerné
 * @param[in] monde_courant Pointeur vers le monde de la simulation
 * @return Structure contenant la perception calculée pour le wolf
 */
PerceptionWolf calculer_perception_wolf(Wolf *wolf, monde *monde_courant);

/**
 * @brief Met à jour l'état d'un wolf (déplacement, collisions et animation) pour un tick donné
 * @details Gère la logique et le comportement, exploration, poursuite d'une chèvre ou fuite face au fermier
 * Pour la poursuite (ACTION_WOLF_CHASSER), Calcul de l'angle à l'aide de la fonction trigonométrique atan2 :
 * \code wolf->angle_actuel = atan2(y_destination - y_actuel, x_destination - x_actuel); \endcode
 * Cette fonction calcule le vecteur qui va du loup vers la cible (la goat la plus proche)
 * et renvoie l'angle exact (en radians) par rapport à l'axe des abscisses positive
 * que le loup doit suivre pour aller vers la chèvre (source : )
 * 
 * @param[in,out] monde_courant Pointeur vers le monde de la simulation
 * @param[in,out] wolf Pointeur vers le loup à mettre à jour
 * @param[in] action Action sélectionnée pour le loup
 * @param[in] tick_animation Indice du tick pour l'animation
 * @param[in] perception_wolf Perception actuelle du loup
 * @return Pointeur vers le wolf mis à jour
 */
Wolf *update_wolf(monde *monde_courant, Wolf *wolf, ActionWolf action, int tick_animation, PerceptionWolf perception_wolf);

/**
 * @brief Gère la mort d'un wolf en le libérant et en mettant à jour le tableau du monde
 * @param[in,out] monde_courant Pointeur vers le monde de la simulation
 * @param[in] index_wolf Indice du wolf à supprimer du tableau
 */
void mourrir_wolf(monde *monde_courant, int index_wolf);

/**
 * @brief Libère la mémoire de tous les wolfs alloués dans le tableau
 * @param[in,out] tableau_wolfs Tableau contenant les pointeurs vers les wolfs
 * @param[in] nombre_wolfs Nombre de wolfs dans le tableau
 */
void free_wolf(Wolf **tableau_wolfs, int nombre_wolfs);

#endif