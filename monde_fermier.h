#ifndef fermier_monde_h
#define fermier_monde_h

#include "monde.h"

Fermier *init_fermier(Fermier *fermier);
monde *ajouter_fermier(monde *monde_courant, Fermier *fermier);
PerceptionFermier calculer_perception_fermier(Fermier *fermier, monde *monde_courant);
Fermier *update_fermier(monde *monde_courant, Fermier *fermier, ActionFermier action, int tick_animation, PerceptionFermier perception_fermier);
monde *mis_a_jour_fermier(monde *monde_courant, int tick_animation, int input_x, int input_y);
void free_fermier(Fermier *fermier);

#endif
