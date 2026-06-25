/**
 * @file monde_fermier.c
 * @brief Implémentation de la gestion de l'agent Fermier dans le monde et la simulation
 * @details Contient la logique d'initialisation, de déplacement physique avec gestion des obstacles,
 * de calcul de perception locale et de boucle globale de décision
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "monde_fermier.h"
#include "reinforce.h"

/* ========== Fermier ========== */

/**
 * @brief Initialise le fermier avec ses coordonnées de départ (devant sa maison) et ses paramètres par défaut
 * @param[in,out] fermier Pointeur vers la structure Fermier à initialiser
 * @return Fermier* Pointeur vers le fermier initialisé
 */
Fermier *init_fermier(Fermier *fermier)
{
  fermier->x = 842;
  fermier->y = 242;
  fermier->speed = VITESSE_FERMIER;
  fermier->decision_cooldown = 0;
  for (int a = 0; a < NB_ACTIONS_FERMIER; a++)
  {
    for (int k = 0; k < DIMENSION_PHI_FERMIER; k++)
    {
      fermier->weights[a][k] = 0.0f;
    }
  }
  return fermier;
}

/**
 * @brief Ajoute le fermier dans le monde de la simulation
 * @param[in,out] monde_courant Pointeur vers le monde de la simulation
 * @param[in] fermier Pointeur vers le fermier à ajouter
 * @return monde* Le monde mis à jour
 */
monde *ajouter_fermier(monde *monde_courant, Fermier *fermier)
{
  monde_courant->fermiers = fermier;
  return monde_courant;
}

/**
 * @brief Calcule la perception du fermier
 * @details Identifie les distances et positions du loup et la chèvre les plus proche
 * @param[in] fermier Pointeur vers le fermier
 * @param[in] monde_courant Pointeur vers le monde de la simulation.
 * @return PerceptionFermier La structure de perception résultante.
 */
PerceptionFermier calculer_perception_fermier(Fermier *fermier, monde *monde_courant)
{
  PerceptionFermier perception;
  perception.input_x = 0;
  perception.input_y = 0;
  perception.dist_wolf = 2000.0f; // Valeur par défaut très loin pour que n'importe quel loup soit plus proche
  perception.dx_wolf = 0.0f;
  perception.dy_wolf = 0.0f;
  perception.dist_goat = 2000.0f; // Valeur par défaut très loin pour que n'importe quel chèvre soit plus proche
  perception.dx_goat = 0.0f;
  perception.dy_goat = 0.0f;

  // calcul du loup le plus proche
  for (int i = 0; i < monde_courant->nb_wolf; i++)
  {
    Wolf *wolf = monde_courant->wolfs_tab[i];
    if (wolf)
    {
      float dx = wolf->x - fermier->x;
      float dy = wolf->y - fermier->y;
      float distance = sqrtf(dx * dx + dy * dy);
      if (distance < perception.dist_wolf)
      {
        perception.dist_wolf = distance;
        perception.dx_wolf = dx;
        perception.dy_wolf = dy;
      }
    }
  }

  // calcul de la chèvre la plus proche
  for (int i = 0; i < monde_courant->nb_goat; i++)
  {
    Goat *goat = monde_courant->goats_tab[i];
    if (goat)
    {
      float dx = goat->x - fermier->x;
      float dy = goat->y - fermier->y;
      float distance = sqrtf(dx * dx + dy * dy);
      if (distance < perception.dist_goat)
      {
        perception.dist_goat = distance;
        perception.dx_goat = dx;
        perception.dy_goat = dy;
      }
    }
  }

  return perception;
}

/**
 * @brief Déplace le fermier et gère ses collisions avec les obstacles et les chèvres, on met a jour le fermier pour le moteur du jeu
 * @param[in,out] monde_courant Pointeur vers le monde
 * @param[in,out] fermier_actuel Pointeur vers la structure de l'agent Fermier
 * @param[in] action_fermier Action de mouvement choisie pos_x et pos_y
 * @param[in] tick_animation Horodatage/index de frame courant (utilisé pour les animations)
 * @return Fermier* Pointeur vers le fermier après déplacement
 */
Fermier *update_fermier(monde *monde_courant, Fermier *fermier_actuel, ActionFermier action_fermier, int tick_animation)
{

  float next_fermier_x = fermier_actuel->x;
  float next_fermier_y = fermier_actuel->y;

  float vitesse_actuelle = fermier_actuel->speed;
  
  // Normalisation de la vitesse en diagonale
  if (action_fermier.dx != 0 && action_fermier.dy != 0)
  {
    vitesse_actuelle /= sqrt(2);
  }

  // Application des déplacements selon la direction
  if (action_fermier.dy == 1)
  {
    next_fermier_y -= vitesse_actuelle;
    fermier_actuel->direction_sprite = 1;
  }
  if (action_fermier.dy == -1)
  {
    next_fermier_y += vitesse_actuelle;
    fermier_actuel->direction_sprite = 3;
  }
  if (action_fermier.dx == 1)
  {
    next_fermier_x -= vitesse_actuelle;
    fermier_actuel->direction_sprite = 4;
  }
  if (action_fermier.dx == -1)
  {
    next_fermier_x += vitesse_actuelle;
    fermier_actuel->direction_sprite = 2;
  }

  // Gestion du repos ou du mouvement (face à nous quand le fermier ne bouge pas)
  if (action_fermier.dx == 0 && action_fermier.dy == 0)
  {
    fermier_actuel->frame = 5;
    fermier_actuel->direction_sprite = 0;
  }
  else
  {
    // ...... on se déplace et on glisse sur l'axe x si besoin

    float pos_potentielle_x = next_fermier_x;
    
    // bord de la map en x
    if (pos_potentielle_x < MARGE) pos_potentielle_x = MARGE;
    if (pos_potentielle_x > LARGEUR - MARGE - WIDTH_FERMIER) pos_potentielle_x = LARGEUR - MARGE - WIDTH_FERMIER;

    int collision_x = 0;
    Hitbox hitbox_pos_potentielle_x = get_hitbox_fermier(pos_potentielle_x, fermier_actuel->y);

    // Collision axe x avec le terrain (lac, maison)
    if (check_collision_obstacles(hitbox_pos_potentielle_x))
    {
      collision_x = 1;
    }

    // Collision de l'axe x avec les chèvres
    if (!collision_x)
    {
      for (int j = 0; j < monde_courant->nb_goat; j++)
      {
        Hitbox hb_goat = get_hitbox_goat(monde_courant->goats_tab[j]->x, monde_courant->goats_tab[j]->y);
        if (check_collision_rect(hitbox_pos_potentielle_x.x, hitbox_pos_potentielle_x.y, hitbox_pos_potentielle_x.w, hitbox_pos_potentielle_x.h, hb_goat.x, hb_goat.y, hb_goat.w, hb_goat.h))
        {
          collision_x = 1;
          break;
        }
      }
    }

    // Si l'axe X est libre, on valide le mouvement X
    if (!collision_x)
    {
      fermier_actuel->x = pos_potentielle_x;
    }

    // ....... déplacement et gestion des collisions et de la glisse sur l'axe y
    float pos_potentielle_y = next_fermier_y;
    
    // bord de map en Y
    if (pos_potentielle_y < MARGE)
      pos_potentielle_y = MARGE;
    if (pos_potentielle_y > HAUTEUR - MARGE - HEIGHT_FERMIER)
      pos_potentielle_y = HAUTEUR - MARGE - HEIGHT_FERMIER;

    int collision_y = 0;
    // On teste le Y en utilisant la nouvelle coordonnée X du fermier 
    Hitbox hitbox_pos_potentielle_y = get_hitbox_fermier(fermier_actuel->x, pos_potentielle_y);

    // Collision de l'axe Y avec les obstacles du terrain (lac, maison)
    if (check_collision_obstacles(hitbox_pos_potentielle_y))
    {
      collision_y = 1;
    }

    // Collision de l'axe Y avec les chèvres
    if (!collision_y)
    {
      for (int j = 0; j < monde_courant->nb_goat; j++)
      {
        Hitbox hb_goat = get_hitbox_goat(monde_courant->goats_tab[j]->x, monde_courant->goats_tab[j]->y);
        if (check_collision_rect(hitbox_pos_potentielle_y.x, hitbox_pos_potentielle_y.y, hitbox_pos_potentielle_y.w, hitbox_pos_potentielle_y.h, hb_goat.x, hb_goat.y, hb_goat.w, hb_goat.h))
        {
          collision_y = 1;
          break;
        }
      }
    }

    // Si l'axe Y est libre, on valide le mouvement Y
    if (!collision_y)
    {
      fermier_actuel->y = pos_potentielle_y;
    }

    // Index de frame d'animation du fermier
    if (tick_animation % 6 == 0)
    {
      fermier_actuel->frame = (fermier_actuel->frame + 1) % 9;
    }
  }
  return fermier_actuel;
}

/**
 * @brief Effectue la mise à jour globale du comportement du fermier (prise de décision RL ou manuelle, puis déplacement).
 * @param[in,out] monde_courant Pointeur vers le monde de la simulation.
 * @param[in] tick_animation Horodatage/index de frame courant.
 * @param[in] input_x Déplacement horizontal saisi au clavier (mode manuel).
 * @param[in] input_y Déplacement vertical saisi au clavier (mode manuel).
 * @return monde* Le monde mis à jour.
 */
monde *mis_a_jour_fermier(monde *monde_courant, int tick_animation, int input_x, int input_y)
{
  if (!monde_courant || !monde_courant->fermiers)
    return monde_courant;

  ActionFermier action_fermier;
  action_fermier.dx = 0;
  action_fermier.dy = 0;

  // Fermier, Prise de décision
  if (monde_courant->mode)
  {
    Fermier *fermier = monde_courant->fermiers;
    fermier->decision_cooldown--;

    if (fermier->decision_cooldown <= 0)
    {
      PerceptionFermier perception_fermier = calculer_perception_fermier(fermier, monde_courant);
      
      evaluer_interets_fermier(fermier, perception_fermier);
      
      int action_choisie = choisir_action_softmax(fermier->table_interets, NB_ACTIONS_FERMIER);
      fermier->action_id = action_choisie;

      // Traduction de ActionFermierType vers ActionFermier (dx, dy)
      ActionFermier action;
      action.dx = 0;
      action.dy = 0;
      switch (action_choisie)
      {
      case ACTION_FERMIER_AVANCER:
        action.dy = 1;
        break;
      case ACTION_FERMIER_RECULER:
        action.dy = -1;
        break;
      case ACTION_FERMIER_DROITE:
        action.dx = -1;
        break;
      case ACTION_FERMIER_GAUCHE:
        action.dx = 1;
        break;
      case ACTION_FERMIER_HAUT_GAUCHE:
        action.dx = 1;
        action.dy = 1;
        break;
      case ACTION_FERMIER_HAUT_DROITE:
        action.dx = -1;
        action.dy = 1;
        break;
      case ACTION_FERMIER_BAS_GAUCHE:
        action.dx = 1;
        action.dy = -1;
        break;
      case ACTION_FERMIER_BAS_DROITE:
        action.dx = -1;
        action.dy = -1;
        break;
      default:
        break;
      }
      fermier->dirrection_choisi = action;
      fermier->decision_cooldown = 30 + (rand() % 45); // Cooldown entre 0.5s et 1.25s à 60 FPS
    }
    action_fermier = fermier->dirrection_choisi;
  }
  else
  {
    PerceptionFermier perception_fermier;
    perception_fermier.input_x = input_x;
    perception_fermier.input_y = input_y;
    action_fermier = decider_action_fermier(monde_courant->fermiers, perception_fermier);
  }

  // Application de l'action du Fermier
  update_fermier(monde_courant, monde_courant->fermiers, action_fermier, tick_animation);

  return monde_courant;
}

/**
 * @brief Libère la mémoire allouée pour le fermier.
 * @param[in,out] fermier Pointeur vers le fermier à libérer.
 */
void free_fermier(Fermier *fermier)
{
  if (fermier != NULL)
  {
    free(fermier);
  }
}
