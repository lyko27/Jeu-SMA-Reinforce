/**
 * @file reinforce.h
 * @brief Déclarations pour l'algorithme d'apprentissage par renforcement REINFORCE.
 * @details Définit les structures de trajectoire, les prototypes pour la génération des caractéristiques
 *          et la mise à jour des politiques du Fermier et des Loups.
 */

#ifndef REINFORCE_H
#define REINFORCE_H

#include "fermier.h"
#include "loup.h"

struct monde_t;

/**
 * @struct Transition
 * @brief Représente une transition (état, action, récompense) à une étape donnée.
 */
typedef struct {
  float phi[7];       ///< Vecteur de caractéristiques de l'état (taille fixe de 7)
  int action;         ///< Indice de l'action choisie à cette étape
  float recompense;   ///< Récompense immédiate obtenue suite à l'action
} Transition;

/**
 * @struct Trajectoire
 * @brief Collection redimensionnable de transitions formant un épisode.
 */
typedef struct {
  Transition *transitions; ///< Tableau dynamique de transitions
  int taille;              ///< Nombre actuel de transitions enregistrées
  int capacite;            ///< Capacité maximale allouée pour le tableau
} Trajectoire;

/**
 * @brief Initialise une trajectoire vide.
 * @param[in,out] trajectoire Pointeur vers la structure Trajectoire à initialiser.
 */
void init_trajectoire(Trajectoire *trajectoire);

/**
 * @brief Ajoute une transition à une trajectoire en cours.
 * @param[in,out] trajectoire Pointeur vers la structure Trajectoire.
 * @param[in] phi Vecteur de caractéristiques de l'état d'origine.
 * @param[in] action Action effectuée.
 * @param[in] recompense Récompense obtenue.
 */
void ajouter_transition(Trajectoire *trajectoire, float *phi, int action, float recompense);

/**
 * @brief Libère la mémoire associée aux transitions d'une trajectoire.
 * @param[in,out] trajectoire Pointeur vers la structure Trajectoire à libérer.
 */
void liberer_trajectoire(Trajectoire *trajectoire);

/**
 * @brief Génère le vecteur de caractéristiques phi pour le Fermier à partir de sa perception.
 * @param[in] perception Perception actuelle du fermier (positions relatives aux cibles).
 * @param[out] phi Tableau dans lequel stocker le vecteur de caractéristiques généré.
 */
void generer_phi_fermier(PerceptionFermier perception, float *phi);

/**
 * @brief Génère le vecteur de caractéristiques phi pour un Loup à partir de sa perception.
 * @param[in] wolf Pointeur vers le loup concerné.
 * @param[in] perception Perception actuelle du loup (positions relatives aux cibles).
 * @param[out] phi Tableau dans lequel stocker le vecteur de caractéristiques généré.
 */
void generer_phi_wolf(Wolf *wolf, PerceptionWolf perception, float *phi);

/**
 * @brief Met à jour les poids de la politique du Fermier en appliquant l'algorithme REINFORCE.
 * @param[in,out] fermier Pointeur vers l'agent Fermier à mettre à jour.
 * @param[in] trajectoires Tableau de trajectoires enregistrées pendant les épisodes.
 * @param[in] nb_episodes Nombre de trajectoires/épisodes dans le lot.
 * @param[in] alpha Pas d'apprentissage (taux d'apprentissage).
 * @param[in] gamma Facteur d'actualisation des récompenses.
 */
void mise_a_jour_reinforce_fermier(Fermier *fermier, Trajectoire *trajectoires, int nb_episodes, float alpha, float gamma);

/**
 * @brief Met à jour les poids de la politique des Loups en appliquant l'algorithme REINFORCE.
 * @param[in,out] loups Tableau des agents Loups (les poids sont partagés et synchronisés).
 * @param[in] nb_loups Nombre de loups dans la meute.
 * @param[in] trajectoires Tableau de trajectoires enregistrées pour tous les loups.
 * @param[in] nb_episodes Nombre d'épisodes par cycle.
 * @param[in] alpha Pas d'apprentissage (taux d'apprentissage).
 * @param[in] gamma Facteur d'actualisation des récompenses.
 */
void mise_a_jour_reinforce_loups(Wolf **loups, int nb_loups, Trajectoire *trajectoires, int nb_episodes, float alpha, float gamma);

/**
 * @brief Sauvegarde les poids du Fermier dans un fichier texte.
 * @param[in] fermier Pointeur vers l'agent Fermier.
 * @param[in] filepath Chemin du fichier cible.
 * @return int 1 si succès, 0 sinon.
 */
int sauvegarder_poids_fermier(Fermier *fermier, char *filepath);

/**
 * @brief Charge les poids du Fermier depuis un fichier texte.
 * @param[in,out] fermier Pointeur vers l'agent Fermier.
 * @param[in] filepath Chemin du fichier source.
 * @return int 1 si succès, 0 sinon.
 */
int charger_poids_fermier(Fermier *fermier, char *filepath);

/**
 * @brief Sauvegarde les poids d'un Loup dans un fichier texte.
 * @param[in] wolf Pointeur vers l'agent Loup.
 * @param[in] filepath Chemin du fichier cible.
 * @return int 1 si succès, 0 sinon.
 */
int sauvegarder_poids_loup(Wolf *wolf, char *filepath);

/**
 * @brief Charge les poids d'un Loup depuis un fichier texte.
 * @param[in,out] wolf Pointeur vers l'agent Loup.
 * @param[in] filepath Chemin du fichier source.
 * @return int 1 si succès, 0 sinon.
 */
int charger_poids_loup(Wolf *wolf, char *filepath);

#endif
