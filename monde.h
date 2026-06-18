#ifndef MONDE_H
#define MONDE_H

typedef enum {
    FERMIER,
    CHEVRE,
} TypeEntite;

typedef struct {
    int x;
    int y;
} Vecteur2D;

// une entitée
typedef struct {
    Vecteur2D position;
    Vecteur2D velocite; // La direction et la vitesse actuelles
    TypeEntite type;
    int en_vie;         // 1 si actif, 0 si mort (à retirer du tableau)
} entitee_t;

// Le Monde 
typedef struct {
    int largeur;
    int hauteur;
    entitee_t * entites;      // Tableau dynamique (malloc) contenant TOUS les agents
    int nb_entites;      // Nombre actuel d'entitée
    int capacite_max;   // Taille allouée pour le tableau (pour ajouter des bébés chèvres)
                            // L'économie du joueur...
} monde;

#endif 