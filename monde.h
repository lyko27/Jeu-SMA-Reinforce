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
    Wolf ** wolfs_tab;
    int capacite_max_wolf;
    int nb_wolf ; 

                            // L'économie du joueur...
} monde;

void free_goats(Goat **goats_tab, int nb_goat);
void free_wolf(Wolf **wolf_tab, int nombre_wolf);
monde *creer_monde(int largeur, int hauteur);
monde *generer_un_monde(monde *monde_courant);
void afficher_monde(monde *monde_courant);
monde *mis_à_jour_monde(monde *monde_courant, int tick_animation);

#endif