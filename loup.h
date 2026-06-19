#ifndef loup_h
#define loup_h

#define WIDTH_WOLF 50
#define HEIGHT_WOLF 30
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
    int timer_mouvement; // Le temps restant à marcher dans la même direction
    float angle_actuel;
} Wolf;

Wolf *init_wolf(Wolf *g);
Wolf *update_wolf(Wolf *g, Wolf **all_wolfs, int nb_wolfs);
int check_collision(float x1, float y1, float x2, float y2);

#endif
