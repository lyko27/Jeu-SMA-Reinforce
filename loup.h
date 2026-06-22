#ifndef wolf_h
#define wolf_h

#include "goat.h"

typedef enum {
    ACTION_WOLF_ERRER,
    ACTION_WOLF_CHASSER,
    ACTION_WOLF_ARRET,
    NB_ACTIONS_WOLF
} ActionWolf;

typedef struct {
    Goat **goats_tab;
    int nb_goat;
    float dist_goat_proche;
    int pos_x_goat;
    int pos_y_goat;
    int pos_x_fermier;
    int pos_y_fermier;
} PerceptionWolf;

typedef struct
{
    float x;
    float y;
    float speed;
    float dir_x;
    float dir_y;
    int frame;
    int en_mouvement;
    int direction_sprite; // 1 haut, 2 droite, 3 bas, 4,gauche
    int timer_mouvement; // Le temps restant à marcher dans la même direction
    float angle_actuel;
    
    float table_interets[NB_ACTIONS_WOLF];
    int decision_cooldown;
    ActionWolf action_choisi;
} Wolf;

ActionWolf decider_action_wolf(Wolf *wolf, PerceptionWolf perception_wolf);
void evaluer_interets_wolf(Wolf *wolf, PerceptionWolf perception_wolf);

#endif
