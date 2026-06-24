#ifndef goat_monde_h
#define goat_monde_h

#include "monde.h"

Goat *init_goat(Goat *goat, int x, int y);
monde *ajouter_goat(monde *monde_courant, Goat *goat);
PerceptionGoat calculer_perception_goat(Goat *goat, monde *monde_courant);
Goat *update_goat(monde *monde_courant, Goat *goat, ActionGoat action, int tick_animation, PerceptionGoat perception_goat);
void mourrir_goat(monde *monde_courant, int index);
void free_goats(Goat **goats_tab, int nb_goat);

#endif