#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "loup.h"
#include "reinforce.h"

void evaluer_interets_wolf(Wolf *wolf, PerceptionWolf perception_wolf)
{
    // Calcul de la distance avec le fermier
    float dx_fermier = perception_wolf.pos_x_fermier - wolf->x;
    float dy_fermier = perception_wolf.pos_y_fermier - wolf->y;
    float dist_fermier = sqrt(dx_fermier * dx_fermier + dy_fermier * dy_fermier);

    // Fuir le fermier s'il est proche
    if (dist_fermier < 200.0f)
    {
        wolf->table_interets[ACTION_WOLF_FUIR_FERMIER] = 15.0f;
        wolf->table_interets[ACTION_WOLF_CHASSER] = -10.0f;
        wolf->table_interets[ACTION_WOLF_ERRER] = -10.0f;
        wolf->table_interets[ACTION_WOLF_ARRET] = -10.0f;
    }
    // Chasser la chèvre si elle est proche
    else if (perception_wolf.pos_x_goat != -1 && perception_wolf.dist_goat_proche < 300.0f)
    {
        wolf->table_interets[ACTION_WOLF_FUIR_FERMIER] = -10.0f;
        wolf->table_interets[ACTION_WOLF_CHASSER] = 15.0f - (perception_wolf.dist_goat_proche / 20.0f);
        wolf->table_interets[ACTION_WOLF_ERRER] = -2.0f;
        wolf->table_interets[ACTION_WOLF_ARRET] = -5.0f;
    }
    // par défaut
    else
    {
        wolf->table_interets[ACTION_WOLF_FUIR_FERMIER] = -10.0f;
        wolf->table_interets[ACTION_WOLF_CHASSER] = -10.0f;
        wolf->table_interets[ACTION_WOLF_ERRER] = 8.0f;
        wolf->table_interets[ACTION_WOLF_ARRET] = 5.0f;
    }
}
void evaluer_interets_wolf_rl(Wolf *wolf, PerceptionWolf perception_wolf)
{
    float phi[DIMENSION_PHI_WOLF];
    generer_phi_wolf(wolf, perception_wolf, phi);
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