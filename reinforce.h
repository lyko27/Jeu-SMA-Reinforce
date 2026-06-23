#ifndef REINFORCE_H
#define REINFORCE_H

#include "fermier.h"
#include "loup.h"

struct monde_t;

typedef struct {
  float phi[7]; 
  int action;              
  float recompense;
} Transition;

typedef struct {
  Transition *transitions;  
  int taille;       
  int capacite; 
} Trajectoire;

void init_trajectoire(Trajectoire *t);
void ajouter_transition(Trajectoire *t, float *phi, int action, float recompense);
void liberer_trajectoire(Trajectoire *t);

void generer_phi_fermier(PerceptionFermier perception, float *phi);
void generer_phi_wolf(Wolf *w, PerceptionWolf perception, float *phi);

void mise_a_jour_reinforce_fermier(Fermier *f, Trajectoire *trajectoires, int nb_episodes, float alpha, float gamma);
void mise_a_jour_reinforce_loups(Wolf **loups, int nb_loups, Trajectoire *trajectoires, int nb_episodes, float alpha, float gamma);

int sauvegarder_poids_fermier(Fermier *f, char *filepath);
int charger_poids_fermier(Fermier *f, char *filepath);
int sauvegarder_poids_loup(Wolf *w, char *filepath);
int charger_poids_loup(Wolf *w, char *filepath);

#endif
