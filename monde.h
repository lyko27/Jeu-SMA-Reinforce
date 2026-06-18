#ifndef MONDE_H
#define MONDE_H


// Le Monde 
typedef struct {
    int largeur;
    int hauteur;
    Fermier * fermiers;
    Goat ** goats_tab;
    int capacite_max_goat;   // Taille allouée pour le tableau (pour ajouter des bébés chèvres)
    int nb_goat ;      // Nombre actuel de chèvres
                            // L'économie du joueur...
} monde;

#endif 