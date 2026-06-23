#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "goat.h"
#include "monde.h"
#include "loup.h"

void evaluer_interets_goat(Goat *current_goat, PerceptionGoat perception_goat)
{
    // intérêts par défaut
    current_goat->table_interets[ACTION_ERRER] = 0.0;
    current_goat->table_interets[ACTION_FUIR_WOLF] = 0.0;
    current_goat->table_interets[ACTION_BROUTER] = 0.0;

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