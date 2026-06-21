#ifndef loup_h
#define loup_h

#include "goat.h"

typedef enum {
    ACTION_LOUP_ERRER,
    ACTION_LOUP_CHASSER,
    ACTION_LOUP_ARRET,
    NB_ACTIONS_LOUP
} ActionLoup;

typedef struct {
    Goat **goats_tab;
    int nb_goat;
    int pos_x_fermier;
    int pos_y_fermier;
} PerceptionLoup;

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
    
    float table_interets[NB_ACTIONS_LOUP];
    ActionLoup action_choisi;
} Wolf;

ActionLoup decider_action_wolf(Wolf *l, PerceptionLoup p);
void evaluer_interets_wolf(Wolf *l, PerceptionLoup p);

#endif
