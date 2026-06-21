#ifndef fermier_h
#define fermier_h

typedef enum {
    ACTION_FERMIER_BOUGER_HAUT,
    ACTION_FERMIER_BOUGER_BAS,
    ACTION_FERMIER_BOUGER_GAUCHE,
    ACTION_FERMIER_BOUGER_DROITE,
    ACTION_FERMIER_IMMOBILE,
    NB_ACTIONS_FERMIER
} ActionFermier;

typedef struct {
    int input_x; // -1, 0, 1
    int input_y; // -1, 0, 1
} PerceptionFermier;

typedef struct
{
    float x;
    float y;
    float speed;
    float dir_x;
    float dir_y;
    int frame;
    int direction_sprite; // 1 haut, 2 droite, 3 bas, 4,gauche
    
    ActionFermier action_choisi;
} Fermier;

ActionFermier decider_action_fermier(Fermier *f, PerceptionFermier p);

#endif
