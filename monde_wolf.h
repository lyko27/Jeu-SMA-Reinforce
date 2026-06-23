#ifndef wolf_monde_h
#define wolf_monde_h

#include "monde.h"

Wolf *init_wolf(Wolf *wolf);
monde *ajouter_wolf(monde *monde_courant, Wolf *wolf);
PerceptionWolf calculer_perception_wolf(Wolf *wolf, monde *monde_courant);
Wolf *update_wolf(monde *monde_courant, Wolf *wolf, ActionWolf action, int tick_animation, PerceptionWolf perception_wolf);
void mourrir_wolf(monde *monde_courant, int index);
void free_wolf(Wolf **wolf_tab, int nombre_wolf);

#endif