#ifndef goat_h
#define goat_h

#define WIDTH_GOAT 50
#define HEIGHT_GOAT 30
#define TAILLE_MAP 1024

typedef enum {
    ACTION_FUIR_LOUP,      // Fuit dans la direction opposée au loup
    ACTION_ERRER,          // Marche aléatoirement
    NB_ACTIONS
} ActionPossible;

typedef struct {
    float dist_loup_proche;
    int pos_x_loup;
    int pos_y_loup;
} Perception;

typedef struct
{
    float x;
    float y;
    float speed;
    float dir_x;
    float dir_y;
    int frame;
    int direction_sprite; // 1 haut, 2 droite, 3 bas, 4,gauche
    int en_mouvement;
    int timer_mouvement; // Le temps restant à marcher dans la même direction
    float angle_actuel;
    
    float table_interets[NB_ACTIONS];
    int decision_cooldown ;
    ActionPossible action_choisi
} Goat;

Goat *init_goat(Goat *g);
Goat *update_goat(Goat *g, Goat **all_goats, int nb_goats);
int check_collision_rect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

#endif
