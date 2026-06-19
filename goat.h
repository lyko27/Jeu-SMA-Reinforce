#ifndef goat_h
#define goat_h

#define WIDTH_GOAT 50
#define HEIGHT_GOAT 30
#define TAILLE_MAP 1024

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
} Goat;

Goat *init_goat(Goat *g);
Goat *update_goat(Goat *g, Goat **all_goats, int nb_goats);
int check_collision(float x1, float y1, float x2, float y2);

#endif
