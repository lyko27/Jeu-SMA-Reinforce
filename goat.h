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
} Goat;

void init_goats(Goat *goats, int num_goats);
void update_goats(Goat *goats, int num_goats);

#endif
