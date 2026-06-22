#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "loup.h"

void evaluer_interets_wolf(Wolf *wolf, PerceptionWolf perception_wolf) {
    
    // Par défaut, un wolf erre.
    wolf->table_interets[ACTION_WOLF_ERRER] = 5.0f;
    wolf->table_interets[ACTION_WOLF_CHASSER] = 0.0f;
    wolf->table_interets[ACTION_WOLF_ARRET] = 0.0f;

    // modif selon la Perception
    if (perception_wolf.dist_goat_proche < 200.0) 
    {
        // on chasse si la chèvre est proche
        wolf->table_interets[ACTION_WOLF_CHASSER] = 10.0 - (perception_wolf.dist_goat_proche / 20.0);
        wolf->table_interets[ACTION_WOLF_ERRER] = -10.0f;
    }
    else
    {
        // sinon la chèvre chill
        wolf->table_interets[ACTION_WOLF_CHASSER] = -10.0;
        wolf->table_interets[ACTION_WOLF_ERRER] = 8.0;
        wolf->table_interets[ACTION_WOLF_ARRET] = 10.0;
    }

}

ActionWolf decider_action_wolf(Wolf *wolf, PerceptionWolf perception_wolf) {
    evaluer_interets_wolf(wolf, perception_wolf);
    
    if (wolf->table_interets[ACTION_WOLF_CHASSER] > wolf->table_interets[ACTION_WOLF_ERRER]) {
        wolf->action_choisi = ACTION_WOLF_CHASSER;
        return wolf->action_choisi;
    }
    
    wolf->timer_mouvement--;
    if (wolf->timer_mouvement <= 0) {
        int chance = rand() % 100;
        if (chance < 20) {
            wolf->action_choisi = ACTION_WOLF_ARRET;
            wolf->timer_mouvement = 30 + (rand() % 90);
        } else {
            wolf->action_choisi = ACTION_WOLF_ERRER;
            wolf->timer_mouvement = 30 + (rand() % 90);
            wolf->angle_actuel = ((float)rand()/(float)RAND_MAX) * 2.0 * 3.14;
        }
    }
    
    return wolf->action_choisi;
}