#include "reinforce.h"
#include "monde.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Gestion des trajectoires
void init_trajectoire(Trajectoire *t)
{
  t->taille = 0;
  t->capacite = 100;
  t->transitions = malloc(t->capacite * sizeof(Transition));
}

void ajouter_transition(Trajectoire *t, float *phi, int action, float recompense)
{
  if (t->taille == t->capacite)
  {
    t->capacite *= 2;
    t->transitions = realloc(t->transitions, t->capacite * sizeof(Transition));
  }
  memcpy(t->transitions[t->taille].phi, phi, 7 * sizeof(float));
  t->transitions[t->taille].action = action;
  t->transitions[t->taille].recompense = recompense;
  t->taille++;
}

void liberer_trajectoire(Trajectoire *t)
{
  if (t->transitions)
  {
    free(t->transitions);
    t->transitions = NULL;
  }
  t->taille = 0;
  t->capacite = 0;
}

// Extraction de phi pour le Fermier
void calcul_interets_fermier(Fermier *f, struct monde_t *m, float *phi)
{
  phi[0] = 1.0f; // Biais

  // Trouver le loup le plus proche
  float dist_wolf = 999999.0f;
  float dx_wolf = 0.0f;
  float dy_wolf = 0.0f;

  for (int i = 0; i < m->nb_wolf; i++)
  {
    Wolf *w = m->wolfs_tab[i];
    if (w)
    {
      float dx = w->x - f->x;
      float dy = w->y - f->y;
      float d = sqrtf(dx * dx + dy * dy);
      if (d < dist_wolf)
      {
        dist_wolf = d;
        dx_wolf = dx;
        dy_wolf = dy;
      }
    }
  }
  if (dist_wolf > 0.001f && dist_wolf < 99999.0f)
  {
    phi[1] = dist_wolf / 1024.0f; // distance normalisée du loup le plus proche
    phi[2] = dx_wolf / dist_wolf; // direction x du loup le plus proche
    phi[3] = dy_wolf / dist_wolf; // direction y du loup le plus proche
  }
  else
  {
    phi[1] = 1.0f;
    phi[2] = 0.0f;
    phi[3] = 0.0f;
  }

  // Trouver le goat la plus proche
  float dist_goat = 999999.0f;
  float dx_goat = 0.0f;
  float dy_goat = 0.0f;
  for (int i = 0; i < m->nb_goat; i++)
  {
    Goat *g = m->goats_tab[i];
    if (g)
    {
      float dx = g->x - f->x;
      float dy = g->y - f->y;
      float d = sqrtf(dx * dx + dy * dy);
      if (d < dist_goat)
      {
        dist_goat = d;
        dx_goat = dx;
        dy_goat = dy;
      }
    }
  }
  if (dist_goat > 0.001f && dist_goat < 99999.0f)
  {
    phi[4] = dist_goat / 1024.0f; // distance normalisée du goat le plus proche
    phi[5] = dx_goat / dist_goat; // direction x du goat le plus proche
    phi[6] = dy_goat / dist_goat; // direction y du goat le plus proche
  }
  else
  {
    phi[4] = 1.0f;
    phi[5] = 0.0f;
    phi[6] = 0.0f;
  }
}

// Extraction de caractéristiques pour le Loup
void calcul_interets_wolf(Wolf *w, struct monde_t *m, float *phi)
{
  phi[0] = 1.0f; // Biais

  // Trouver la goat la plus proche
  float dist_goat = 999999.0f;
  float dx_goat = 0.0f;
  float dy_goat = 0.0f;
  for (int i = 0; i < m->nb_goat; i++)
  {
    Goat *g = m->goats_tab[i];
    if (!g)
      continue;
    float dx = g->x - w->x;
    float dy = g->y - w->y;
    float d = sqrtf(dx * dx + dy * dy);
    if (d < dist_goat)
    {
      dist_goat = d;
      dx_goat = dx;
      dy_goat = dy;
    }
  }
  if (dist_goat > 0.001f && dist_goat < 99999.0f)
  {
    phi[1] = dist_goat / 1024.0f; // distance normalisée du goat le plus proche
    phi[2] = dx_goat / dist_goat; // direction x du goat le plus proche
    phi[3] = dy_goat / dist_goat; // direction y du goat le plus proche
  }
  else
  {
    phi[1] = 1.0f;
    phi[2] = 0.0f;
    phi[3] = 0.0f;
  }

  // Distance au fermier
  Fermier *f = m->fermiers;
  if (f)
  {
    float dx_f = f->x - w->x;
    float dy_f = f->y - w->y;
    float d_f = sqrtf(dx_f * dx_f + dy_f * dy_f);
    if (d_f > 0.001f)
    {
      phi[4] = d_f / 1024.0f; // distance normalisée au fermier
      phi[5] = dx_f / d_f;    // direction x du fermier
      phi[6] = dy_f / d_f;    // direction y du fermier
    }
    else
    {
      phi[4] = 0.0f;
      phi[5] = 0.0f;
      phi[6] = 0.0f;
    }
  }
  else
  {
    phi[4] = 1.0f;
    phi[5] = 0.0f;
    phi[6] = 0.0f;
  }
}

// Softmax
void calculer_softmax(const float *phi, float poids[][7], int nb_actions, int dim_phi, float *probabilites)
{
  float logits[nb_actions];
  float max_logit = -999999.0f;

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

  float somme_exp = 0.0f;
  for (int a = 0; a < nb_actions; a++)
  {
    probabilites[a] = expf(logits[a] - max_logit);
    somme_exp += probabilites[a];
  }
  for (int a = 0; a < nb_actions; a++)
  {
    probabilites[a] /= somme_exp;
  }
}

// Reinforce pour le Fermier
void mise_a_jour_reinforce_fermier(Fermier *f, Trajectoire *trajectoires, int nb_episodes, float alpha, float gamma)
{
  float D[NB_ACTIONS_FERMIER][DIMENSION_PHI_FERMIER];

  for (int i = 0; i < NB_ACTIONS_FERMIER; i++)
  {
    for (int j = 0; j < DIMENSION_PHI_FERMIER; j++)
    {
      D[i][j] = 0.0f;
    }
  }

  for (int i = 0; i < nb_episodes; i++)
  {
    Trajectoire *traj = &trajectoires[i];
    int T = traj->taille;
    float G = 0.0f;

    for (int u = 0; u < T; u++)
    {
      int t = T - 1 - u;
      Transition transition = traj->transitions[t];
      G = transition.recompense + G * gamma;
      float GG = powf(gamma, t) * G;

      float P[NB_ACTIONS_FERMIER];
      calculer_softmax(transition.phi, f->weights, NB_ACTIONS_FERMIER, DIMENSION_PHI_FERMIER, P);

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

  // Mise à jour ascendante des paramètres
  for (int a = 0; a < NB_ACTIONS_FERMIER; a++)
  {
    for (int k = 0; k < DIMENSION_PHI_FERMIER; k++)
    {
      f->weights[a][k] += alpha * (1.0f / nb_episodes) * D[a][k];
    }
  }
}

// Reinforce loups
void mise_a_jour_reinforce_loups(Wolf **loups, int nb_loups, Trajectoire *trajectoires, int nb_episodes, float alpha, float gamma)
{
  float D[NB_ACTIONS_WOLF][DIMENSION_PHI_WOLF];

  for (int i = 0; i < NB_ACTIONS_WOLF; i++)
  {
    for (int j = 0; j < DIMENSION_PHI_WOLF; j++)
    {
      D[i][j] = 0.0f;
    }
  }

  int total_trajectoires = nb_episodes * nb_loups;

  for (int i = 0; i < total_trajectoires; i++)
  {
    Trajectoire *traj = &trajectoires[i];
    int T = traj->taille;
    float G = 0.0f;

    for (int u = 0; u < T; u++)
    {
      int t = T - 1 - u;
      Transition transition = traj->transitions[t];
      G = transition.recompense + G * gamma;
      float GG = powf(gamma, t) * G;

      float P[NB_ACTIONS_WOLF];
      // On utilise les poids du premier loup (ils sont partagés/identiques)
      calculer_softmax(transition.phi, loups[0]->weights, NB_ACTIONS_WOLF, DIMENSION_PHI_WOLF, P);

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
  for (int w = 0; w < nb_loups; w++)
  {
    for (int a = 0; a < NB_ACTIONS_WOLF; a++)
    {
      for (int k = 0; k < DIMENSION_PHI_WOLF; k++)
      {
        loups[w]->weights[a][k] += alpha * (1.0f / total_trajectoires) * D[a][k];
      }
    }
  }
}

// Sauvegarde et chargement des poids du Fermier en format texte meme si pas optimal
int sauvegarder_poids_fermier(Fermier *f, char *filepath)
{
  FILE *file = fopen(filepath, "w");
  if (!file)
    return 0;
  for (int a = 0; a < NB_ACTIONS_FERMIER; a++)
  {
    for (int k = 0; k < DIMENSION_PHI_FERMIER; k++)
    {
      fprintf(file, "%f ", f->weights[a][k]);
    }
    fprintf(file, "\n");
  }
  fclose(file);
  return 1;
}

int charger_poids_fermier(Fermier *f, char *filepath)
{
  FILE *file = fopen(filepath, "r");
  if (!file)
    return 0;
  for (int a = 0; a < NB_ACTIONS_FERMIER; a++)
  {
    for (int k = 0; k < DIMENSION_PHI_FERMIER; k++)
    {
      if (fscanf(file, "%f", &f->weights[a][k]) != 1)
      {
        fclose(file);
        return 0;
      }
    }
  }
  fclose(file);
  return 1;
}

int sauvegarder_poids_loup(Wolf *w, char *filepath)
{
  FILE *file = fopen(filepath, "w");
  if (!file)
    return 0;
  for (int a = 0; a < NB_ACTIONS_WOLF; a++)
  {
    for (int k = 0; k < DIMENSION_PHI_WOLF; k++)
    {
      fprintf(file, "%f ", w->weights[a][k]);
    }
    fprintf(file, "\n");
  }
  fclose(file);
  return 1;
}

int charger_poids_loup(Wolf *w, char *filepath)
{
  FILE *file = fopen(filepath, "r");
  if (!file)
    return 0;
  for (int a = 0; a < NB_ACTIONS_WOLF; a++)
  {
    for (int k = 0; k < DIMENSION_PHI_WOLF; k++)
    {
      if (fscanf(file, "%f", &w->weights[a][k]) != 1)
      {
        fclose(file);
        return 0;
      }
    }
  }
  fclose(file);
  return 1;
}
