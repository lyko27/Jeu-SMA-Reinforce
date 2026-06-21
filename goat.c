#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "goat.h"
#include "monde.h"
#include "loup.h"

 /**
 * Synopsis : Décide de la prochaine action de la chèvre en fonction de sa perception.
 * Entrée   : Pointeur vers la chèvre, Perception de la chèvre.
 * Sortie   : Action choisie par la chèvre.
 */
ActionGoat decider_action(Goat *g, PerceptionGoat p) {
    evaluer_interets(g, p);

    // Si l'intérêt de fuir le loup est plus fort que l'errance, on fuit
    if (g->table_interets[ACTION_FUIR_LOUP] > g->table_interets[ACTION_ERRER] &&
        g->table_interets[ACTION_FUIR_LOUP] > 0) {
        g->action_choisi = ACTION_FUIR_LOUP;
        return ACTION_FUIR_LOUP;
    }

    // Sinon, comportement par défaut : errer ou brouter
    g->timer_mouvement--;
    if (g->timer_mouvement <= 0) {
        int chance = rand() % 100;

        if (chance < 80) { // 80% de chance de s'arrêter pour brouter
            g->action_choisi = ACTION_BROUTER;
            g->timer_mouvement = 30 + (rand() % 90);
        } 
        else { // 20% de chance de commencer à marcher
            g->action_choisi = ACTION_ERRER;
            g->timer_mouvement = 30 + (rand() % 90);
            g->angle_actuel = ((float)rand()/(float)RAND_MAX) * 2.0 * 3.14;
        }
    }

    return g->action_choisi;
}

Goat * evaluer_interets(Goat *current_goat, PerceptionGoat perception) 
{
    // intérêts par défaut
    current_goat->table_interets[ACTION_ERRER] = 0.0;
    current_goat->table_interets[ACTION_FUIR_LOUP] = 0.0;

    // modif selon la Perception
    if (perception.dist_loup_proche < 150.0) 
    {
        // on fuit si le loup est très proche
        current_goat->table_interets[ACTION_FUIR_LOUP] = 10.0 - (perception.dist_loup_proche / 15.0);
        current_goat->table_interets[ACTION_ERRER] = -10.0f;
    }
    else
    {
        // sinon la chèvre chill
        current_goat->table_interets[ACTION_FUIR_LOUP] = -10.0;
        current_goat->table_interets[ACTION_ERRER] = 10.0;
    }
    return current_goat;
}

/*
    Entrée : le monde actuel et la chèvre dont on veut calculer la perception
    Sortie : la perception de la chèvre
    Synopsis : Calcule la perception d'une chèvre par rapport à son environnement.
 */
PerceptionGoat * calculer_perception_goat(monde * monde_courant,Goat * current_goat)
{
    PerceptionGoat * perception_goat = malloc(sizeof(PerceptionGoat));
    int indice_loup_proche = 0;
    int distance_loup_plus_proche = 1000;
    for (int i = 0; i<monde_courant->nb_wolf; i++)
    {
        Wolf * current_wolf = monde_courant->wolfs_tab[i];
        int distance_x = current_goat->x-current_wolf->x;
        int distance_y = current_goat->y-current_wolf->y;
        int distance = sqrt(distance_x*distance_x + distance_y*distance_y);
        if (distance < distance_loup_plus_proche)
        {
            distance_loup_plus_proche = distance;
            indice_loup_proche = i;
        }
    }
    perception_goat->dist_loup_proche = distance_loup_plus_proche;
    perception_goat->pos_x_loup = monde_courant->wolfs_tab[indice_loup_proche]->x;
    perception_goat->pos_y_loup = monde_courant->wolfs_tab[indice_loup_proche]->y;
    perception_goat->goats_tab = monde_courant->goats_tab;
    perception_goat->capacite_max_goat = monde_courant->capacite_max_goat;
    perception_goat->nb_goat = monde_courant->nb_goat;
    perception_goat->pos_x_fermier = monde_courant->fermiers->x;
    perception_goat->pos_y_fermier = monde_courant->fermiers->y;
    return perception_goat;
}