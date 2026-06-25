/**
 * @file fermier.c
 * @brief Implémentation des fonctions de décision et d'évaluation pour l'agent Fermier.
 * @details Gère la prise de décision manuelle (clavier) et l'évaluation des intérêts (par renforcement) du fermier.
 */

#include "fermier.h"
#include "reinforce.h"

/**
 * @brief Décide de l'action de déplacement manuel du fermier à partir des entrées utilisateur.
 * @details Cette fonction est appelée en mode manuel. Elle récupère les inputs de direction (x et y)
 *          depuis la perception et les stockes comme la direction choisie par le fermier.
 * @param[in,out] fermier la structure de l'agent Fermier à modifier.
 * @param[in] perception_fermier Perception contenant les entrées utilisateur (clavier).
 * @return ActionFermier L'action de déplacement calculée dx et dy
 */
ActionFermier decider_action_fermier(Fermier *fermier, PerceptionFermier perception_fermier)
{
    ActionFermier action;
    
    // déplacements voulus saisis par l'utilisateur 
    action.dx = perception_fermier.input_x;
    action.dy = perception_fermier.input_y;
    
    // direction choisie dans la structure du fermier
    fermier->dirrection_choisi = action;
    
    return action;
}

/**
 * @brief Évalue les intérêts de chaque action possible du fermier à l'aide de sa politique apprise.
 * @details Cette fonction calcule la valeur d'intérêt (logit) pour chacune des 8 actions de mouvement.
 *          Elle génère d'abord le vecteur de caractéristiques d'état (phi), puis effectue un produit
 *          scalaire entre ce vecteur et la matrice des poids du fermier pour chaque action. Les résultats
 *          sont stockés dans la table d'intérêts pour le tirage de la décision via Softmax.
 * @param[in,out] fermier la structure du Fermier dont on met à jour la table d'intérêts.
 * @param[in] perception_fermier Perception du fermier (positions du loup et de la chèvre la plus proche).
 */
void evaluer_interets_fermier(Fermier *fermier, PerceptionFermier perception_fermier)
{
    float phi[DIMENSION_PHI_FERMIER];
    
    // Génération de phi à partir de la perception
    generer_phi_fermier(perception_fermier, phi);
    
    // Pour chaque action possible du fermier, on calcule sa valeur d'intérêt (logit)
    for (int a = 0; a < NB_ACTIONS_FERMIER; a++)
    {
        float val = 0.0f;
        // produit scalaire poids[action][k] * caractéristiques[k]
        for (int k = 0; k < DIMENSION_PHI_FERMIER; k++)
        {
            val += fermier->weights[a][k] * phi[k];
        }
        fermier->table_interets[a] = val;
    }
}