#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "goat.h"
#include "monde.h"
#include "loup.h"

 /**
 * Synopsis : Décide de la prochaine action de la chèvre en fonction de sa perception_goat.
 * Entrée   : Pointeur vers la chèvre, Perception de la chèvre.
 * Sortie   : Action choisie par la chèvre.
 */
ActionGoat decider_action_goat(Goat *goat, PerceptionGoat perception_goat) {
    evaluer_interets_goat(goat, perception_goat);

    // Si l'intérêt de fuir le wolf est plus fort que l'errance, on fuit
    if (goat->table_interets[ACTION_FUIR_WOLF] > goat->table_interets[ACTION_ERRER] &&
        goat->table_interets[ACTION_FUIR_WOLF] > 0) {
        goat->action_choisi = ACTION_FUIR_WOLF;
        return ACTION_FUIR_WOLF;
    }

    // Sinon, comportement par défaut : errer ou brouter
    goat->timer_mouvement--;
    if (goat->timer_mouvement <= 0) {
        int chance = rand() % 100;

        if (chance < 80) { // 80% de chance de s'arrêter pour brouter
            goat->action_choisi = ACTION_BROUTER;
            goat->timer_mouvement = 30 + (rand() % 90);
        } 
        else { // 20% de chance de commencer à marcher
            goat->action_choisi = ACTION_ERRER;
            goat->timer_mouvement = 30 + (rand() % 90);
            goat->angle_actuel = ((float)rand()/(float)RAND_MAX) * 2.0 * 3.14;
        }
    }

    return goat->action_choisi;
}


void evaluer_interets_goat(Goat *current_goat, PerceptionGoat perception_goat) 
{
    // intérêts par défaut
    current_goat->table_interets[ACTION_ERRER] = 0.0;
    current_goat->table_interets[ACTION_FUIR_WOLF] = 0.0;
    current_goat->table_interets[ACTION_FUIR_WOLF] = 0.0;

    // modif selon la Perception
    if (perception_goat.dist_wolf_proche < 150.0) 
    {
        // on fuit si le wolf est très proche
        current_goat->table_interets[ACTION_FUIR_WOLF] = 10.0 - (perception_goat.dist_wolf_proche / 15.0);
        current_goat->table_interets[ACTION_ERRER] = -10.0f;
    }
    else
    {
        // sinon la chèvre chill
        current_goat->table_interets[ACTION_FUIR_WOLF] = -10.0;
        current_goat->table_interets[ACTION_ERRER] = 8.0;
        current_goat->table_interets[ACTION_BROUTER] = 10.0;
    }
}