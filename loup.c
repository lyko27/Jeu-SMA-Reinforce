/**
 * @file loup.c
 * @brief Implémentation des fonctions d'évaluation pour l'agent Loup
 * @details Gère la logique comportementale (heuristique et par renforcement) des loups.
 */

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "loup.h"
#include "reinforce.h"

/**
 * @brief Évalue les intérêts des différentes actions d'un loup en utilisant des règles heuristiques simples.
 * @details Cette fonction applique des règles comportementales deterministe :
 *          Si le fermier est proche (< 200.0f), le loup fuit en priorité absolue.
 *          Si une chèvre est proche (< 300.0f), le loup la chasse.
 *          Si aucun des deux n'est proche, par défaut, le loup erre de façon aléatoire ou s'arrête.
 * @param[in,out] wolf Pointeur vers la structure du loup dont on met à jour la table d'intérêts.
 * @param[in] perception_wolf Perception actuelle contenant les distances du fermier et des chèvres.
 */
void evaluer_interets_wolf(Wolf *wolf, PerceptionWolf perception_wolf)
{
    // Calcul de la distance avec le fermier
    float dx_fermier = perception_wolf.pos_x_fermier - wolf->x;
    float dy_fermier = perception_wolf.pos_y_fermier - wolf->y;
    float dist_fermier = sqrt(dx_fermier * dx_fermier + dy_fermier * dy_fermier);

    // fuir le fermier s'il est à portée
    if (dist_fermier < 200.0f)
    {
        wolf->table_interets[ACTION_WOLF_FUIR_FERMIER] = 15.0f;
        wolf->table_interets[ACTION_WOLF_CHASSER] = -10.0f;
        wolf->table_interets[ACTION_WOLF_ERRER] = -10.0f;
        wolf->table_interets[ACTION_WOLF_ARRET] = -10.0f;
    }
    // chasser la chèvre la plus proche
    else if (perception_wolf.pos_x_goat != -1 && perception_wolf.dist_goat_proche < 300.0f)
    {
        wolf->table_interets[ACTION_WOLF_FUIR_FERMIER] = -10.0f;
        wolf->table_interets[ACTION_WOLF_CHASSER] = 15.0f - (perception_wolf.dist_goat_proche / 20.0f);
        wolf->table_interets[ACTION_WOLF_ERRER] = -2.0f;
        wolf->table_interets[ACTION_WOLF_ARRET] = -5.0f;
    }
    // sinon (exploration ou pause)
    else
    {
        wolf->table_interets[ACTION_WOLF_FUIR_FERMIER] = -10.0f;
        wolf->table_interets[ACTION_WOLF_CHASSER] = -10.0f;
        wolf->table_interets[ACTION_WOLF_ERRER] = 8.0f;
        wolf->table_interets[ACTION_WOLF_ARRET] = 5.0f;
    }
}

/**
 * @brief Évalue les intérêts des différentes actions d'un loup à l'aide de sa politique apprise (mode RL).
 * @details Calcule le score (logit) pour chacune des 4 actions à partir du vecteur de caractéristiques d'état (phi) 
 * et de la matrice des poids (weights) associée à l'agent Loup.
 * @param[in,out] wolf Pointeur vers la structure du loup à mettre à jour.
 * @param[in] perception_wolf Perception actuelle contenant les distances du fermier et des chèvres.
 */
void evaluer_interets_wolf_rl(Wolf *wolf, PerceptionWolf perception_wolf)
{
    float phi[DIMENSION_PHI_WOLF];
    
    // génération de phi
    generer_phi_wolf(wolf, perception_wolf, phi);
    
    // calcul de l'intérêt pour chaque action possible (produit scalaire)
    for (int a = 0; a < NB_ACTIONS_WOLF; a++)
    {
        float val = 0.0f;
        for (int k = 0; k < DIMENSION_PHI_WOLF; k++)
        {
            val += wolf->weights[a][k] * phi[k];
        }
        wolf->table_interets[a] = val;
    }
}