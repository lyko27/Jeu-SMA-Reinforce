#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "affichage.h"
#include "fermier.h"
#include "goat.h"
#include "loup.h"
#include "monde.h"
#include "reinforce.h"
#include "utilisateur.h"

// Calcul de la récompense instantanée pour le fermier
float calculer_recompense_fermier(monde *m)
{
    float r = -0.1f; // Pénalité de temps par tick pour forcer le mouvement
    Fermier *f = m->fermiers;
    float min_dist_wolf = 9999.0f;

    for (int i = 0; i < m->nb_wolf; i++)
    {
        Wolf *w = m->wolfs_tab[i];
        if (w)
        {
            float dx = w->x - f->x;
            float dy = w->y - f->y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist < min_dist_wolf)
            {
                min_dist_wolf = dist;
            }

            // Pénalité si le loup est trop proche d'une chèvre
            for (int j = 0; j < m->nb_goat; j++)
            {
                Goat *g = m->goats_tab[j];
                if (g)
                {
                    float dgx = g->x - w->x;
                    float dgy = g->y - w->y;
                    float dist_g = sqrtf(dgx * dgx + dgy * dgy);
                    if (dist_g < 50.0f)
                    {
                        r -= 5.0f; // Pénalité
                    }
                }
            }
        }
    }
    if (min_dist_wolf < 9999.0f)
    {
        r += (400.0f - min_dist_wolf) / 50.0f; // Gradient d'approche plus fort et portée plus longue
    }

    if (min_dist_wolf < 80.0f)
    {
        r += 15.0f; // Récompense forte pour chasser le loup
    }
    return r;
}

// Calcul de la récompense instantanée pour un loup
float calculer_recompense_loup(Wolf *w, monde *m)
{
    float r = -0.1f; // Pénalité de temps par tick pour forcer le mouvement
    float min_dist_chevre = 9999.0f;

    for (int j = 0; j < m->nb_goat; j++)
    {
        Goat *g = m->goats_tab[j];
        if (g)
        {
            float dx = g->x - w->x;
            float dy = g->y - w->y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist < min_dist_chevre)
            {
                min_dist_chevre = dist;
            }
            if (dist < 40.0f)
            {
                r += 70.0f; // Récompense pour avoir mangé/attrapé une chèvre
            }
        }
    }

    if (min_dist_chevre < 9999.0f)
    {
        r += (300.0f - min_dist_chevre) / 50.0f; // Récompense pour chasser la chèvre
    }

    Fermier *f = m->fermiers;
    if (f)
    {
        float dfx = f->x - w->x;
        float dfy = f->y - w->y;
        float dist_f = sqrtf(dfx * dfx + dfy * dfy);
        if (dist_f < 200.0f)
        {
            r -= (250.0f - dist_f) / 20.0f; // Peur du fermier
        }
        if (dist_f < 80.0f)
        {
            r -= 25.0f; // Grosse pénalité s'il se fait presque attraper
        }
    }
    return r;
}

// Nettoyage complet des ressources du monde
void nettoyer_monde(monde *m)
{
    if (m)
    {
        free_goats(m->goats_tab, m->nb_goat);
        free_wolf(m->wolfs_tab, m->nb_wolf);
        free(m->goats_tab);
        free(m->wolfs_tab);
        free(m->fermiers);
        free(m);
    }
}

// Boucle d'entraînement
void entrainer_agents()
{
    time_t begin = time( NULL );

    int nb_cycles = 5000;
    int nb_episodes = 25;
    int max_steps = 1000;
    float alpha = 0.001f;
    float gamma = 0.99f;

    // Mémoriser les poids appris d'une époque à l'autre
    Fermier *fermier_poids = malloc(sizeof(Fermier));
    init_fermier(fermier_poids);
    charger_poids_fermier(fermier_poids, "poids_fermier.txt");

    Wolf *loups_poids[3];
    for (int i = 0; i < 3; i++)
    {
        loups_poids[i] = malloc(sizeof(Wolf));
        init_wolf(loups_poids[i]);
        charger_poids_loup(loups_poids[i], "poids_loup.txt");
    }
    int nbre_cycles_effectués = 0;

    for (int cycle = 1; cycle <= nb_cycles; cycle++)
    {
        Trajectoire *trajectoires_fermier = malloc(nb_episodes * sizeof(Trajectoire));
        Trajectoire *trajectoires_loups = malloc(nb_episodes * 3 * sizeof(Trajectoire));

        for (int ep = 0; ep < nb_episodes; ep++)
        {
            init_trajectoire(&trajectoires_fermier[ep]);
            for (int w = 0; w < 3; w++)
            {
                init_trajectoire(&trajectoires_loups[ep * 3 + w]);
            }

            monde *m = creer_monde(LARGEUR, HAUTEUR);
            m = generer_un_monde(m);
            m->mode = 1;

            // Injection des poids
            memcpy(m->fermiers->weights, fermier_poids->weights, sizeof(fermier_poids->weights));
            for (int w = 0; w < m->nb_wolf && w < 3; w++)
            {
                memcpy(m->wolfs_tab[w]->weights, loups_poids[w]->weights, sizeof(loups_poids[w]->weights));
            }

            for (int step = 0; step < max_steps; step++)
            {
                // Extraire phi dans l'état courant
                float phi_fermier[DIMENSION_PHI_FERMIER];
                PerceptionFermier perc_fermier = calculer_perception_fermier(m->fermiers, m);
                generer_phi_fermier(perc_fermier, phi_fermier);

                float phi_loups[3][DIMENSION_PHI_WOLF];
                for (int w = 0; w < m->nb_wolf && w < 3; w++)
                {
                    PerceptionWolf perc_wolf = calculer_perception_wolf(m->wolfs_tab[w], m);
                    generer_phi_wolf(m->wolfs_tab[w], perc_wolf, phi_loups[w]);
                }

                m = mis_à_jour_monde(m, step, 0, 0);

                // Calculer les récompenses suite à l'action
                float r_fermier = calculer_recompense_fermier(m);

                float r_loups[3];
                for (int w = 0; w < m->nb_wolf && w < 3; w++)
                {
                    r_loups[w] = calculer_recompense_loup(m->wolfs_tab[w], m);
                }

                // Enregistrer l'étape dans les trajectoires correspondantes
                ajouter_transition(&trajectoires_fermier[ep], phi_fermier, m->fermiers->action_id, r_fermier);
                for (int w = 0; w < m->nb_wolf && w < 3; w++)
                {
                    ajouter_transition(&trajectoires_loups[ep * 3 + w], phi_loups[w], m->wolfs_tab[w]->action_choisi, r_loups[w]);
                }
            }
            nettoyer_monde(m);
        }

        // Appliquer reinforce
        mise_a_jour_reinforce_fermier(fermier_poids, trajectoires_fermier, nb_episodes, alpha, gamma);
        mise_a_jour_reinforce_loups(loups_poids, 3, trajectoires_loups, nb_episodes, alpha, gamma);

        // Libération de la mémoire des trajectoires
        for (int ep = 0; ep < nb_episodes; ep++)
        {
            liberer_trajectoire(&trajectoires_fermier[ep]);
            for (int w = 0; w < 3; w++)
            {
                liberer_trajectoire(&trajectoires_loups[ep * 3 + w]);
            }
        }
        free(trajectoires_fermier);
        free(trajectoires_loups);

        sauvegarder_poids_fermier(fermier_poids, "poids_fermier.txt");
        sauvegarder_poids_loup(loups_poids[0], "poids_loup.txt");
        printf("Poids sauvegardés. Récompense fermier : %f Recompense loup : %f\n", 
            calculer_recompense_fermier(fermier_poids), calculer_recompense_loup(loups_poids[0], NULL));
        nbre_cycles_effectués++;
    }

    sauvegarder_poids_fermier(fermier_poids, "poids_fermier.txt");
    sauvegarder_poids_loup(loups_poids[0], "poids_loup.txt");
    free(fermier_poids);
    for (int i = 0; i < 3; i++) free(loups_poids[i]);
    time_t end = time( NULL );
    printf("Entairnement terminé ! %d cyles effectués en %ld minutes et %ld secondes\n", nbre_cycles_effectués, ((unsigned long) difftime( end, begin ))/60, ((unsigned long) difftime( end, begin ))%60);
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    if (argc > 1 && strcmp(argv[1], "train") == 0)
    {
        entrainer_agents();
        return 0;
    }

    // Création et initialisation du monde
    monde *monde_courrant = creer_monde(LARGEUR, HAUTEUR);
    monde_courrant = generer_un_monde(monde_courrant);

    if (argc > 1 && strcmp(argv[1], "test") == 0)
    {
        monde_courrant->mode = 1;
    }

    // Charger les poids s'ils existent
    charger_poids_fermier(monde_courrant->fermiers, "poids_fermier.txt");
    for (int i = 0; i < monde_courrant->nb_wolf; i++)
    {
        charger_poids_loup(monde_courrant->wolfs_tab[i], "poids_loup.txt");
    }

    int quiiter_le_programme = 0;
    int en_pause = 0;
    int tick_animation = 0;
    init_affichage();

    while (quiiter_le_programme == 0)
    {
        interaction_utilisateur *utilisateur =
            malloc(sizeof(interaction_utilisateur));
        if (utilisateur)
        {
            utilisateur->x_deplacement = 0;
            utilisateur->y_deplacement = 0;
            utilisateur->pause = 0;
            utilisateur->quitter = 0;
            utilisateur->switch_mode = 0;
            utilisateur = recuperer_mouvement(utilisateur);
            quiiter_le_programme = utilisateur->quitter;

            if (utilisateur->pause)
            {
                en_pause = !en_pause;
            }
            if (utilisateur->switch_mode)
            {
                if (monde_courrant->mode)
                {
                    monde_courrant->mode = 0;
                }
                else
                {
                    monde_courrant->mode = 1;
                }
            }
            if (!en_pause)
            {
                tick_animation++;
                monde_courrant = mis_à_jour_monde(monde_courrant, tick_animation, utilisateur->x_deplacement, utilisateur->y_deplacement);
            }
            afficher_monde(monde_courrant);
            free(utilisateur);
        }
        SDL_Delay(16);
    }

    quitter_affichage();
    nettoyer_monde(monde_courrant);
    return 0;
}