/**
 * @file reinforce.c
 * @brief Implémentation de l'algorithme d'apprentissage par renforcement REINFORCE.
 * @details Gère les trajectoires, le calcul de la politique softmax, la génération des vecteurs de caractéristiques phi, 
 * et les mises à jour des poids.
 * @author Nicolas
 */

#include "reinforce.h"
#include "monde.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Initialise une trajectoire vide.
 * @param[in,out] trajectoire Pointeur vers la structure Trajectoire à initialiser.
 */
void init_trajectoire(Trajectoire *trajectoire)
{
  trajectoire->taille = 0;
  trajectoire->capacite = 100;
  trajectoire->transitions = malloc(trajectoire->capacite * sizeof(Transition));
}

/**
 * @brief Ajoute une transition à une trajectoire en cours.
 * @param[in,out] trajectoire Pointeur vers la structure Trajectoire
 * @param[in] phi Vecteur de caractéristiques de l'état d'origine
 * @param[in] action Action effectuée
 * @param[in] recompense Récompense obtenue
 */
void ajouter_transition(Trajectoire *trajectoire, float *phi, int action, float recompense)
{
  if (trajectoire->taille == trajectoire->capacite)
  {
    trajectoire->capacite *= 2;
    trajectoire->transitions = realloc(trajectoire->transitions, trajectoire->capacite * sizeof(Transition));
  }
  memcpy(trajectoire->transitions[trajectoire->taille].phi, phi, 7 * sizeof(float));
  trajectoire->transitions[trajectoire->taille].action = action;
  trajectoire->transitions[trajectoire->taille].recompense = recompense;
  trajectoire->taille++;
}

/**
 * @brief Libère la mémoire associée aux transitions d'une trajectoire
 * @param[in,out] trajectoire Pointeur vers la structure Trajectoire à libérer
 */
void liberer_trajectoire(Trajectoire *trajectoire)
{
  if (trajectoire->transitions)
  {
    free(trajectoire->transitions);
    trajectoire->transitions = NULL;
  }
  trajectoire->taille = 0;
  trajectoire->capacite = 0;
}

/**
 * @brief Calcule la distribution de probabilité Softmax sur les actions
 * @details Applique la formule softmax sur les logits calculés par le produit scalaire ( weights * phi )
 * soustrait le logit maximum pour assurer la stabilité
 * @param[in] phi Vecteur de caractéristiques
 * @param[in] poids Matrice des poids (weights) pour chaque action
 * @param[in] nb_actions Nombre d'actions possibles
 * @param[in] dim_phi Dimension du vecteur phi (nombre de caractéristiques)
 * @param[out] probabilites Tableau de sortie dans lequel stocker les probabilités calculées
 */
void calculer_softmax(const float *phi, float poids[][7], int nb_actions, int dim_phi, float *probabilites)
{
  float logits[nb_actions];
  float max_logit = -999999.0f;

  // Calcul des logits pour chaque action
  for (int a = 0; a < nb_actions; a++)
  {
    float val = 0.0f;
    for (int k = 0; k < dim_phi; k++)
    {
      val += poids[a][k] * phi[k];
    }
    logits[a] = val;
    if (val > max_logit)
    {
      max_logit = val;
    }
  }

  // exponentiel et somme des probabilités 
  float somme_exp = 0.0f;
  for (int a = 0; a < nb_actions; a++)
  {
    probabilites[a] = expf(logits[a] - max_logit);
    somme_exp += probabilites[a];
  }
  
  // Normalisation pour obtenir une distribution de probabilité cohérente
  for (int a = 0; a < nb_actions; a++)
  {
    probabilites[a] /= somme_exp;
  }
}


// ..................... Reinforce pour le fermier

/**
 * @brief Génère le vecteur de caractéristiques phi pour le Fermier à partir de sa perception
 * @param[in] perception Perception du fermier
 * @param[out] phi Tableau où on stock phi.
 */
void generer_phi_fermier(PerceptionFermier perception, float *phi)
{
  phi[0] = 1.0f; // Biais

  // Distance et direction du loup le plus proche
  if (perception.dist_wolf > 0.001f && perception.dist_wolf < 99999.0f)
  {
    phi[1] = perception.dist_wolf / 1024.0f;            // distance normalisée
    phi[2] = perception.dx_wolf / perception.dist_wolf; // direction x
    phi[3] = perception.dy_wolf / perception.dist_wolf; // direction y
  }
  else
  {
    phi[1] = 1.0f;
    phi[2] = 0.0f;
    phi[3] = 0.0f;
  }

  // Distance et direction de la chèvre la plus proche
  if (perception.dist_goat > 0.001f && perception.dist_goat < 99999.0f)
  {
    phi[4] = perception.dist_goat / 1024.0f;            // distance normalisée
    phi[5] = perception.dx_goat / perception.dist_goat; // direction x
    phi[6] = perception.dy_goat / perception.dist_goat; // direction y
  }
  else
  {
    phi[4] = 1.0f;
    phi[5] = 0.0f;
    phi[6] = 0.0f;
  }
}

/**
 * @brief Met à jour les poids de la politique du Fermier en appliquant l'algorithme REINFORCE
 * @param[in,out] fermier Pointeur vers l'agent Fermier à mettre à jour
 * @param[in] trajectoires Tableau de trajectoires enregistrées pendant les épisodes
 * @param[in] nb_episodes Nombre d'épisodes dans le lot
 * @param[in] alpha le pas d'apprentissage (taux d'apprentissage)
 * @param[in] gamma Facteur d'actualisation des récompenses
 */
void mise_a_jour_reinforce_fermier(Fermier *fermier, Trajectoire *trajectoires, int nb_episodes, float alpha, float gamma)
{
  float D[NB_ACTIONS_FERMIER][DIMENSION_PHI_FERMIER];

  // Initialisation du gradient accumulé D à 0
  for (int i = 0; i < NB_ACTIONS_FERMIER; i++)
  {
    for (int j = 0; j < DIMENSION_PHI_FERMIER; j++)
    {
      D[i][j] = 0.0f;
    }
  }

  for (int i = 0; i < nb_episodes; i++)
  {
    Trajectoire *trajectoire = &trajectoires[i];
    int T = trajectoire->taille;
    float G = 0.0f;

    // Parcours de la trajectoire en remontant le temps (de T-1 à 0) pour calculer les retours actualisés G_t
    for (int u = 0; u < T; u++)
    {
      int t = T - 1 - u;
      Transition transition = trajectoire->transitions[t];
      G = transition.recompense + G * gamma;
      float GG = powf(gamma, t) * G;

      float P[NB_ACTIONS_FERMIER];
      calculer_softmax(transition.phi, fermier->weights, NB_ACTIONS_FERMIER, DIMENSION_PHI_FERMIER, P);

      // Accumulation des gradients,  modif = phi * (I(a == action_choisie) - P(a))
      for (int a = 0; a < NB_ACTIONS_FERMIER; a++)
      {
        for (int k = 0; k < DIMENSION_PHI_FERMIER; k++)
        {
          float modif = 0.0f;
          if (a == transition.action)
          {
            modif = transition.phi[k];
          }
          modif -= P[a] * transition.phi[k];
          D[a][k] += GG * modif;
        }
      }
    }
  }

  // Mis à jour des poids du fermier (weights += alpha * (1 / N) * D)
  for (int a = 0; a < NB_ACTIONS_FERMIER; a++)
  {
    for (int k = 0; k < DIMENSION_PHI_FERMIER; k++)
    {
      fermier->weights[a][k] += alpha * (1.0f / nb_episodes) * D[a][k];
    }
  }
}

/**
 * @brief Sauvegarde les poids du Fermier dans un fichier texte
 * @param[in] fermier Pointeur vers l'agent Fermier
 * @param[in] filepath Chemin du fichier cible
 * @return int 1 si succès, 0 sinon
 */
int sauvegarder_poids_fermier(Fermier *fermier, char *filepath)
{
  FILE *file = fopen(filepath, "w");
  if (!file)
    return 0;
  for (int a = 0; a < NB_ACTIONS_FERMIER; a++)
  {
    for (int k = 0; k < DIMENSION_PHI_FERMIER; k++)
    {
      fprintf(file, "%f ", fermier->weights[a][k]);
    }
    fprintf(file, "\n");
  }
  fclose(file);
  return 1;
}

/**
 * @brief Charge les poids du Fermier depuis un fichier texte
 * @param[in,out] fermier Pointeur vers l'agent Fermier.
 * @param[in] filepath Chemin du fichier source
 * @return int 1 si succès, 0 sinon
 */
int charger_poids_fermier(Fermier *fermier, char *filepath)
{
  FILE *file = fopen(filepath, "r");
  if (!file)
    return 0;
  for (int a = 0; a < NB_ACTIONS_FERMIER; a++)
  {
    for (int k = 0; k < DIMENSION_PHI_FERMIER; k++)
    {
      if (fscanf(file, "%f", &fermier->weights[a][k]) != 1)
      {
        fclose(file);
        return 0;
      }
    }
  }
  fclose(file);
  return 1;
}






// .................. Reinforce pour le loup





/**
 * @brief Génère le vecteur de caractéristiques phi pour un Loup à partir de sa perception
 * @param[in] wolf Pointeur vers le loup 
 * @param[in] perception Perception du loup
 * @param[out] phi Tableau où on stocke phi
 */
void generer_phi_wolf(Wolf *wolf, PerceptionWolf perception, float *phi)
{
  phi[0] = 1.0f; // Biais

  // Distance et direction de la chèvre la plus proche
  if (perception.pos_x_goat != -1 && perception.dist_goat_proche > 0.001f && perception.dist_goat_proche < 99999.0f)
  {
    phi[1] = perception.dist_goat_proche / 1024.0f; // distance normalisée
    float dx_goat = perception.pos_x_goat - wolf->x;
    float dy_goat = perception.pos_y_goat - wolf->y;
    phi[2] = dx_goat / perception.dist_goat_proche; // direction x
    phi[3] = dy_goat / perception.dist_goat_proche; // direction y
  }
  else
  {
    phi[1] = 1.0f;
    phi[2] = 0.0f;
    phi[3] = 0.0f;
  }

  // Distance et direction du fermier
  float dx_fermier = perception.pos_x_fermier - wolf->x;
  float dy_fermier = perception.pos_y_fermier - wolf->y;
  float dist_fermier = sqrtf(dx_fermier * dx_fermier + dy_fermier * dy_fermier);
  if (dist_fermier > 0.001f && dist_fermier < 99999.0f)
  {
    phi[4] = dist_fermier / 1024.0f; // distance normalisée
    phi[5] = dx_fermier / dist_fermier; // direction x
    phi[6] = dy_fermier / dist_fermier; // direction y
  }
  else
  {
    phi[4] = 1.0f;
    phi[5] = 0.0f;
    phi[6] = 0.0f;
  }
}




/**
 * @brief Met à jour les poids de la politique des Loups en appliquant l'algorithme REINFORCE
 * @param[in,out] loups Tableau des agents Loups (les poids sont partagés et synchronisés)
 * @param[in] nb_loups Nombre de loups dans la meute
 * @param[in] trajectoires Tableau de trajectoires enregistrées pour tous les loups
 * @param[in] nb_episodes Nombre d'épisodes par cycle
 * @param[in] alpha le pas d'apprentissage (taux d'apprentissage)
 * @param[in] gamma Facteur d'actualisation des récompenses
 */
void mise_a_jour_reinforce_loups(Wolf **loups, int nb_loups, Trajectoire *trajectoires, int nb_episodes, float alpha, float gamma)
{
  float D[NB_ACTIONS_WOLF][DIMENSION_PHI_WOLF];

  // Initialisation du gradient accumulé D à 0
  for (int i = 0; i < NB_ACTIONS_WOLF; i++)
  {
    for (int j = 0; j < DIMENSION_PHI_WOLF; j++)
    {
      D[i][j] = 0.0f;
    }
  }

  int total_trajectoires = nb_episodes * nb_loups;

  // Boucle sur toutes les trajectoires enregistrées pour la meute
  for (int i = 0; i < total_trajectoires; i++)
  {
    Trajectoire *trajectoire = &trajectoires[i];
    int T = trajectoire->taille;
    float G = 0.0f;

    // Parcours de la trajectoire en remontant le temps pour calculer G_t
    for (int u = 0; u < T; u++)
    {
      int t = T - 1 - u;
      Transition transition = trajectoire->transitions[t];
      G = transition.recompense + G * gamma;
      float GG = powf(gamma, t) * G;

      float P[NB_ACTIONS_WOLF];
      // On utilise les poids du premier loup (ils sont partagés et identiques)
      calculer_softmax(transition.phi, loups[0]->weights, NB_ACTIONS_WOLF, DIMENSION_PHI_WOLF, P);

      // Accumulation des gradients
      for (int a = 0; a < NB_ACTIONS_WOLF; a++)
      {
        for (int k = 0; k < DIMENSION_PHI_WOLF; k++)
        {
          float modif = 0.0f;
          if (a == transition.action)
          {
            modif = transition.phi[k];
          }
          modif -= P[a] * transition.phi[k];
          D[a][k] += GG * modif;
        }
      }
    }
  }

  // Appliquer la mise à jour à TOUS les loups pour qu'ils restent synchronisés
  for (int index_loup = 0; index_loup < nb_loups; index_loup++)
  {
    for (int a = 0; a < NB_ACTIONS_WOLF; a++)
    {
      for (int k = 0; k < DIMENSION_PHI_WOLF; k++)
      {
        loups[index_loup]->weights[a][k] += alpha * (1.0f / total_trajectoires) * D[a][k];
      }
    }
  }
}

/**
 * @brief Sauvegarde les poids d'un Loup dans un fichier texte.
 * @param[in] wolf Pointeur vers l'agent Loup
 * @param[in] filepath Chemin du fichier cible
 * @return int 1 si succès, 0 sinon
 */
int sauvegarder_poids_loup(Wolf *wolf, char *filepath)
{
  FILE *file = fopen(filepath, "w");
  if (!file)
    return 0;
  for (int a = 0; a < NB_ACTIONS_WOLF; a++)
  {
    for (int k = 0; k < DIMENSION_PHI_WOLF; k++)
    {
      fprintf(file, "%f ", wolf->weights[a][k]);
    }
    fprintf(file, "\n");
  }
  fclose(file);
  return 1;
}

/**
 * @brief Charge les poids d'un Loup depuis un fichier texte
 * @param[in,out] wolf Pointeur vers l'agent Loup
 * @param[in] filepath Chemin du fichier source
 * @return int 1 si succès, 0 sinon
 */
int charger_poids_loup(Wolf *wolf, char *filepath)
{
  FILE *file = fopen(filepath, "r");
  if (!file)
    return 0;
  for (int a = 0; a < NB_ACTIONS_WOLF; a++)
  {
    for (int k = 0; k < DIMENSION_PHI_WOLF; k++)
    {
      if (fscanf(file, "%f", &wolf->weights[a][k]) != 1)
      {
        fclose(file);
        return 0;
      }
    }
  }
  fclose(file);
  return 1;
}
