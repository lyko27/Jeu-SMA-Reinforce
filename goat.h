#ifndef goat_h
#define goat_h

#define WIDTH_GOAT 50
#define HEIGHT_GOAT 30
#define TAILLE_MAP 1024

typedef struct {
    float x;
    float y;
    float speed;
    float dir_x;
    float dir_y;
    int frame;
    int direction_sprite; // 1 haut, 2 droite, 3 bas, 4,gauche
} Goat;

void init_goats(Goat *goats, int num_goats);
void update_goats(Goat *goats, int num_goats);

#endif
