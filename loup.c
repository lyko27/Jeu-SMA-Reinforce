#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "loup.h"

void evaluer_interets_wolf(Wolf *l, PerceptionLoup p) {
    (void)p; // Pour l'instant on ne s'en sert pas, on le fera plus tard pour chasser
    
    // Par défaut, un loup erre.
    l->table_interets[ACTION_LOUP_ERRER] = 5.0f;
    l->table_interets[ACTION_LOUP_CHASSER] = 0.0f;
    l->table_interets[ACTION_LOUP_ARRET] = 0.0f;
}

ActionLoup decider_action_wolf(Wolf *l, PerceptionLoup p) {
    evaluer_interets_wolf(l, p);
    
    if (l->table_interets[ACTION_LOUP_CHASSER] > l->table_interets[ACTION_LOUP_ERRER]) {
        l->action_choisi = ACTION_LOUP_CHASSER;
        return l->action_choisi;
    }
    
    l->timer_mouvement--;
    if (l->timer_mouvement <= 0) {
        int chance = rand() % 100;
        if (chance < 20) {
            l->action_choisi = ACTION_LOUP_ARRET;
            l->timer_mouvement = 30 + (rand() % 90);
        } else {
            l->action_choisi = ACTION_LOUP_ERRER;
            l->timer_mouvement = 30 + (rand() % 90);
            l->angle_actuel = ((float)rand()/(float)RAND_MAX) * 2.0 * 3.14;
        }
    }
    
    return l->action_choisi;
}