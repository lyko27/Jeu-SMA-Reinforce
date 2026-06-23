#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "monde_wolf.h"

/* ========== Loups ========== */

/**
 * Synopsis : Initialise un loup avec des coordonnées aléatoires et une
 * direction par défaut. Entrée   : Pointeur vers la structure Wolf à
 * initialiser. Sortie   : Pointeur vers la structure Wolf initialisée.
 */
Wolf *init_wolf(Wolf *wolf)
{
  wolf->x = rand() % (LARGEUR - WIDTH_WOLF);
  wolf->y = rand() % (HAUTEUR - HEIGHT_WOLF);

  wolf->dir_x = wolf->x;
  wolf->dir_y = wolf->y;

  wolf->speed = 0;
  wolf->direction_sprite = 1 + (rand() % 4);
  wolf->frame = 0;
  wolf->timer_mouvement = 10;
  wolf->en_mouvement = 0;
  wolf->angle_actuel = 0.0f;

  for (int i = 0; i < NB_ACTIONS_WOLF; i++)
  {
    wolf->table_interets[i] = 0.0f;
  }
  for (int a = 0; a < NB_ACTIONS_WOLF; a++)
  {
    for (int k = 0; k < DIMENSION_PHI_WOLF; k++)
    {
      wolf->weights[a][k] = 0.0f;
    }
  }
  return wolf;
}

/* ENtrées : le monde actuel dans sa structure et un wolf
    Sotie : Le monde mis a jour avec le nouveau wolf en plus
    Synopsis : prend un wolf et l'ajoute au monde*/
monde *ajouter_wolf(monde *monde_courant, Wolf *wolf)
{
  if (monde_courant->nb_wolf + 1 > monde_courant->capacite_max_wolf)
  {
    monde_courant->wolfs_tab =
        realloc(monde_courant->wolfs_tab,
                monde_courant->capacite_max_wolf * 2 * sizeof(Wolf));
    monde_courant->capacite_max_wolf *= 2;
  }
  monde_courant->wolfs_tab[monde_courant->nb_wolf] = wolf;
  monde_courant->nb_wolf++;
  return monde_courant;
}

PerceptionWolf calculer_perception_wolf(Wolf *wolf, monde *monde_courant)
{
  PerceptionWolf perception;
  perception.dist_goat_proche = 300.0f; // Valeur par défaut
  perception.goats_tab = monde_courant->goats_tab;
  perception.nb_goat = monde_courant->nb_goat;
  perception.pos_x_fermier = monde_courant->fermiers->x;
  perception.pos_y_fermier = monde_courant->fermiers->y;
  perception.pos_x_goat = -1;
  perception.pos_y_goat = -1;

  for (int i = 0; i < monde_courant->nb_goat; i++)
  {
    Goat *current_goat = monde_courant->goats_tab[i];
    float dist_x = current_goat->x - wolf->x;
    float dist_y = current_goat->y - wolf->y;
    float distance = sqrt(dist_x * dist_x + dist_y * dist_y);

    if (distance < perception.dist_goat_proche)
    {
      perception.dist_goat_proche = distance;
      perception.pos_x_goat = current_goat->x;
      perception.pos_y_goat = current_goat->y;
    }
  }
  return perception;
}

Wolf *update_wolf(monde *monde_courant, Wolf *wolf, ActionWolf action,
                  int tick_animation, PerceptionWolf perception_wolf)
{
  float next_x = wolf->x;
  float next_y = wolf->y;

  if (action == ACTION_WOLF_ERRER || action == ACTION_WOLF_CHASSER)
  {
    wolf->speed = 2;

    if (action == ACTION_WOLF_CHASSER)
    {
      if (perception_wolf.pos_x_goat != -1)
      {
        // Aller vers la chèvre
        wolf->angle_actuel = atan2(perception_wolf.pos_y_goat - wolf->y,
                                   perception_wolf.pos_x_goat - wolf->x);
      }
    }
  }

  if (action == ACTION_WOLF_FUIR_FERMIER)
  {
    wolf->speed = 4;
    // Fuir le fermier
    wolf->angle_actuel = atan2(wolf->y - perception_wolf.pos_y_fermier,
                               wolf->x - perception_wolf.pos_x_fermier);
  }
  if (action == ACTION_WOLF_ARRET)
  {
    wolf->speed = 0;
  }

  next_x += cos(wolf->angle_actuel) * wolf->speed;
  next_y += sin(wolf->angle_actuel) * wolf->speed;

  if (fabs(next_x - wolf->x) > fabs(next_y - wolf->y))
  {
    if (next_x > wolf->x) // on va a droite
    {
      wolf->direction_sprite = 2;
    }
    else
    {
      wolf->direction_sprite = 4;
    }
  }
  else
  {
    if (next_y > wolf->y)
    {
      wolf->direction_sprite = 3;
    }
    else
    {
      wolf->direction_sprite = 1;
    }
  }
  if (next_x < MARGE)
    next_x = MARGE;
  if (next_y < MARGE)
    next_y = MARGE;
  if (next_x > LARGEUR - MARGE - WIDTH_WOLF)
    next_x = LARGEUR - MARGE - WIDTH_WOLF;
  if (next_y > HAUTEUR - MARGE - HEIGHT_WOLF)
    next_y = HAUTEUR - MARGE - HEIGHT_WOLF;

  int collision = 0;

  // Hitbox future du loup (Marges : gauche, droite, haut, bas)
  Hitbox hb_future = creer_hitbox(next_x, next_y, WIDTH_WOLF, HEIGHT_WOLF,
                                  20.0f, 20.0f, 15.0f, 15.0f);
  Hitbox hb_lac =
      creer_hitbox(LAC_X, LAC_Y, LAC_WIDTH, LAC_HEIGHT, 0.0f, 0.0f, 0.0f, 0.0f);

  if (check_collision_rect(hb_future.x, hb_future.y, hb_future.w, hb_future.h,
                           hb_lac.x, hb_lac.y, hb_lac.w, hb_lac.h))
  {
    collision = 1;
    wolf->timer_mouvement = 0;
  }

  if (!collision)
  {
    for (int j = 0; j < monde_courant->nb_wolf; j++)
    {
      if (monde_courant->wolfs_tab[j] != wolf)
      {
        Hitbox hb_autre = creer_hitbox(
            monde_courant->wolfs_tab[j]->x, monde_courant->wolfs_tab[j]->y,
            WIDTH_WOLF, HEIGHT_WOLF, 20.0f, 20.0f, 15.0f, 15.0f);

        if (check_collision_rect(hb_future.x, hb_future.y, hb_future.w,
                                 hb_future.h, hb_autre.x, hb_autre.y,
                                 hb_autre.w, hb_autre.h))
        {
          collision = 1;
          wolf->timer_mouvement = 0;
          break;
        }
      }
    }
  }
  if (!collision)
  {
    wolf->x = next_x;
    wolf->y = next_y;
    wolf->dir_x = next_x;
    wolf->dir_y = next_y;
  }
  else
  {
    wolf->dir_x = wolf->x;
    wolf->dir_y = wolf->y;
  }

  if (tick_animation % 6 == 0)
  {
    if (wolf->speed > 0 && !collision)
    {
      wolf->frame = (wolf->frame + 1) % 4;
    }
    else
    {
      wolf->frame = 0;
    }
  }
  return wolf;
}

/* ENtrées : le tableau de wolf, nombre de wolf
    Sotie : aucune
    Synopsis : libère toute les wolfs du tableau*/
void free_wolf(Wolf **wolf_tab, int nombre_wolf)
{
  if (wolf_tab != NULL)
  {
    for (int i = 0; i < nombre_wolf; i++)
    {
      if (wolf_tab[i] != NULL)
      {
        free(wolf_tab[i]);
        wolf_tab[i] = NULL;
      }
    }
  }
}
