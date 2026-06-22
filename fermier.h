#ifndef fermier_h
#define fermier_h

#define DIMENSION_PHI_FERMIER 7

typedef struct {
  int dx;
  int dy;
} ActionFermier;

typedef enum {
  ACTION_FERMIER_ERRER,
  ACTION_FERMIER_AVANCER,
  ACTION_FERMIER_RECULER,
  ACTION_FERMIER_DROITE,
  ACTION_FERMIER_GAUCHE,
  ACTION_FERMIER_HAUT_GAUCHE,
  ACTION_FERMIER_HAUT_DROITE,
  ACTION_FERMIER_BAS_GAUCHE,
  ACTION_FERMIER_BAS_DROITE,
  NB_ACTIONS_FERMIER
} ActionFermierType;

typedef struct {
  int input_x; // -1, 0, 1
  int input_y; // -1, 0, 1
} PerceptionFermier;

typedef struct {
  float x;
  float y;
  float speed;
  float dir_x;
  float dir_y;
  int frame;
  int direction_sprite; // 1 haut, 2 droite, 3 bas, 4,gauche
  float table_interets[NB_ACTIONS_FERMIER];
  float weights[NB_ACTIONS_FERMIER][DIMENSION_PHI_FERMIER];
  int decision_cooldown;
  ActionFermier action_choisi;
  int action_id;
} Fermier;

ActionFermier decider_action_fermier(Fermier *fermier,
                                     PerceptionFermier perception_fermier);

#endif
