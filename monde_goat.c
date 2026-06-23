#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "monde_goat.h"

/* ========== Chèvres ========== */

/**
 * Synopsis : Initialise une chèvre avec des coordonnées aléatoires et une
 * direction par défaut. Entrée   : Pointeur vers la structure Goat à
 * initialiser. Sortie   : Pointeur vers la structure Goat initialisée.
 */
Goat *init_goat(Goat *goat)
{
  goat->x = rand() % (LARGEUR - WIDTH_GOAT);
  goat->y = rand() % (HAUTEUR - HEIGHT_GOAT);

  goat->dir_x = goat->x;
  goat->dir_y = goat->y;

  goat->speed = 0;
  goat->direction_sprite = 1 + (rand() % 4);
  goat->frame = 0;
  goat->timer_mouvement = 10;
  goat->en_mouvement = 0;
  goat->angle_actuel = 0.0f;

  goat->hp = 2;
  goat->cooldown_dinvisibilite = 180; // 3 secondes

  // Initialisation de la table des intérêts
  for (int i = 0; i < NB_ACTIONS; i++)
  {
    goat->table_interets[i] = 0.0f;
  }

  return goat;
}

/* ENtrées : le monde actuel dans sa structure et une chèvre
    Sotie : Le monde mis a jour avec la nouvelle chèvre en plus
    Synopsis : prend une chèvre et l'ajoute au monde*/
monde *ajouter_goat(monde *monde_courant, Goat *goat)
{
  if (monde_courant->nb_goat + 1 > monde_courant->capacite_max_goat)
  {
    monde_courant->goats_tab =
        realloc(monde_courant->goats_tab,
                monde_courant->capacite_max_goat * 2 * sizeof(Goat));
    monde_courant->capacite_max_goat *= 2;
  }
  monde_courant->goats_tab[monde_courant->nb_goat] = goat;
  monde_courant->nb_goat++;
  return monde_courant;
}

PerceptionGoat calculer_perception_goat(Goat *goat, monde *monde_courant)
{
  PerceptionGoat perception;
  perception.dist_wolf_proche = 200.0f; // Valeur par défaut
  perception.goats_tab = monde_courant->goats_tab;
  perception.nb_goat = monde_courant->nb_goat;
  perception.pos_x_fermier = monde_courant->fermiers->x;
  perception.pos_y_fermier = monde_courant->fermiers->y;

  for (int i = 0; i < monde_courant->nb_wolf; i++)
  {
    Wolf *current_wolf = monde_courant->wolfs_tab[i];
    float dist_x = current_wolf->x - goat->x;
    float dist_y = current_wolf->y - goat->y;
    float distance = sqrt(dist_x * dist_x + dist_y * dist_y);

    if (distance < perception.dist_wolf_proche)
    {
      perception.dist_wolf_proche = distance;
      perception.pos_x_wolf = current_wolf->x;
      perception.pos_y_wolf = current_wolf->y;
    }
  }
  return perception;
}

Goat *update_goat(monde *monde_courant, Goat *goat, ActionGoat action,
                  int tick_animation, PerceptionGoat perception_goat)
{
  float next_x = goat->x;
  float next_y = goat->y;

  if (action == ACTION_ERRER || action == ACTION_FUIR_WOLF)
  {
    goat->speed = 2;
    if (action == ACTION_FUIR_WOLF)
    {
      if (perception_goat.pos_x_wolf != -1)
      {
        // Fuir dans la direction opposée
        goat->angle_actuel = atan2(goat->y - perception_goat.pos_y_wolf,
                                   goat->x - perception_goat.pos_x_wolf);
      }
    }
    next_x += cos(goat->angle_actuel) * goat->speed;
    next_y += sin(goat->angle_actuel) * goat->speed;

    if (fabs(next_x - goat->x) > fabs(next_y - goat->y))
    {
      if (next_x > goat->x)
      {
        goat->direction_sprite = 2;
      }
      else
      {
        goat->direction_sprite = 4;
      }
    }
    else
    {
      if (next_y > goat->y)
      {
        goat->direction_sprite = 3;
      }
      else
      {
        goat->direction_sprite = 1;
      }
    }
  }
  else if (action == ACTION_BROUTER)
  {
    goat->speed = 0;
  }

  if (next_x < MARGE)
    next_x = MARGE;
  if (next_y < MARGE)
    next_y = MARGE;
  if (next_x > LARGEUR - MARGE - WIDTH_GOAT)
    next_x = LARGEUR - MARGE - WIDTH_GOAT;
  if (next_y > HAUTEUR - MARGE - HEIGHT_GOAT)
    next_y = HAUTEUR - MARGE - HEIGHT_GOAT;

  int collision = 0;

  Hitbox hb_future = get_hitbox_goat(next_x, next_y);
  Hitbox hb_lac =
      creer_hitbox(LAC_X, LAC_Y, LAC_WIDTH, LAC_HEIGHT, 0.0f, 0.0f, 0.0f, 0.0f);

  if (check_collision_rect(hb_future.x, hb_future.y, hb_future.w, hb_future.h,
                           hb_lac.x, hb_lac.y, hb_lac.w, hb_lac.h))
  {
    collision = 1;
    goat->timer_mouvement = 0;
  }

  if (!collision)
  {
    for (int j = 0; j < monde_courant->nb_goat; j++)
    {
      if (monde_courant->goats_tab[j] != goat)
      {
        Hitbox hb_autre = get_hitbox_goat(monde_courant->goats_tab[j]->x,
                                          monde_courant->goats_tab[j]->y);

        if (check_collision_rect(hb_future.x, hb_future.y, hb_future.w,
                                 hb_future.h, hb_autre.x, hb_autre.y,
                                 hb_autre.w, hb_autre.h))
        {
          collision = 1;
          goat->timer_mouvement = 0;
          break;
        }
      }
    }
  }

  if (!collision)
  {
    goat->x = next_x;
    goat->y = next_y;
    goat->dir_x = next_x;
    goat->dir_y = next_y;
  }
  else
  {
    goat->dir_x = goat->x;
    goat->dir_y = goat->y;
  }

  if (tick_animation % 6 == 0)
  {
    if (goat->speed > 0 && !collision)
    {
      goat->frame = (goat->frame + 1) % 4;
    }
    else
    {
      goat->frame = 0;
    }
  }
  return goat;
}

void mourrir_goat(monde *monde_courant, int index)
{
  if (index < 0 || index >= monde_courant->nb_goat)
  {
    return; // la chèvre d'indice i n'existe pas 
  }

  free(monde_courant->goats_tab[index]);

  for (int i = index; i < monde_courant->nb_goat - 1; i++)
  {
    monde_courant->goats_tab[i] = monde_courant->goats_tab[i + 1];
  }

  monde_courant->goats_tab[monde_courant->nb_goat - 1] = NULL;
  monde_courant->nb_goat--;
}

/* ENtrées : le tableau de chèvre, nombre de chèvre
    Sotie : aucune
    Synopsis : libère toute les goats du tableau*/
void free_goats(Goat **goats_tab, int nb_goat)
{
  if (goats_tab != NULL)
  {
    for (int i = 0; i < nb_goat; i++)
    {
      if (goats_tab[i] != NULL)
      {
        free(goats_tab[i]);
        goats_tab[i] = NULL;
      }
    }
  }
}