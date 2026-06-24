#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "monde_fermier.h"

/* ========== Fermier ========== */

/**
 * Synopsis : Initialise le fermier devant sa maison.
 * Entrée   : Pointeur vers la structure Fermier à initialiser.
 * Sortie   : Pointeur vers la structure Fermier initialisée.
 */
Fermier *init_fermier(Fermier *fermier)
{
  fermier->x = 842;
  fermier->y = 242;
  fermier->speed = VITESSE_FERMIER;
  for (int a = 0; a < NB_ACTIONS_FERMIER; a++)
  {
    for (int k = 0; k < DIMENSION_PHI_FERMIER; k++)
    {
      fermier->weights[a][k] = 0.0f;
    }
  }
  return fermier;
}

monde *ajouter_fermier(monde *monde_courant, Fermier *fermier)
{
  monde_courant->fermiers = fermier;
  return monde_courant;
}

PerceptionFermier calculer_perception_fermier(Fermier *f, monde *monde_courant)
{
  PerceptionFermier perc;
  perc.input_x = 0;
  perc.input_y = 0;
  perc.dist_wolf = 2000.0f; // Valeur par défaut
  perc.dx_wolf = 0.0f;
  perc.dy_wolf = 0.0f;
  perc.dist_goat = 2000.0f;
  perc.dx_goat = 0.0f;
  perc.dy_goat = 0.0f;

  for (int i = 0; i < monde_courant->nb_wolf; i++)
  {
    Wolf *w = monde_courant->wolfs_tab[i];
    if (w)
    {
      float dx = w->x - f->x;
      float dy = w->y - f->y;
      float dist = sqrtf(dx * dx + dy * dy);
      if (dist < perc.dist_wolf)
      {
        perc.dist_wolf = dist;
        perc.dx_wolf = dx;
        perc.dy_wolf = dy;
      }
    }
  }

  for (int i = 0; i < monde_courant->nb_goat; i++)
  {
    Goat *g = monde_courant->goats_tab[i];
    if (g)
    {
      float dx = g->x - f->x;
      float dy = g->y - f->y;
      float dist = sqrtf(dx * dx + dy * dy);
      if (dist < perc.dist_goat)
      {
        perc.dist_goat = dist;
        perc.dx_goat = dx;
        perc.dy_goat = dy;
      }
    }
  }

  return perc;
}

Fermier *update_fermier(monde *monde_courant, Fermier *fermier_actuel, ActionFermier action_fermier, int tick_animation, PerceptionFermier perception_fermier)
{
  (void)perception_fermier;
  float next_fermier_x = fermier_actuel->x;
  float next_fermier_y = fermier_actuel->y;

  float vitesse_actuelle = fermier_actuel->speed;
  if (action_fermier.dx != 0 && action_fermier.dy != 0)
  {
    vitesse_actuelle /= sqrt(2);
  }

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

  if (action_fermier.dx == 0 && action_fermier.dy == 0)
  {
    fermier_actuel->frame = 5;
    fermier_actuel->direction_sprite = 0;
  }
  else
  {
    // --- Déplacement et Collision sur l'axe X ---
    float pos_potentielle_x = next_fermier_x;
    
    // Limites de la carte en X
    if (pos_potentielle_x < MARGE) pos_potentielle_x = MARGE;
    if (pos_potentielle_x > LARGEUR - MARGE - WIDTH_FERMIER) pos_potentielle_x = LARGEUR - MARGE - WIDTH_FERMIER;

    int collision_x = 0;
    Hitbox hb_pos_potentielle_x = get_hitbox_fermier(pos_potentielle_x, fermier_actuel->y);

    // Collision de l'axe X avec les obstacles du terrain (lac, maison)
    if (check_collision_obstacles(hb_pos_potentielle_x))
    {
      collision_x = 1;
    }

    // Collision de l'axe X avec les chèvres
    if (!collision_x)
    {
      for (int j = 0; j < monde_courant->nb_goat; j++)
      {
        Hitbox hb_goat = get_hitbox_goat(monde_courant->goats_tab[j]->x, monde_courant->goats_tab[j]->y);
        if (check_collision_rect(hb_pos_potentielle_x.x, hb_pos_potentielle_x.y, hb_pos_potentielle_x.w, hb_pos_potentielle_x.h, hb_goat.x, hb_goat.y, hb_goat.w, hb_goat.h))
        {
          collision_x = 1;
          break;
        }
      }
    }

    // Si l'axe X est libre, on applique le mouvement X
    if (!collision_x)
    {
      fermier_actuel->x = pos_potentielle_x;
    }

    // --- Déplacement et Collision sur l'axe Y ---
    float pos_potentielle_y = next_fermier_y;
    
    // Limites de la carte en Y
    if (pos_potentielle_y < MARGE)
      pos_potentielle_y = MARGE;
    if (pos_potentielle_y > HAUTEUR - MARGE - HEIGHT_FERMIER)
      pos_potentielle_y = HAUTEUR - MARGE - HEIGHT_FERMIER;

    int collision_y = 0;
    // On teste le Y en utilisant la nouvelle coordonnée X du fermier (mise à jour juste avant)
    Hitbox hb_pos_potentielle_y = get_hitbox_fermier(fermier_actuel->x, pos_potentielle_y);

    // Collision de l'axe Y avec les obstacles du terrain (lac, maison)
    if (check_collision_obstacles(hb_pos_potentielle_y))
    {
      collision_y = 1;
    }

    // Collision de l'axe Y avec les chèvres
    if (!collision_y)
    {
      for (int j = 0; j < monde_courant->nb_goat; j++)
      {
        Hitbox hb_goat = get_hitbox_goat(monde_courant->goats_tab[j]->x, monde_courant->goats_tab[j]->y);
        if (check_collision_rect(hb_pos_potentielle_y.x, hb_pos_potentielle_y.y, hb_pos_potentielle_y.w, hb_pos_potentielle_y.h, hb_goat.x, hb_goat.y, hb_goat.w, hb_goat.h))
        {
          collision_y = 1;
          break;
        }
      }
    }

    // Si l'axe Y est libre, on applique le mouvement Y
    if (!collision_y)
    {
      fermier_actuel->y = pos_potentielle_y;
    }

    if (tick_animation % 6 == 0)
    {
      fermier_actuel->frame = (fermier_actuel->frame + 1) % 9;
    }
  }
  return fermier_actuel;
}

void free_fermier(Fermier *fermier)
{
  if (fermier != NULL)
  {
    free(fermier);
  }
}
