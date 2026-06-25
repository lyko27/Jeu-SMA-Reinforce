/**
 * @file monde_goat.h
 * @brief Déclarations des fonctions pour la gestion du monde des chèvres
 * @details Gère l'initialisation, le déplacement, la perception et le cycle de vie des chèvres dans la simulation
 */

#ifndef goat_monde_h
#define goat_monde_h

#include "monde.h"

/**
 * @brief Initialise une chèvre avec ses coordonnées et ses attributs par défaut
 * @param[in,out] goat Pointeur vers la structure de la chèvre à initialiser
 * @param[in] position_x Position horizontale initiale de la chèvre
 * @param[in] position_y Position verticale initiale de la chèvre
 * @return Pointeur vers la chèvre initialisée
 */
Goat *init_goat(Goat *goat, int position_x, int position_y);

/**
 * @brief Ajoute une chèvre dans le tableau des chèvres du monde
 * @param[in,out] monde_courant Pointeur vers le monde de la simulation
 * @param[in] goat Pointeur vers la chèvre à ajouter au monde
 * @return Pointeur vers le monde mis à jour
 */
monde *ajouter_goat(monde *monde_courant, Goat *goat);

/**
 * @brief Calcule ce qu'une chèvre perçoit dans son environnement
 * @param[in] goat Pointeur vers la chèvre concernée
 * @param[in] monde_courant Pointeur vers le monde de la simulation
 * @return Structure contenant la perception de la chèvre
 */
PerceptionGoat calculer_perception_goat(Goat *goat, monde *monde_courant);

/**
 * @brief Met à jour l'état d'une chèvre (déplacement, collisions et animation) pour un tick donné
 * @param[in,out] monde_courant Pointeur vers le monde de la simulation
 * @param[in,out] goat Pointeur vers la chèvre à mettre à jour
 * @param[in] action Action sélectionnée pour la chèvre
 * @param[in] tick_animation Indice du tick pour l'animation
 * @param[in] perception_goat Perception actuelle de cette chèvre
 * @return Pointeur vers la chèvre mise à jour
 */
Goat *update_goat(monde *monde_courant, Goat *goat, ActionGoat action, int tick_animation, PerceptionGoat perception_goat);

/**
 * @brief Gère la mort d'une chèvre en la libérant et en mettant à jour le tableau du monde
 * @param[in,out] monde_courant Pointeur vers le monde de la simulation
 * @param[in] index_goat Indice de la chèvre à supprimer du tableau
 */
void mourrir_goat(monde *monde_courant, int index_goat);

/**
 * @brief Libère la mémoire de toutes les chèvres allouées dans le tableau
 * @param[in,out] tableau_goats Tableau contenant les pointeurs vers les chèvres
 * @param[in] nombre_goats Nombre actuel de chèvres dans le tableau
 */
void free_goats(Goat **tableau_goats, int nombre_goats);

#endif