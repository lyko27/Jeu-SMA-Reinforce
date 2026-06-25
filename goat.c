/**
 * @file goat.c
 * @brief Implémentation des fonctions liées à l'agent chèvre
 * @details Contient la logique d'évaluation des comportements et intérêts de la chèvre
 */

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "goat.h"
#include "monde.h"
#include "loup.h"

/**
 * @brief Évalue les intérêts des différentes actions pour une chèvre donnée en fonction de sa perception.
 * @details Si un loup est à moins de 150 pixels, la chèvre priorise la fuite. Sinon, elle 
 *          privilégie des comportements pacifiques (brouter, errer).
 * @param[in,out] current_goat Pointeur vers la structure de la chèvre à mettre à jour
 * @param[in] perception_goat Perception actuelle de l'environnement (distance du loup)
 */
void evaluer_interets_goat(Goat *current_goat, PerceptionGoat perception_goat)
{
    // valeurs d'intérêts par défaut
    current_goat->table_interets[ACTION_ERRER] = 0.0;
    current_goat->table_interets[ACTION_FUIR_WOLF] = 0.0;
    current_goat->table_interets[ACTION_BROUTER] = 0.0;

    // on regarde si un loup est proche
    if (perception_goat.dist_wolf_proche < 150.0)
    {
        // Un loup est proche, on priorise la fuite en fonction de la proximité
        current_goat->table_interets[ACTION_FUIR_WOLF] = 10.0 - (perception_goat.dist_wolf_proche / 15.0);
        current_goat->table_interets[ACTION_ERRER] = -10.0f; // On évite d'errer 
        current_goat->table_interets[ACTION_BROUTER] = -10.0f; // On s'arrête pas pour manger
    }
    else
    {
        // Aucun danger la chèvre peut brouter ou errer tranquillement
        current_goat->table_interets[ACTION_FUIR_WOLF] = -10.0;
        current_goat->table_interets[ACTION_ERRER] = 8.0;
        current_goat->table_interets[ACTION_BROUTER] = 10.0; // Brouter est l'intérêt prioritaire (lohique un peu organique)
    }
}