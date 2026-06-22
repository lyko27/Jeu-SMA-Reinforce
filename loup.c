#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "loup.h"

void evaluer_interets_wolf(Wolf *wolf, PerceptionWolf perception_wolf) {
    
    // Par défaut, un wolf erre.
    wolf->table_interets[ACTION_WOLF_ERRER] = 5.0f;
    wolf->table_interets[ACTION_WOLF_CHASSER] = 0.0f;
    wolf->table_interets[ACTION_WOLF_ARRET] = 0.0f;
    wolf->table_interets[ACTION_WOLF_FUIR_FERMIER] = 0.0f;

    // modif selon la Perception
    if (perception_wolf.dist_goat_proche < 200.0) 
    {
        // on chasse si la chèvre est proche
        wolf->table_interets[ACTION_WOLF_CHASSER] = 10.0 - (perception_wolf.dist_goat_proche / 20.0);
        wolf->table_interets[ACTION_WOLF_ERRER] = -10.0f;
    }
    if (sqrt((perception_wolf.pos_x_fermier - wolf->x) * (perception_wolf.pos_x_fermier - wolf->x) + (perception_wolf.pos_y_fermier - wolf->y) * (perception_wolf.pos_y_fermier - wolf->y)) < 200.0) 
    {
        // on chasse si la chèvre est proche
        wolf->table_interets[ACTION_WOLF_CHASSER] = -10.0;
        wolf->table_interets[ACTION_WOLF_ERRER] = -10.0f;
        wolf->table_interets[ACTION_WOLF_FUIR_FERMIER] = 10.0f;
    }
    else
    {
        // sinon loup chill
        wolf->table_interets[ACTION_WOLF_CHASSER] = -10.0;
        wolf->table_interets[ACTION_WOLF_ERRER] = 8.0;
        wolf->table_interets[ACTION_WOLF_ARRET] = 10.0;
        wolf->table_interets[ACTION_WOLF_FUIR_FERMIER] = -10.0f;
    }
}