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
  int coordonnees_valides = 0;
  float x_genere = 0;
  float y_genere = 0;

  while (!coordonnees_valides)
  {
    // Limiter aux limites autorisées de la map (MARGE à LARGEUR - MARGE - WIDTH)
    x_genere = MARGE + (rand() % (LARGEUR - 2 * MARGE - WIDTH_WOLF));
    y_genere = MARGE + (rand() % (HAUTEUR - 2 * MARGE - HEIGHT_WOLF));

    // Créer une hitbox temporaire
    Hitbox hb_temp = get_hitbox_wolf(x_genere, y_genere);

    // Si pas de collision avec le lac ou la maison
    if (!check_collision_obstacles(hb_temp))
    {
      coordonnees_valides = 1;
    }
  }

  wolf->x = x_genere;
  wolf->y = y_genere;

  wolf->dir_x = wolf->x;
  wolf->dir_y = wolf->y;

  wolf->speed = 0;
  wolf->direction_sprite = 1 + (rand() % 4);
  wolf->frame = 0;
  wolf->timer_mouvement = 10;
  wolf->en_mouvement = 0;
  wolf->angle_actuel = 0.0f;

  wolf->hp = 2;
  wolf->cooldown_dinvisibilite = 180; // 3 secondes

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
    monde_courant->wolfs_tab = realloc(monde_courant->wolfs_tab, monde_courant->capacite_max_wolf * 2 * sizeof(Wolf));
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

Wolf *update_wolf(monde *monde_courant, Wolf *wolf, ActionWolf action, int tick_animation, PerceptionWolf perception_wolf)
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
        /* on calcul le vecteur qui va du loup vers la chèvre, et atan2 renvoie l'angle exact que le loup doit suivre pour aller sur la chèvre
        atan2(x_direction_voulu - x_actuel, y_direction_voulu - y_actuel) => angle pour aller à direction voulu
        soit l'angle entre l'axe des abscisses et le vecteur de direction voulu*/
        wolf->angle_actuel = atan2(perception_wolf.pos_y_goat - wolf->y,
                                   perception_wolf.pos_x_goat - wolf->x);
      }
    }
  }

  if (action == ACTION_WOLF_FUIR_FERMIER)
  {
    wolf->speed = 4;
    // Fuir le fermier
    wolf->angle_actuel = atan2(wolf->y - perception_wolf.pos_y_fermier, wolf->x - perception_wolf.pos_x_fermier);
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
  float old_x = wolf->x;
  float old_y = wolf->y;

  // Déplacement et Collision sur l'axe X
  float pos_potentielle_x = next_x;
  if (pos_potentielle_x < MARGE)
    pos_potentielle_x = MARGE;
  if (pos_potentielle_x > LARGEUR - MARGE - WIDTH_WOLF)
    pos_potentielle_x = LARGEUR - MARGE - WIDTH_WOLF;

  int collision_x = 0;
  Hitbox hb_pos_potentielle_x = get_hitbox_wolf(pos_potentielle_x, wolf->y);

  // Collision de l'axe X avec les obstacles du terrain (lac, maison)
  if (check_collision_obstacles(hb_pos_potentielle_x))
  {
    collision_x = 1;
    wolf->timer_mouvement = 0;
  }

  // Collision de l'axe X avec d'autres loups
  if (!collision_x)
  {
    for (int j = 0; j < monde_courant->nb_wolf; j++)
    {
      if (monde_courant->wolfs_tab[j] != wolf)
      {
        Hitbox hb_autre = get_hitbox_wolf(monde_courant->wolfs_tab[j]->x, monde_courant->wolfs_tab[j]->y);
        if (check_collision_rect(hb_pos_potentielle_x.x, hb_pos_potentielle_x.y, hb_pos_potentielle_x.w, hb_pos_potentielle_x.h, hb_autre.x, hb_autre.y, hb_autre.w, hb_autre.h))
        {
          collision_x = 1;
          wolf->timer_mouvement = 0;
          break;
        }
      }
    }
  }

  if (!collision_x)
  {
    wolf->x = pos_potentielle_x;
    wolf->dir_x = pos_potentielle_x;
  }
  else
  {
    wolf->dir_x = wolf->x;
  }

  // --- Déplacement et Collision sur l'axe Y ---
  float pos_potentielle_y = next_y;
  if (pos_potentielle_y < MARGE)
    pos_potentielle_y = MARGE;
  if (pos_potentielle_y > HAUTEUR - MARGE - HEIGHT_WOLF)
    pos_potentielle_y = HAUTEUR - MARGE - HEIGHT_WOLF;

  int collision_y = 0;
  // On teste le Y en utilisant la nouvelle coordonnée X du loup
  Hitbox hb_pos_potentielle_y = get_hitbox_wolf(wolf->x, pos_potentielle_y);

  // Collision de l'axe Y avec les obstacles du terrain (lac, maison)
  if (check_collision_obstacles(hb_pos_potentielle_y))
  {
    collision_y = 1;
    wolf->timer_mouvement = 0;
  }

  // Collision de l'axe Y avec d'autres loups
  if (!collision_y)
  {
    for (int j = 0; j < monde_courant->nb_wolf; j++)
    {
      if (monde_courant->wolfs_tab[j] != wolf)
      {
        Hitbox hb_autre = get_hitbox_wolf(monde_courant->wolfs_tab[j]->x, monde_courant->wolfs_tab[j]->y);
        if (check_collision_rect(hb_pos_potentielle_y.x, hb_pos_potentielle_y.y, hb_pos_potentielle_y.w, hb_pos_potentielle_y.h, hb_autre.x, hb_autre.y, hb_autre.w, hb_autre.h))
        {
          collision_y = 1;
          wolf->timer_mouvement = 0;
          break;
        }
      }
    }
  }

  if (!collision_y)
  {
    wolf->y = pos_potentielle_y;
    wolf->dir_y = pos_potentielle_y;
  }
  else
  {
    wolf->dir_y = wolf->y;
  }

  if (tick_animation % 6 == 0)
  {
    int a_bouge = (wolf->x != old_x || wolf->y != old_y);
    if (wolf->speed > 0 && a_bouge)
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

void mourrir_wolf(monde *monde_courant, int index)
{
  if (index < 0 || index >= monde_courant->nb_wolf)
  {
    return; // le loup d'indice i n'existe pas
  }

  free(monde_courant->wolfs_tab[index]);

  for (int i = index; i < monde_courant->nb_wolf - 1; i++)
  {
    monde_courant->wolfs_tab[i] = monde_courant->wolfs_tab[i + 1];
  }

  monde_courant->wolfs_tab[monde_courant->nb_wolf - 1] = NULL;
  monde_courant->nb_wolf--;
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
