/**
 * @file monde_goat.c
 * @brief Implémentation des fonctions de gestion de l'agent Chèvre dans le monde
 * @details Gère la création, la perception, les mouvements et collisions ainsi que la destruction des chèvres
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "monde_goat.h"

/* ========== Chèvres ========== */

/**
 * @brief Initialise une chèvre avec ses coordonnées et ses attributs par défaut
 * @details Configure les positions initiales, la vitesse, le sprite, le timer de mouvement et la vie de la chèvre
 * Initialise également la table des intérêts pour la prise de décision
 * @param[in,out] goat Pointeur vers la structure de la chèvre à initialiser
 * @param[in] position_x pos x initiale de la chèvre
 * @param[in] position_y pos y initiale de la chèvre
 * @return Pointeur vers la chèvre initialisée
 */
Goat *init_goat(Goat *goat, int position_x, int position_y)
{
  goat->x = position_x;
  goat->y = position_y;

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
  for (int index_action = 0; index_action < NB_ACTIONS; index_action++)
  {
    goat->table_interets[index_action] = 0.0f;
  }

  return goat;
}

/**
 * @brief Ajoute une chèvre dans le tableau des chèvres du monde
 * @details Réalloue le tableau des chèvres si la capacité max est dépassée
 * @param[in,out] monde_courant Pointeur vers le monde
 * @param[in] goat Pointeur vers la chèvre à ajouter au monde
 * @return Pointeur vers le monde mis à jour avec la nouvelle chèvre
 */
monde *ajouter_goat(monde *monde_courant, Goat *goat)
{
  if (monde_courant->nb_goat + 1 > monde_courant->capacite_max_goat)
  {
    monde_courant->goats_tab = realloc(monde_courant->goats_tab, monde_courant->capacite_max_goat * 2 * sizeof(Goat));
    monde_courant->capacite_max_goat *= 2;
  }
  monde_courant->goats_tab[monde_courant->nb_goat] = goat;
  monde_courant->nb_goat++;
  return monde_courant;
}

/**
 * @brief Calcule ce qu'une chèvre perçoit
 * @details Identifie le loup le plus proche s'il est à portée de détection (< 200 pixels) et stocke ses coordonnées
 * Récupère aussi le tableau de chèvres et la position du fermier
 * @param[in] goat Pointeur vers la chèvre concernée
 * @param[in] monde_courant Pointeur vers le monde de la simulation
 * @return Structure contenant la perception de la chèvre
 */
PerceptionGoat calculer_perception_goat(Goat *goat, monde *monde_courant)
{
  PerceptionGoat perception;
  perception.dist_wolf_proche = 200.0f; // Valeur par défaut
  perception.goats_tab = monde_courant->goats_tab;
  perception.nb_goat = monde_courant->nb_goat;
  perception.pos_x_fermier = monde_courant->fermiers->x;
  perception.pos_y_fermier = monde_courant->fermiers->y;

  // Initialisation par défaut pour éviter des valeurs indéterminées
  perception.pos_x_wolf = -1;
  perception.pos_y_wolf = -1;

  for (int index_loup = 0; index_loup < monde_courant->nb_wolf; index_loup++)
  {
    Wolf *loup_courant = monde_courant->wolfs_tab[index_loup];
    float distance_x = loup_courant->x - goat->x;
    float distance_y = loup_courant->y - goat->y;
    float distance_euclidienne = sqrt(distance_x * distance_x + distance_y * distance_y);

    if (distance_euclidienne < perception.dist_wolf_proche)
    {
      perception.dist_wolf_proche = distance_euclidienne;
      perception.pos_x_wolf = loup_courant->x;
      perception.pos_y_wolf = loup_courant->y;
    }
  }
  return perception;
}

/**
 * @brief Met à jour l'état d'une chèvre (déplacement, collisions et animation) pour un tick donné
 * @details Calcule la prochaine position de la chèvre en fonction de son action (errer, fuir ou brouter)
 *          Gère le rebond sur les parois de la carte et les collisions avec les obstacles ou d'autres chèvres
 *          Gère également la mise à jour de la frame d'animation du sprite
 * @param[in,out] monde_courant Pointeur vers le monde de la simulation
 * @param[in,out] goat Pointeur vers la chèvre à mettre à jour
 * @param[in] action Action sélectionnée pour la chèvre
 * @param[in] tick_animation Indice du tick pour l'animation
 * @param[in] perception_goat Perception actuelle de cette chèvre
 * @return Pointeur vers la chèvre mise à jour
 */
Goat *update_goat(monde *monde_courant, Goat *goat, ActionGoat action, int tick_animation, PerceptionGoat perception_goat)
{
  float prochaine_position_x = goat->x;
  float prochaine_position_y = goat->y;

  if (action == ACTION_ERRER || action == ACTION_FUIR_WOLF)
  {
    goat->speed = 1.5;
    if (action == ACTION_FUIR_WOLF)
    {
      if (perception_goat.pos_x_wolf != -1)
      {
        // Fuir dans la direction opposée
        goat->angle_actuel = atan2(goat->y - perception_goat.pos_y_wolf, goat->x - perception_goat.pos_x_wolf);
      }
    }
    prochaine_position_x += cos(goat->angle_actuel) * goat->speed;
    prochaine_position_y += sin(goat->angle_actuel) * goat->speed;

    // decision de la direction du sprite en focntion du déplacement
    if (fabs(prochaine_position_x - goat->x) > fabs(prochaine_position_y - goat->y))
    {
      if (prochaine_position_x > goat->x)
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
      if (prochaine_position_y > goat->y)
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

  // Si la chèvre touche ou dépasse une paroi, on la repositionne et on fait rebondir son angle
  if (prochaine_position_x < MARGE)
  {
    prochaine_position_x = MARGE;
    goat->angle_actuel = 3.14159265f - goat->angle_actuel; // Rebond horizontal
  }
  else if (prochaine_position_x > LARGEUR - MARGE - WIDTH_GOAT)
  {
    prochaine_position_x = LARGEUR - MARGE - WIDTH_GOAT;
    goat->angle_actuel = 3.14159265f - goat->angle_actuel; // Rebond horizontal
  }

  if (prochaine_position_y < MARGE)
  {
    prochaine_position_y = MARGE;
    goat->angle_actuel = -goat->angle_actuel; // Rebond vertical
  }
  else if (prochaine_position_y > HAUTEUR - MARGE - HEIGHT_GOAT)
  {
    prochaine_position_y = HAUTEUR - MARGE - HEIGHT_GOAT;
    goat->angle_actuel = -goat->angle_actuel; // Rebond vertical
  }

  float ancienne_position_x = goat->x;
  float ancienne_position_y = goat->y;

  // colision axe x
  int collision_axe_x = 0;
  Hitbox hitbox_potentielle_x = get_hitbox_goat(prochaine_position_x, goat->y);

  // avec les obstacles du terrain (lac, maison)
  if (check_collision_obstacles(hitbox_potentielle_x))
  {
    collision_axe_x = 1;
    goat->timer_mouvement = 0;
  }

  // avec d'autres chèvres
  if (!collision_axe_x)
  {
    for (int index_autre_goat = 0; index_autre_goat < monde_courant->nb_goat; index_autre_goat++)
    {
      if (monde_courant->goats_tab[index_autre_goat] != goat)
      {
        Hitbox hitbox_autre = get_hitbox_goat(monde_courant->goats_tab[index_autre_goat]->x, monde_courant->goats_tab[index_autre_goat]->y);
        if (check_collision_rect(hitbox_potentielle_x.x, hitbox_potentielle_x.y, hitbox_potentielle_x.w, hitbox_potentielle_x.h, hitbox_autre.x, hitbox_autre.y, hitbox_autre.w, hitbox_autre.h))
        {
          collision_axe_x = 1;
          goat->timer_mouvement = 0;
          break;
        }
      }
    }
  }

  if (!collision_axe_x)
  {
    goat->x = prochaine_position_x;
    goat->dir_x = prochaine_position_x;
  }
  else
  {
    goat->dir_x = goat->x;
  }

  // collision sur l'axe y
  int collision_axe_y = 0;
  Hitbox hitbox_potentielle_y = get_hitbox_goat(goat->x, prochaine_position_y);

  // avec les obstacles du terrain (lac, maison)
  if (check_collision_obstacles(hitbox_potentielle_y))
  {
    collision_axe_y = 1;
    goat->timer_mouvement = 0;
  }

  // avec d'autres chèvres
  if (!collision_axe_y)
  {
    for (int index_autre_goat = 0; index_autre_goat < monde_courant->nb_goat; index_autre_goat++)
    {
      if (monde_courant->goats_tab[index_autre_goat] != goat)
      {
        Hitbox hitbox_autre = get_hitbox_goat(monde_courant->goats_tab[index_autre_goat]->x, monde_courant->goats_tab[index_autre_goat]->y);
        if (check_collision_rect(hitbox_potentielle_y.x, hitbox_potentielle_y.y, hitbox_potentielle_y.w, hitbox_potentielle_y.h, hitbox_autre.x, hitbox_autre.y, hitbox_autre.w, hitbox_autre.h))
        {
          collision_axe_y = 1;
          goat->timer_mouvement = 0;
          break;
        }
      }
    }
  }

  if (!collision_axe_y)
  {
    goat->y = prochaine_position_y;
    goat->dir_y = prochaine_position_y;
  }
  else
  {
    goat->dir_y = goat->y;
  }

  if (tick_animation % 6 == 0)
  {
    int a_bouge = (goat->x != ancienne_position_x || goat->y != ancienne_position_y);
    if (goat->speed > 0 && a_bouge)
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

/**
 * @brief Gère la mort d'une chèvre en la libérant et en mettant à jour le tableau du monde
 * @details Libère la mémoire associée à la chèvre et décale tous les éléments suivants pour combler le vide
 * @param[in,out] monde_courant Pointeur vers le monde de la simulation
 * @param[in] index_goat Indice de la chèvre à supprimer du tableau
 */
void mourrir_goat(monde *monde_courant, int index_goat)
{
  if (index_goat < 0 || index_goat >= monde_courant->nb_goat)
  {
    return; // la chèvre d'indice indiqué n'existe pas
  }

  free(monde_courant->goats_tab[index_goat]);

  for (int index_decalage = index_goat; index_decalage < monde_courant->nb_goat - 1; index_decalage++)
  {
    monde_courant->goats_tab[index_decalage] = monde_courant->goats_tab[index_decalage + 1];
  }

  monde_courant->goats_tab[monde_courant->nb_goat - 1] = NULL;
  monde_courant->nb_goat--;
}

/**
 * @brief Libère la mémoire de toutes les chèvres allouées dans le tableau
 * @details Parcourt le tableau de chèvres pour libérer individuellement chaque structure existante
 * @param[in,out] tableau_goats Tableau contenant les pointeurs vers les chèvres
 * @param[in] nombre_goats Nombre actuel de chèvres dans le tableau
 */
void free_goats(Goat **tableau_goats, int nombre_goats)
{
  if (tableau_goats != NULL)
  {
    for (int index_goat = 0; index_goat < nombre_goats; index_goat++)
    {
      if (tableau_goats[index_goat] != NULL)
      {
        free(tableau_goats[index_goat]);
        tableau_goats[index_goat] = NULL;
      }
    }
  }
}