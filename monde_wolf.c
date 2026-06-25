/**
 * @file monde_wolf.c
 * @brief Implémentation des fonctions de gestion de l'agent Wolf dans le monde
 * @details Gère la création, la perception, les mouvements et collisions ainsi que la destruction des wolfs
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "monde_wolf.h"

/* ========== Loups ========== */

/**
 * @brief Initialise un wolf avec des coordonnées de spawn valides (hors obstacles) et des attributs par défaut
 * @details Configure les positions de départ de manière aléatoire en évitant les obstacles de la carte
 * Initialise sa vitesse, son sprite, son angle de déplacement, ses PV et ses matrices d'apprentissage
 * @param[in,out] wolf Pointeur vers la structure du wolf à initialiser
 * @return Pointeur vers le wolf initialisé
 */
Wolf *init_wolf(Wolf *wolf)
{
  int coordonnees_valides = 0;
  float position_x_generee = 0;
  float position_y_generee = 0;

  while (!coordonnees_valides)
  {
    // spawn uniquement DANS la carte et loin des bords
    position_x_generee = MARGE + (rand() % (LARGEUR - 2 * MARGE - WIDTH_WOLF));
    position_y_generee = MARGE + (rand() % (HAUTEUR - 2 * MARGE - HEIGHT_WOLF));

    // hitbox temporaire pour la validation
    Hitbox hitbox_temporaire = get_hitbox_wolf(position_x_generee, position_y_generee);

    // Si aucune collision n'est détectée avec les obstacles de la carte (lac, maison)
    if (!check_collision_obstacles(hitbox_temporaire))
    {
      coordonnees_valides = 1;
    }
  }

  wolf->x = position_x_generee;
  wolf->y = position_y_generee;

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

  // Initialisation de la table des intérêts
  for (int index_action = 0; index_action < NB_ACTIONS_WOLF; index_action++)
  {
    wolf->table_interets[index_action] = 0.0f;
  }

  // Initialisation de la matrice de poids
  for (int index_action = 0; index_action < NB_ACTIONS_WOLF; index_action++)
  {
    for (int index_phi = 0; index_phi < DIMENSION_PHI_WOLF; index_phi++)
    {
      wolf->weights[index_action][index_phi] = 0.0f;
    }
  }
  return wolf;
}

/**
 * @brief Ajoute un wolf dans le tableau des loups du monde
 * @details Réalloue le tableau dynamique des wolfs si la capacité maximale est dépassée
 * @param[in,out] monde_courant Pointeur vers le monde de la simulation
 * @param[in] wolf Pointeur vers le wolf à ajouter au monde
 * @return Pointeur vers le monde mis à jour avec le nouveau wolf
 */
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

/**
 * @brief Calcule la perception d'un wolf
 * @details Parcourt toutes les goats présentes pour identifier la plus proche à moins de 300 pixels
 * Stocke la distance et ses coordonées et la position du fermier
 * @param[in] wolf Pointeur vers le wolf concerné
 * @param[in] monde_courant Pointeur vers le monde
 * @return Structure contenant la perception calculée pour le loup
 */
PerceptionWolf calculer_perception_wolf(Wolf *wolf, monde *monde_courant)
{
  PerceptionWolf perception;
  perception.dist_goat_proche = 300.0f; // Valeur par défaut de la talle de sa vision, n'importe quel chèvre dans le rayon sera plus proche
  perception.goats_tab = monde_courant->goats_tab;
  perception.nb_goat = monde_courant->nb_goat;
  perception.pos_x_fermier = monde_courant->fermiers->x;
  perception.pos_y_fermier = monde_courant->fermiers->y;
  perception.pos_x_goat = -1;
  perception.pos_y_goat = -1;

  for (int index_goat = 0; index_goat < monde_courant->nb_goat; index_goat++)
  {
    Goat *goat_courant = monde_courant->goats_tab[index_goat];
    float distance_x = goat_courant->x - wolf->x;
    float distance_y = goat_courant->y - wolf->y;
    float distance_euclidienne = sqrt(distance_x * distance_x + distance_y * distance_y);

    if (distance_euclidienne < perception.dist_goat_proche)
    {
      perception.dist_goat_proche = distance_euclidienne;
      perception.pos_x_goat = goat_courant->x;
      perception.pos_y_goat = goat_courant->y;
    }
  }
  return perception;
}

/**
 * @brief Met à jour l'état d'un wolf (déplacement, collisions et animation) pour un tick donné
 * @details Gère la logique et le comportement, exploration, poursuite d'une chèvre ou fuite face au fermier
 * Pour la poursuite (ACTION_WOLF_CHASSER), Calcul de l'angle à l'aide de la fonction trigonométrique atan2 :
 * \code wolf->angle_actuel = atan2(y_destination - y_actuel, x_destination - x_actuel); \endcode
 * Cette fonction calcule le vecteur qui va du loup vers la cible (la goat la plus proche)
 * et renvoie l'angle exact (en radians) par rapport à l'axe des abscisses positive
 * que le loup doit suivre pour aller vers la chèvre (source : )
 * 
 * @param[in,out] monde_courant Pointeur vers le monde de la simulation
 * @param[in,out] wolf Pointeur vers le loup à mettre à jour
 * @param[in] action Action sélectionnée pour le loup
 * @param[in] tick_animation Indice du tick pour l'animation
 * @param[in] perception_wolf Perception actuelle du loup
 * @return Pointeur vers le wolf mis à jour
 */
Wolf *update_wolf(monde *monde_courant, Wolf *wolf, ActionWolf action, int tick_animation, PerceptionWolf perception_wolf)
{
  float prochaine_position_x = wolf->x;
  float prochaine_position_y = wolf->y;

  if (action == ACTION_WOLF_ERRER || action == ACTION_WOLF_CHASSER)
  {
    wolf->speed = 2;

    if (action == ACTION_WOLF_CHASSER)
    {
      if (perception_wolf.pos_x_goat != -1)
      {
        // Poursuite de la goat la plus proche
        // Calcul trigonométrique de l'angle requis avec la fonction atan2 :
        // atan2(y_destination - y_actuel, x_destination - x_actuel) => angle pour s'orienter vers la destination
        // Soit l'angle entre l'axe des abscisses et le vecteur menant à la destination recherchée
        wolf->angle_actuel = atan2(perception_wolf.pos_y_goat - wolf->y, perception_wolf.pos_x_goat - wolf->x);
      }
    }
  }

  if (action == ACTION_WOLF_FUIR_FERMIER)
  {
    wolf->speed = 4;
    // Fuite face au fermier dans la direction opposée
    wolf->angle_actuel = atan2(wolf->y - perception_wolf.pos_y_fermier, wolf->x - perception_wolf.pos_x_fermier);
  }
  if (action == ACTION_WOLF_ARRET)
  {
    wolf->speed = 0;
  }

  prochaine_position_x += cos(wolf->angle_actuel) * wolf->speed;
  prochaine_position_y += sin(wolf->angle_actuel) * wolf->speed;

  if (fabs(prochaine_position_x - wolf->x) > fabs(prochaine_position_y - wolf->y))
  {
    if (prochaine_position_x > wolf->x) // De déplacement vers la droite
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
    if (prochaine_position_y > wolf->y)
    {
      wolf->direction_sprite = 3;
    }
    else
    {
      wolf->direction_sprite = 1;
    }
  }

  // Si le loup touche ou dépasse une paroi, on la repositionne et on fait rebondir son angle
  if (prochaine_position_x < MARGE)
  {
    prochaine_position_x = MARGE;
    wolf->angle_actuel = 3.14159265f - wolf->angle_actuel; // Rebond horizontal
  }
  else if (prochaine_position_x > LARGEUR - MARGE - WIDTH_WOLF)
  {
    prochaine_position_x = LARGEUR - MARGE - WIDTH_WOLF;
    wolf->angle_actuel = 3.14159265f - wolf->angle_actuel; // Rebond horizontal
  }

  if (prochaine_position_y < MARGE)
  {
    prochaine_position_y = MARGE;
    wolf->angle_actuel = -wolf->angle_actuel; // Rebond vertical
  }
  else if (prochaine_position_y > HAUTEUR - MARGE - HEIGHT_WOLF)
  {
    prochaine_position_y = HAUTEUR - MARGE - HEIGHT_WOLF;
    wolf->angle_actuel = -wolf->angle_actuel; // Rebond vertical
  }

  float ancienne_position_x = wolf->x;
  float ancienne_position_y = wolf->y;

  // deplacement et collision axe x
  int collision_axe_x = 0;
  Hitbox hitbox_potentielle_x = get_hitbox_wolf(prochaine_position_x, wolf->y);

  // avec les obstacles du terrain (lac, maison)
  if (check_collision_obstacles(hitbox_potentielle_x))
  {
    collision_axe_x = 1;
    wolf->timer_mouvement = 0;
  }

  // avec d'autres loups
  if (!collision_axe_x)
  {
    for (int index_autre_wolf = 0; index_autre_wolf < monde_courant->nb_wolf; index_autre_wolf++)
    {
      if (monde_courant->wolfs_tab[index_autre_wolf] != wolf)
      {
        Hitbox hitbox_autre = get_hitbox_wolf(monde_courant->wolfs_tab[index_autre_wolf]->x, monde_courant->wolfs_tab[index_autre_wolf]->y);
        if (check_collision_rect(hitbox_potentielle_x.x, hitbox_potentielle_x.y, hitbox_potentielle_x.w, hitbox_potentielle_x.h, hitbox_autre.x, hitbox_autre.y, hitbox_autre.w, hitbox_autre.h))
        {
          collision_axe_x = 1;
          wolf->timer_mouvement = 0;
          break;
        }
      }
    }
  }

  if (!collision_axe_x)
  {
    wolf->x = prochaine_position_x;
    wolf->dir_x = prochaine_position_x;
  }
  else
  {
    wolf->dir_x = wolf->x;
  }

  // deplacement et colision axe y
  // On teste le Y en utilisant la nouvelle coordonnée X du wolf, note : prochaine_position_y est déjà clampée ci-dessus
  int collision_axe_y = 0;
  Hitbox hitbox_potentielle_y = get_hitbox_wolf(wolf->x, prochaine_position_y);

  // avec les obstacles du terrain (lac, maison)
  if (check_collision_obstacles(hitbox_potentielle_y))
  {
    collision_axe_y = 1;
    wolf->timer_mouvement = 0;
  }

  // avec d'autres loups
  if (!collision_axe_y)
  {
    for (int index_autre_wolf = 0; index_autre_wolf < monde_courant->nb_wolf; index_autre_wolf++)
    {
      if (monde_courant->wolfs_tab[index_autre_wolf] != wolf)
      {
        Hitbox hitbox_autre = get_hitbox_wolf(monde_courant->wolfs_tab[index_autre_wolf]->x, monde_courant->wolfs_tab[index_autre_wolf]->y);
        if (check_collision_rect(hitbox_potentielle_y.x, hitbox_potentielle_y.y, hitbox_potentielle_y.w, hitbox_potentielle_y.h, hitbox_autre.x, hitbox_autre.y, hitbox_autre.w, hitbox_autre.h))
        {
          collision_axe_y = 1;
          wolf->timer_mouvement = 0;
          break;
        }
      }
    }
  }

  if (!collision_axe_y)
  {
    wolf->y = prochaine_position_y;
    wolf->dir_y = prochaine_position_y;
  }
  else
  {
    wolf->dir_y = wolf->y;
  }

  if (tick_animation % 6 == 0)
  {
    int a_bouge = (wolf->x != ancienne_position_x || wolf->y != ancienne_position_y);
    if (wolf->speed > 0 && a_bouge)
    {
      wolf->frame = (wolf->frame + 1) % 3;
    }
    else
    {
      wolf->frame = 0;
    }
  }
  return wolf;
}

/**
 * @brief Gère la mort d'un wolf en le libérant et en mettant à jour le tableau du monde
 * @details Libère la mémoire associée à la structure du wolf et décale tous les éléments suivants
 * @param[in,out] monde_courant Pointeur vers le monde de la simulation
 * @param[in] index_wolf Indice du wolf à supprimer du tableau
 */
void mourrir_wolf(monde *monde_courant, int index_wolf)
{
  if (index_wolf < 0 || index_wolf >= monde_courant->nb_wolf)
  {
    return; // le loup d'indice indiqué n'existe pas
  }

  free(monde_courant->wolfs_tab[index_wolf]);

  for (int index_decalage = index_wolf; index_decalage < monde_courant->nb_wolf - 1; index_decalage++)
  {
    monde_courant->wolfs_tab[index_decalage] = monde_courant->wolfs_tab[index_decalage + 1];
  }

  monde_courant->wolfs_tab[monde_courant->nb_wolf - 1] = NULL;
  monde_courant->nb_wolf--;
}

/**
 * @brief Libère la mémoire de tous les loup alloués dans le tableau
 * @details Parcourt le tableau de loups pour libérer individuellement chaque structure existante
 * @param[in,out] tableau_wolfs Tableau contenant les pointeurs vers les loups
 * @param[in] nombre_wolfs Nombre de loups dans le tableau
 */
void free_wolf(Wolf **tableau_wolfs, int nombre_wolfs)
{
  if (tableau_wolfs != NULL)
  {
    for (int index_wolf = 0; index_wolf < nombre_wolfs; index_wolf++)
    {
      if (tableau_wolfs[index_wolf] != NULL)
      {
        free(tableau_wolfs[index_wolf]);
        tableau_wolfs[index_wolf] = NULL;
      }
    }
  }
}
