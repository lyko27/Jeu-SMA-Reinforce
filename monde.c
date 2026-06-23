#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "affichage.h"
#include "fermier.h"
#include "goat.h"
#include "loup.h"
#include "monde.h"
#include "reinforce.h"


/**
 * Synopsis : calcul la proba de chaque action et calcul un nombre aleatoire et
 * en fonction de ce nombre et des proba renvoie une action. Entrée   : le
 * tableau d interet qui contient l interet de chaque action et le nombre d
 * actions possible. Sortie   : le numero de l'action retenue.
 */
int choisir_action_softmax(float *tab_interet, int nb_actions)
{
  float max_interet = tab_interet[0];
  for (int i = 1; i < nb_actions;
       i++)
  { // ici on l'action avec le meilleur interet
    if (tab_interet[i] > max_interet)
    {
      max_interet = tab_interet[i];
    }
  }

  // on calcule exp(interet de chaque action) et on met ca dans le tableau de
  // probabilite
  float somme_exp = 0.0f;
  float probabilites[nb_actions];

  for (int i = 0; i < nb_actions; i++)
  {
    probabilites[i] = exp(tab_interet[i] - max_interet);
    somme_exp +=
        probabilites[i]; // on calcul la somme des probabilites en meme temps
  }

  float tirage = (float)rand() /
                 (float)RAND_MAX; // le tirage simule le de dont on a parle pour
                                  // le cas d un environnement stochastique
  float somme_cumulee = 0.0f;
  for (int i = 0; i < nb_actions; i++)
  {
    somme_cumulee +=
        (probabilites[i] / somme_exp); // on normalise la proba pour que proba
                                       // soit comprise entre 0 et 1
    if (tirage <= somme_cumulee)
    {
      return i;
    }
  }
  return 0; // securite
}

/**
 * Synopsis : Vérifie la collision entre deux objets rectangulaires.
 * Entrée   : Coordonnées (x1, y1) du premier objet, coordonnées (x2, y2) du
 * deuxième objet. Sortie   : 1 si collision, 0 sinon.
 */
int check_collision_rect(float x1, float y1, float w1, float h1, float x2,
                         float y2, float w2, float h2)
{
  if (x1 + w1 < x2 || x2 + w2 < x1 || y1 + h1 < y2 || y2 + h2 < y1)
  {
    return 0; // Pas de collision
  }
  return 1; // Collision
}

/**
 * Synopsis : Crée et retourne la Hitbox à partir d'un rectangle de base.
 * Permet de réduire la taille de la zone sensible du sprite d'une entité en
 * appliquant des marges horizontales et verticales, afin d'ignorer les zones
 * transparentes de l'image. Entrée   : - x, y : Coordonnées du coin supérieur
 * gauche du sprite d'origine.
 * - w, h : Largeur et hauteur totales du sprite d'origine.
 * - marge_x : Nombre de pixels à retirer à gauche et à droite.
 * - marge_y : Nombre de pixels à retirer en haut et en bas.
 * Sortie   : Une structure Hitbox contenant les nouvelles coordonnées (x, y) et
 * dimensions (w, h) ajustées pour la logique de collision.
 */
Hitbox creer_hitbox(float x, float y, float w, float h, float marge_gauche,
                    float marge_droite, float marge_haut, float marge_bas)
{
  Hitbox hb;
  hb.x = x + marge_gauche;
  hb.y = y + marge_haut;
  hb.w = w - (marge_gauche + marge_droite);
  hb.h = h - (marge_haut + marge_bas);

  // Sécurité : éviter d'avoir une largeur ou hauteur négative ou nulle
  if (hb.w <= 0.0f)
    hb.w = 1.0f;
  if (hb.h <= 0.0f)
    hb.h = 1.0f;

  return hb;
}

Hitbox get_hitbox_fermier(float x, float y)
{
  return creer_hitbox(x, y, WIDTH_FERMIER, HEIGHT_FERMIER, 10.0f, 1.0f, 10.0f,
                      10.0f);
}

Hitbox get_hitbox_goat(float x, float y)
{
  return creer_hitbox(x, y, WIDTH_GOAT, HEIGHT_GOAT, 0.5f, 28.0f, 10.0f, 10.0f);
}

Hitbox get_hitbox_wolf(float x, float y)
{
  return creer_hitbox(x, y, WIDTH_WOLF, HEIGHT_WOLF, 0.5f, 5.0f, 10.0f, 10.0f);
}

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



/* ========= Monde ========= */

/* Entrée : deux entier la largeur et la hauteur du monde
   Sortie : le monde vide
   synopsis : créer un monde vide avec la structure monde*/
monde *creer_monde(int largeur, int hauteur)
{
  monde *monde_courant = malloc(sizeof(monde));
  if (monde_courant)
  {
    monde_courant->largeur = largeur;
    monde_courant->hauteur = hauteur;
    monde_courant->capacite_max_goat = 100;
    monde_courant->nb_goat = 0;
    monde_courant->capacite_max_wolf = 100;
    monde_courant->nb_wolf = 0;
    monde_courant->mode = 0;
    monde_courant->goats_tab =
        malloc(monde_courant->capacite_max_goat * sizeof(Goat *));
    monde_courant->wolfs_tab =
        malloc(monde_courant->capacite_max_wolf * sizeof(Wolf *));

    if (monde_courant->wolfs_tab)
      return monde_courant;
    else
    {
      free(monde_courant->wolfs_tab);
      free(monde_courant);
      return NULL;
    }
  }
  else
  {
    free(monde_courant);
    return NULL;
  }
}

/* Entrée : le monde actuel, normalement complètement vide
    Sortie : le monde avec le fermier et 10 chèvre qui apparaisse aléatoirement
    Synopsis : remplie le monde de 10 chèvre à des endroits aléatoire et le
   fermier devant sa maison */
monde *generer_un_monde(monde *monde_courant)
{
  Fermier *le_fermier = malloc(sizeof(Fermier));
  if (le_fermier)
  {
    le_fermier->frame = 0;
    le_fermier->direction_sprite = 2;
    le_fermier = init_fermier(le_fermier);
    monde_courant->fermiers = le_fermier;
  }
  else
  {
    free(le_fermier);
    return NULL;
  }
  for (int i = 0; i < 10; i++)
  {
    Goat *une_goat = malloc(sizeof(Goat));
    if (une_goat)
    {
      une_goat->frame = 0;
      une_goat->direction_sprite = 2;
      une_goat = init_goat(une_goat);
      monde_courant = ajouter_goat(monde_courant, une_goat);
    }
    else
    {
      free(une_goat);
      return NULL;
    }
  }
  for (int i = 0; i < 3; i++)
  {
    Wolf *un_wolf = malloc(sizeof(Wolf));
    if (un_wolf)
    {
      un_wolf->frame = 0;
      un_wolf->direction_sprite = 2;
      un_wolf = init_wolf(un_wolf);
      monde_courant = ajouter_wolf(monde_courant, un_wolf);
    }
    else
    {
      free(un_wolf);
      return NULL;
    }
  }
  return monde_courant;
}

/* Entrée : le monde actuel
    Sortie : le monde mis à jour
    Synopsis : Met à jour la position de chaque entité après déplacement dans
   notre jeu */
monde *mis_à_jour_monde(monde *monde_courant, int tick_animation, int input_x, int input_y)
{
  ActionFermier action_fermier;

  // Fermier - Décision
  if (monde_courant->mode)
  {
    float phi[7];
    calcul_interets_fermier(monde_courant->fermiers, monde_courant, phi);
    for (int a = 0; a < NB_ACTIONS_FERMIER; a++)
    {
      float val = 0.0f;
      for (int k = 0; k < DIMENSION_PHI_FERMIER; k++)
      {
        val += monde_courant->fermiers->weights[a][k] * phi[k];
      }
      monde_courant->fermiers->table_interets[a] = val;
    }
    int action_choisie = choisir_action_softmax(monde_courant->fermiers->table_interets, NB_ACTIONS_FERMIER);
    monde_courant->fermiers->action_id = action_choisie;

    // Mapper de ActionFermierType vers ActionFermier
    action_fermier.dx = 0;
    action_fermier.dy = 0;
    switch (action_choisie)
    {
    case ACTION_FERMIER_AVANCER:
      action_fermier.dy = 1;
      break;
    case ACTION_FERMIER_RECULER:
      action_fermier.dy = -1;
      break;
    case ACTION_FERMIER_DROITE:
      action_fermier.dx = -1;
      break;
    case ACTION_FERMIER_GAUCHE:
      action_fermier.dx = 1;
      break;
    case ACTION_FERMIER_HAUT_GAUCHE:
      action_fermier.dx = 1;
      action_fermier.dy = 1;
      break;
    case ACTION_FERMIER_HAUT_DROITE:
      action_fermier.dx = -1;
      action_fermier.dy = 1;
      break;
    case ACTION_FERMIER_BAS_GAUCHE:
      action_fermier.dx = 1;
      action_fermier.dy = -1;
      break;
    case ACTION_FERMIER_BAS_DROITE:
      action_fermier.dx = -1;
      action_fermier.dy = -1;
      break;
    default:
      break;
    }
    monde_courant->fermiers->action_choisi = action_fermier;
  }
  else
  {
    PerceptionFermier perception_fermier;
    perception_fermier.input_x = input_x;
    perception_fermier.input_y = input_y;
    action_fermier = decider_action_fermier(monde_courant->fermiers, perception_fermier);

    int action_choisie = ACTION_FERMIER_ERRER;
    if (input_x == 1 && input_y == 1)
      action_choisie = ACTION_FERMIER_HAUT_GAUCHE;
    else if (input_x == -1 && input_y == 1)
      action_choisie = ACTION_FERMIER_HAUT_DROITE;
    else if (input_x == 1 && input_y == -1)
      action_choisie = ACTION_FERMIER_BAS_GAUCHE;
    else if (input_x == -1 && input_y == -1)
      action_choisie = ACTION_FERMIER_BAS_DROITE;
    else if (input_y == 1)
      action_choisie = ACTION_FERMIER_AVANCER;
    else if (input_y == -1)
      action_choisie = ACTION_FERMIER_RECULER;
    else if (input_x == 1)
      action_choisie = ACTION_FERMIER_GAUCHE;
    else if (input_x == -1)
      action_choisie = ACTION_FERMIER_DROITE;

    monde_courant->fermiers->action_id = action_choisie;
  }

  // Chèvres
  for (int i = 0; i < monde_courant->nb_goat; i++)
  {
    Goat *goat = monde_courant->goats_tab[i];
    goat->decision_cooldown--;

    // Prise de décision (seulement à l'expiration du cooldown)
    if (goat->decision_cooldown <= 0)
    {
      PerceptionGoat perception_goat = calculer_perception_goat(goat, monde_courant);
      evaluer_interets_goat(goat, perception_goat);
      goat->action_choisi = choisir_action_softmax(goat->table_interets, NB_ACTIONS);
      if (goat->action_choisi == ACTION_ERRER)
        goat->angle_actuel = ((float)rand() / (float)RAND_MAX) * 2.0 * 3.14;
      goat->decision_cooldown = 30 + (rand() % 45); // Entre 0.5s et 1.25s à 60 FPS
    }
    update_goat(monde_courant, goat, goat->action_choisi, tick_animation, calculer_perception_goat(goat, monde_courant));
  }

  // Loups
  for (int i = 0; i < monde_courant->nb_wolf; i++)
  {
    Wolf *wolf = monde_courant->wolfs_tab[i];
    wolf->decision_cooldown--;

    if (wolf->decision_cooldown <= 0)
    {
      if (monde_courant->mode)
      {
        float phi[7];
        calcul_interets_wolf(wolf, monde_courant, phi);
        for (int a = 0; a < NB_ACTIONS_WOLF; a++)
        {
          float val = 0.0f;
          for (int k = 0; k < DIMENSION_PHI_WOLF; k++)
          {
            val += wolf->weights[a][k] * phi[k];
          }
          wolf->table_interets[a] = val;
        }
      }
      else
      {
        PerceptionWolf perception_wolf =
            calculer_perception_wolf(wolf, monde_courant);
        evaluer_interets_wolf(wolf, perception_wolf);
      }
      wolf->action_choisi =
          choisir_action_softmax(wolf->table_interets, NB_ACTIONS_WOLF);
      if (wolf->action_choisi == ACTION_WOLF_ERRER)
        wolf->angle_actuel = ((float)rand() / (float)RAND_MAX) * 2.0 * 3.14;
      wolf->decision_cooldown = 30 + (rand() % 45); // Entre 0.5s et 1.25s à 60 FPS
    }
    update_wolf(monde_courant, wolf, wolf->action_choisi, tick_animation, calculer_perception_wolf(wolf, monde_courant));
  }

  // ========
  // Action physique du Fermier
  // ========
  Fermier *fermier_actuel = monde_courant->fermiers;
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
    // collision avec chevres
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

  return monde_courant;
}

/* ========= Affichage ========= */

/**
 * Synopsis : Affiche le monde courant, incluant le terrain, le fermier, les
 * chèvres et les loups. Entrée   : Pointeur vers le monde à afficher. Sortie :
 * Aucune.
 */
void afficher_monde(monde *monde_courant)
{
  if (!monde_courant)
    return;
  dessiner_monde();

  for (int i = 0; i < monde_courant->nb_goat; i++)
  {
    Goat *g = monde_courant->goats_tab[i];
    if (g)
      dessiner_entite(1, g->x, g->y, g->frame, g->direction_sprite);
  }
  for (int i = 0; i < monde_courant->nb_wolf; i++)
  {
    Wolf *w = monde_courant->wolfs_tab[i];
    if (w)
      dessiner_entite(4, w->x, w->y, w->frame, w->direction_sprite);
  }
  if (monde_courant->fermiers)
  {
    dessiner_entite(2, monde_courant->fermiers->x, monde_courant->fermiers->y, monde_courant->fermiers->frame, monde_courant->fermiers->direction_sprite);
  }
  afficher_compteur(monde_courant->nb_goat);
  actualiser_ecran();
}
