#include "fermier.h"
#include "reinforce.h"

/**
 * Synopsis : Décide de la prochaine action du fermier en fonction des flèches directionnelles saisies.
 * Entrée   : Pointeur vers le fermier, Perception du fermier (inputs utilisateur).
 * Sortie   : Action choisie par le fermier.
 */
ActionFermier decider_action_fermier(Fermier *fermier, PerceptionFermier perception_fermier) {
    ActionFermier action;
    action.dx = perception_fermier.input_x;
    action.dy = perception_fermier.input_y;
    fermier->action_choisi = action;
    return action;
}

void evaluer_interets_fermier(Fermier *fermier, PerceptionFermier perception_fermier) {
    float phi[DIMENSION_PHI_FERMIER];
    generer_phi_fermier(perception_fermier, phi);
    for (int a = 0; a < NB_ACTIONS_FERMIER; a++) {
        float val = 0.0f;
        for (int k = 0; k < DIMENSION_PHI_FERMIER; k++) {
            val += fermier->weights[a][k] * phi[k];
        }
        fermier->table_interets[a] = val;
    }
}