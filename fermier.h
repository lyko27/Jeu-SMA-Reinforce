#ifndef fermier_h
#define fermier_h

typedef struct {
    int dx;
    int dy;
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

ActionFermier decider_action_fermier(Fermier *fermier, PerceptionFermier perception_fermier);

#endif
