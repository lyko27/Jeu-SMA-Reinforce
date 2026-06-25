/**
 * @file monde_fermier.h
 * @brief Déclarations pour la gestion de l'agent Fermier dans le monde.
 * @details Prototypes des fonctions d'initialisation, de déplacement, de perception et de mise à jour du fermier.
 */

#ifndef fermier_monde_h
#define fermier_monde_h

#include "monde.h"

/**
 * @brief Initialise le fermier avec ses coordonnées de départ (devant sa maison) et ses paramètres par défaut.
 * @param[in,out] fermier Pointeur vers la structure Fermier à initialiser.
 * @return Fermier* Pointeur vers le fermier initialisé.
 */
Fermier *init_fermier(Fermier *fermier);

/**
 * @brief Ajoute le fermier dans le monde de la simulation.
 * @param[in,out] monde_courant Pointeur vers le monde de la simulation.
 * @param[in] fermier Pointeur vers le fermier à ajouter.
 * @return monde* Le monde mis à jour.
 */
monde *ajouter_fermier(monde *monde_courant, Fermier *fermier);

/**
 * @brief Calcule la perception du fermier à l'instant courant.
 * @details Identifie les distances et directions relatives vers le loup et la chèvre les plus proches.
 * @param[in] fermier Pointeur vers le fermier.
 * @param[in] monde_courant Pointeur vers le monde de la simulation.
 * @return PerceptionFermier La structure de perception résultante.
 */
PerceptionFermier calculer_perception_fermier(Fermier *fermier, monde *monde_courant);

/**
 * @brief Déplace le fermier et gère ses collisions avec les obstacles et les chèvres. (maj fermier)
 * @param[in,out] monde_courant Pointeur vers le monde.
 * @param[in,out] fermier Pointeur vers la structure de l'agent Fermier.
 * @param[in] action Action de mouvement choisie (deltas dx, dy).
 * @param[in] tick_animation Horodatage de la frame courant (utilisé pour les animations).
 * @return Fermier* Pointeur vers le fermier après déplacement.
 */
Fermier *update_fermier(monde *monde_courant, Fermier *fermier, ActionFermier action, int tick_animation);

/**
 * @brief Effectue la mise à jour globale du comportement du fermier (prise de décision RL ou manuelle, puis déplacement). (maj monde pour fermier)
 * @param[in,out] monde_courant Pointeur vers le monde de la simulation.
 * @param[in] tick_animation Horodatage de la frame courant.
 * @param[in] input_x Déplacement horizontal saisi au clavier (mode manuel).
 * @param[in] input_y Déplacement vertical saisi au clavier (mode manuel).
 * @return monde* Le monde mis à jour.
 */
monde *mis_a_jour_fermier(monde *monde_courant, int tick_animation, int input_x, int input_y);

/**
 * @brief Libère la mémoire allouée pour le fermier.
 * @param[in,out] fermier Pointeur vers le fermier à libérer.
 */
void free_fermier(Fermier *fermier);

#endif /* fermier_monde_h */
