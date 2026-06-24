#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Compatibilité parallélisation avec mac
#ifdef __APPLE__
#include <pthread.h>
#define thrd_t pthread_t
#define thrd_create(thr, func, arg) pthread_create((thr), NULL, (void *(*)(void *))(void *)(func), (arg))
#define thrd_join(thr, res) ((void)(res), pthread_join((thr), NULL))
#else
#include <threads.h>
#endif

#include "affichage.h"
#include "fermier.h"
#include "goat.h"
#include "loup.h"
#include "monde.h"
#include "reinforce.h"
#include "utilisateur.h"

// Pour utiliser les fonction plus basse dans le multithreading (ne pas supprimer)
float calculer_recompense_fermier(monde *m);
float calculer_recompense_loup(Wolf *w, monde *m);
void nettoyer_monde(monde *m);

// Multi threading pour l'entrainment

typedef struct thread_arg {
    int episode_id; // le numéro de l'épisode à simuler
    Trajectoire *trajectoires_fermier; 
    Trajectoire *trajectoires_loups; 
    Fermier *fermier_poids;
    Wolf **loups_poids;
    int max_steps;
} EpisodeThreads;

// Calcul de récompense Reinforce

int lancer_un_episode(void * parameters) // Voir 5.4 site du projet
{
    EpisodeThreads *args = (EpisodeThreads *)parameters;
    int episode_id = args->episode_id;
    init_trajectoire(&args->trajectoires_fermier[episode_id]);
    for (int w = 0; w < 3; w++) init_trajectoire(&args->trajectoires_loups[episode_id * 3 + w]);

    monde *monde_local = creer_monde(LARGEUR, HAUTEUR);
    monde_local = generer_un_monde(monde_local);
    monde_local->mode = 1;

    // on copie le monde pour le thread

    /*  void * memcpy( void * restrict destination, const void * restrict source, size_t size );
    Cette fonction permet de copier un bloc de mémoire spécifié par le paramètre source, 
    et dont la taille est spécifiée via le paramètre size, dans un nouvel emplacement 
    désigné par le paramètre destination. source : https://koor.fr/C/cstring/memcpy.wp*/

    memcpy(monde_local->fermiers->weights, args->fermier_poids->weights, sizeof(args->fermier_poids->weights));
    for (int w = 0; w < monde_local->nb_wolf && w < 3; w++)
    {
        memcpy(monde_local->wolfs_tab[w]->weights, args->loups_poids[w]->weights, sizeof(args->loups_poids[w]->weights));
    }

    // on simule l'épisode :
    for(int step = 0; step< args->max_steps; step ++)
    { 
        float phi_fermier[DIMENSION_PHI_FERMIER];
        PerceptionFermier perception_fermier = calculer_perception_fermier(monde_local->fermiers, monde_local);
        generer_phi_fermier(perception_fermier, phi_fermier);
        float phi_loups[3][DIMENSION_PHI_WOLF];
        for (int w = 0; w < monde_local->nb_wolf && w < 3; w++)
        {
            PerceptionWolf perception_loup = calculer_perception_wolf(monde_local->wolfs_tab[w], monde_local);
            generer_phi_wolf(monde_local->wolfs_tab[w], perception_loup, phi_loups[w]);
        }
        monde_local = mis_à_jour_monde(monde_local, step, 0, 0);

        // Cacul des récompenses
        float r_fermier = calculer_recompense_fermier(monde_local);
        float r_loups[3] = {0.0f, 0.0f, 0.0f};
        for (int w = 0; w < monde_local->nb_wolf && w < 3; w++)
        {
            r_loups[w] = calculer_recompense_loup(monde_local->wolfs_tab[w], monde_local);
        }
        // on enregistre dans la trajectoire
        ajouter_transition(&args->trajectoires_fermier[episode_id], phi_fermier, monde_local->fermiers->action_id, r_fermier);
        for (int w = 0; w < monde_local->nb_wolf && w < 3; w++)
        {
            ajouter_transition(&args->trajectoires_loups[episode_id * 3 + w], phi_loups[w], monde_local->wolfs_tab[w]->action_choisi, r_loups[w]);
        }
        // printf("Récompense fermier : %f Recompense loup : %f\n", r_fermier, r_loups[0]); // (à décomenter pour afficher mais ralenti l'entrainement)
    }
    nettoyer_monde(monde_local);
    return 0;
}

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
void entrainer_agents(int simple_ou_multi_coeur)
{
    printf("\nDémarage entrainement...\n");
    time_t begin = time( NULL );
    // modifier
    int nb_cycles = 1000;
    int nb_episodes = 25;
    int max_steps = 1000;
    float alpha = 0.00001f;
    float gamma = 0.99f;

    if(simple_ou_multi_coeur == 1) printf("\nEntraînement en cours pour %d cycles et %d épisodes par cycle en mode : Simple Coeur ! ...\n", nb_cycles, nb_episodes);
    else printf("\nEntraînement en cours pour %d cycles et %d épisodes par cycle en mode : Multi-Coeur ! ...\n", nb_cycles, nb_episodes);

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

        if(simple_ou_multi_coeur == 1)
        {
            for (int ep = 0; ep < nb_episodes; ep++)
            {
                init_trajectoire(&trajectoires_fermier[ep]);
                for (int w = 0; w < 3; w++)
                {
                    init_trajectoire(&trajectoires_loups[ep * 3 + w]);
                }

                monde *m = creer_monde(LARGEUR, HAUTEUR);
                m = generer_un_monde(m);
                m->mode = 2;

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


                    float r_loups[3] = {0.0f, 0.0f , 0.0f};
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
                    // printf("Récompense fermier : %f Recompense loup : %f\n", r_fermier, r_loups[0]); // (à décomenter pour afficher mais ralenti l'entrainement)
                }
                nettoyer_monde(m);
            }
        }
        else
        {
            int taille_chunk = 8;
            for(int episode = 0; episode < nb_episodes; episode += taille_chunk)
            {
                int thread_a_lancer = taille_chunk;
                if(episode + thread_a_lancer > nb_episodes) thread_a_lancer = nb_episodes - episode;
                thrd_t thread_handle_i[taille_chunk];
                EpisodeThreads thread_args[taille_chunk];
                
                // On lance les threads

                for(int i = 0; i < thread_a_lancer;i++)
                {
                    thread_args[i].episode_id = episode + i;
                    thread_args[i].trajectoires_fermier = trajectoires_fermier;
                    thread_args[i].fermier_poids = fermier_poids;
                    thread_args[i].trajectoires_loups = trajectoires_loups;
                    thread_args[i].loups_poids = loups_poids;
                    thread_args[i].max_steps = max_steps;

                    thrd_create(&thread_handle_i[i], lancer_un_episode, &thread_args[i]);
                }
                // on attend la fin de tout le groupe
                for(int i = 0; i<thread_a_lancer;i++)
                {
                    int error_code_thread_i = 0;
                    thrd_join(thread_handle_i[i], &error_code_thread_i);
                }
            }
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
        nbre_cycles_effectués++;
    }

    sauvegarder_poids_fermier(fermier_poids, "poids_fermier.txt");
    sauvegarder_poids_loup(loups_poids[0], "poids_loup.txt");
    free(fermier_poids);
    for (int i = 0; i < 3; i++) free(loups_poids[i]);
    time_t end = time( NULL );
    printf("\nEntairnement terminé ! %d cyles effectués en %ld minutes et %ld secondes\n", nbre_cycles_effectués, ((unsigned long) difftime( end, begin ))/60, ((unsigned long) difftime( end, begin ))%60);
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    if (argc > 2 && strcmp(argv[1], "train") == 0 && strcmp(argv[2], "-m") == 0)
    {
        entrainer_agents(0);
        return 0;
    }
    else if (argc > 1 && strcmp(argv[1], "train") == 0)
    {
        entrainer_agents(1);
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
    int en_pause = 1;
    monde_courrant->en_pause=1;
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
                monde_courrant->en_pause=!monde_courrant->en_pause;
                
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