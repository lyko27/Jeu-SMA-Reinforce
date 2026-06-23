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
  fermier->x = 347;
  fermier->y = 185;
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
    if (next_fermier_x < MARGE)
      next_fermier_x = MARGE;
    if (next_fermier_x > LARGEUR - MARGE - WIDTH_FERMIER)
      next_fermier_x = LARGEUR - MARGE - WIDTH_FERMIER;
    if (next_fermier_y < MARGE)
      next_fermier_y = MARGE;
    if (next_fermier_y > HAUTEUR - MARGE - HEIGHT_FERMIER)
      next_fermier_y = HAUTEUR - MARGE - HEIGHT_FERMIER;

    int collision_fermier = 0;

    Hitbox hb_future_fermier = get_hitbox_fermier(next_fermier_x, next_fermier_y);
    Hitbox hb_lac = creer_hitbox(LAC_X, LAC_Y, LAC_WIDTH, LAC_HEIGHT, 0.0f, 0.0f, 0.0f, 0.0f);

    if (check_collision_rect(hb_future_fermier.x, hb_future_fermier.y, hb_future_fermier.w, hb_future_fermier.h, hb_lac.x, hb_lac.y, hb_lac.w, hb_lac.h))
    {
      collision_fermier = 1;
    }
    // collision avec chevres
    if (!collision_fermier)
    {
      for (int j = 0; j < monde_courant->nb_goat; j++)
      {
        Hitbox hb_goat = get_hitbox_goat(monde_courant->goats_tab[j]->x, monde_courant->goats_tab[j]->y);

        if (check_collision_rect(hb_future_fermier.x, hb_future_fermier.y, hb_future_fermier.w, hb_future_fermier.h, hb_goat.x, hb_goat.y, hb_goat.w, hb_goat.h))
        {
          collision_fermier = 1;
          break;
        }
      }
    }
    // collision avec loups
    if (!collision_fermier)
    {
      for (int j = 0; j < monde_courant->nb_wolf; j++)
      {
        Hitbox hb_wolf = get_hitbox_wolf(monde_courant->wolfs_tab[j]->x, monde_courant->wolfs_tab[j]->y);

        if (check_collision_rect(hb_future_fermier.x, hb_future_fermier.y, hb_future_fermier.w, hb_future_fermier.h, hb_wolf.x, hb_wolf.y, hb_wolf.w, hb_wolf.h))
        {
          collision_fermier = 1; // Le fermier se cogne contre un loup
          break;
        }
      }
    }

    if (!collision_fermier)
    {
      fermier_actuel->x = next_fermier_x;
      fermier_actuel->y = next_fermier_y;
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
