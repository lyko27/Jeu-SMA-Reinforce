/**
 * @file maitre_du_jeu.c
 * @brief Fichier principal gérant la boucle de jeu et le processus d'entraînement
 * @details Gère l'entraînement des politiques des agents fermier et loup en multi-threading ou simple cœur,
 * les fonctions de récompense, le nettoyage du monde et la boucle d'interaction utilisateur SDL2
 */

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

// Pour utiliser les fonctions plus bas dans le multithreading (!!!!! ne pas supprimer)
float calculer_recompense_fermier(monde *monde_courant);
float calculer_recompense_wolf(Wolf *wolf, monde *monde_courant);
void nettoyer_monde(monde *monde_courant);

// Multi threading pour l'entrainement

typedef struct thread_arg
{
    int episode_id; ///< le numéro de l'épisode à simuler
    Trajectoire *trajectoires_fermier;
    Trajectoire *trajectoires_loups;
    Fermier *fermier_poids;
    Wolf **loups_poids;
    int max_steps;
} EpisodeThreads;

// Calcul de récompense Reinforce

/**
 * @brief Lance la simulation d'un épisode complet pour l'apprentissage par renforcement
 * @details Initialise les trajectoires pour le fermier et les loups, crée une instance locale du monde,
 * copie les poids actuels des réseaux d'apprentissage, puis exécute les étapes de simulation
 * Enregistre les transitions (caractéristiques, actions, récompenses) pour chaque pas de temps
 * @param[in,out] parametres_episode Pointeur vers la structure contenant les arguments du thread
 * @return 0 après exécution complète
 */
int lancer_un_episode(void *parametres_episode) // Voir 5.4 site du projet
{
    EpisodeThreads *arguments = (EpisodeThreads *)parametres_episode;
    int episode_id = arguments->episode_id;
    init_trajectoire(&arguments->trajectoires_fermier[episode_id]);
    for (int index_wolf = 0; index_wolf < NB_LOUPS; index_wolf++)
    {
        init_trajectoire(&arguments->trajectoires_loups[episode_id * NB_LOUPS + index_wolf]);
    }

    monde *monde_local = creer_monde(LARGEUR, HAUTEUR);
    monde_local = generer_un_monde(monde_local);
    monde_local->mode = 1;

    // on copie le monde pour le thread

    /*  void * memcpy( void * restrict destination, const void * restrict source, size_t size )
    Cette fonction permet de copier un bloc de mémoire spécifié par le paramètre source
    et dont la taille est spécifiée via le paramètre size dans un nouvel emplacement
    désigné par le paramètre destination (source : https://koor.fr/C/cstring/memcpy.wp) */

    memcpy(monde_local->fermiers->weights, arguments->fermier_poids->weights, sizeof(arguments->fermier_poids->weights));
    for (int index_wolf = 0; index_wolf < monde_local->nb_wolf; index_wolf++)
    {
        memcpy(monde_local->wolfs_tab[index_wolf]->weights, arguments->loups_poids[index_wolf]->weights, sizeof(arguments->loups_poids[index_wolf]->weights));
    }

    // on simule l'épisode :
    for (int etape = 0; etape < arguments->max_steps; etape++)
    {
        float phi_fermier[DIMENSION_PHI_FERMIER];
        PerceptionFermier perception_fermier = calculer_perception_fermier(monde_local->fermiers, monde_local);
        generer_phi_fermier(perception_fermier, phi_fermier);

        float phi_loups[NB_LOUPS][DIMENSION_PHI_WOLF];
        for (int index_wolf = 0; index_wolf < monde_local->nb_wolf; index_wolf++)
        {
            PerceptionWolf perception_wolf = calculer_perception_wolf(monde_local->wolfs_tab[index_wolf], monde_local);
            generer_phi_wolf(monde_local->wolfs_tab[index_wolf], perception_wolf, phi_loups[index_wolf]);
        }
        monde_local = mis_à_jour_monde(monde_local, etape, 0, 0);

        // Calcul des récompenses
        float recompense_fermier = calculer_recompense_fermier(monde_local);
        float recompenses_wolfs[NB_LOUPS] = {0.0f};
        for (int index_wolf = 0; index_wolf < monde_local->nb_wolf; index_wolf++)
        {
            recompenses_wolfs[index_wolf] = calculer_recompense_wolf(monde_local->wolfs_tab[index_wolf], monde_local);
        }

        // on enregistre dans la trajectoire
        ajouter_transition(&arguments->trajectoires_fermier[episode_id], phi_fermier, DIMENSION_PHI_FERMIER, monde_local->fermiers->action_id, recompense_fermier);
        for (int index_wolf = 0; index_wolf < monde_local->nb_wolf; index_wolf++)
        {
            ajouter_transition(&arguments->trajectoires_loups[episode_id * NB_LOUPS + index_wolf], phi_loups[index_wolf], DIMENSION_PHI_WOLF, monde_local->wolfs_tab[index_wolf]->action_choisi, recompenses_wolfs[index_wolf]);
        }
    }
    nettoyer_monde(monde_local);
    return 0;
}

/**
 * @brief Calcule la récompense instantanée pour l'agent Fermier
 * @details Attribue une pénalité de temps, applique des pénalités si des loups s'approchent trop des chèvres,
 *          et offre une récompense si le fermier s'approche ou attrape un loup
 * @param[in] monde_courant Pointeur vers le monde de la simulation
 * @return La valeur de la récompense calculée
 */
float calculer_recompense_fermier(monde *monde_courant)
{
    float recompense = -0.005f; // Pénalité de temps
    Fermier *fermier = monde_courant->fermiers;
    float distance_minimale_wolf = 9999.0f;

    for (int index_wolf = 0; index_wolf < monde_courant->nb_wolf; index_wolf++)
    {
        Wolf *wolf = monde_courant->wolfs_tab[index_wolf];
        if (wolf)
        {
            float difference_x = wolf->x - fermier->x;
            float difference_y = wolf->y - fermier->y;
            float distance = sqrtf(difference_x * difference_x + difference_y * difference_y);
            if (distance < distance_minimale_wolf)
            {
                distance_minimale_wolf = distance;
            }

            // Pénalité si le loup est trop proche d'une chèvre
            for (int index_goat = 0; index_goat < monde_courant->nb_goat; index_goat++)
            {
                Goat *goat = monde_courant->goats_tab[index_goat];
                if (goat)
                {
                    float distance_x_goat = goat->x - wolf->x;
                    float distance_y_goat = goat->y - wolf->y;
                    float distance_goat = sqrtf(distance_x_goat * distance_x_goat + distance_y_goat * distance_y_goat);
                    if (distance_goat < 50.0f)
                    {
                        recompense -= 0.10f; // Pénalité pour que le fermier protège la chèvre
                    }
                }
            }
        }
    }
    if (distance_minimale_wolf < 9999.0f)
    {
        recompense += (800.0f - distance_minimale_wolf) / 5000.0f; // Gradient d'approche plus fort
    }

    if (distance_minimale_wolf < 120.0f)
    {
        recompense += 1.0f; // Grosse récompense plus importante
    }
    return recompense;
}

/**
 * @brief Calcule la récompense instantanée pour un agent Wolf
 * @details Attribue une pénalité de temps, récompense l'approche et la capture de chèvres,
 * et pénalise la proximité avec le fermier de manière proportionel à la distace
 * @param[in,out] wolf Pointeur vers la structure du loup concerné
 * @param[in] monde_courant Pointeur vers le monde de la simulation
 * @return La valeur de la récompense calculée
 */
float calculer_recompense_wolf(Wolf *wolf, monde *monde_courant)
{
    float recompense = -0.005f; // Pénalité de temps
    float distance_minimale_goat = 9999.0f;

    for (int index_goat = 0; index_goat < monde_courant->nb_goat; index_goat++)
    {
        Goat *goat = monde_courant->goats_tab[index_goat];
        if (goat)
        {
            float distance_x_goat = goat->x - wolf->x;
            float distance_y_goat = goat->y - wolf->y;
            float distance = sqrtf(distance_x_goat * distance_x_goat + distance_y_goat * distance_y_goat);
            if (distance < distance_minimale_goat)
            {
                distance_minimale_goat = distance;
            }
            if (distance < 40.0f)
            {
                recompense += 0.60f; // Récompense intermédiaire pour la chèvre
            }
        }
    }

    if (distance_minimale_goat < 9999.0f)
    {
        recompense += (400.0f - distance_minimale_goat) / 4000.0f; // Gradient plus fort pour être moins timide
    }

    Fermier *fermier = monde_courant->fermiers;
    if (fermier)
    {
        float distance_x_fermier = fermier->x - wolf->x;
        float distance_y_fermier = fermier->y - wolf->y;
        float distance_fermier = sqrtf(distance_x_fermier * distance_x_fermier + distance_y_fermier * distance_y_fermier);
        if (distance_fermier < 200.0f)
        {
            recompense -= (200.0f - distance_fermier) / 200.0f; // Peur seulement s'il est plus près
        }
        if (distance_fermier < 120.0f)
        {
            recompense -= 1.50f; // S'il s'approche trop, grosse punition
        }
    }
    return recompense;
}

/**
 * @brief Libère proprement toutes les ressources mémoires allouées pour le monde
 * @param[in,out] monde_courant Pointeur vers le monde à nettoyer
 */
void nettoyer_monde(monde *monde_courant)
{
    if (monde_courant)
    {
        free_goats(monde_courant->goats_tab, monde_courant->nb_goat);
        free_wolf(monde_courant->wolfs_tab, monde_courant->nb_wolf);
        free(monde_courant->goats_tab);
        free(monde_courant->wolfs_tab);
        free(monde_courant->fermiers);
        free(monde_courant);
    }
}

/**
 * @brief Lance le processus d'entraînement des agents par l'algorithme REINFORCE
 * @details Gère les cycles d'entraînement, l'allocation des trajectoires, le chargement/sauvegarde des poids,
 * et la parallélisation en multi-threads ou l'exécution simple cœur des épisodes de simulation
 * @param[in] simple_ou_multi_coeur Indicateur du mode (1 pour simple cœur, 0 pour multi-cœurs)
 */
void entrainer_agents(int simple_ou_multi_coeur)
{
    printf("\nDémarage entrainement...\n");
    time_t temps_debut = time(NULL);
    int nombre_cycles = 10000;
    int nombre_episodes = 25;
    int nombre_etapes_max = 1000;
    float alpha = 0.00002f; // Alpha faible pour la stabilité
    float gamma = 0.99f;

    if (simple_ou_multi_coeur == 1)
        printf("\nEntraînement en cours pour %d cycles et %d épisodes par cycle en mode : Simple Coeur ! ...\n", nombre_cycles, nombre_episodes);
    else
        printf("\nEntraînement en cours pour %d cycles et %d épisodes par cycle en mode : Multi-Coeur ! ...\n", nombre_cycles, nombre_episodes);

    // Mémoriser les poids appris d'une époque à l'autre
    Fermier *poids_fermier = malloc(sizeof(Fermier));
    init_fermier(poids_fermier);
    charger_poids_fermier(poids_fermier, "poids_fermier.txt");

    Wolf *poids_wolves[NB_LOUPS];
    for (int index_wolf = 0; index_wolf < NB_LOUPS; index_wolf++)
    {
        poids_wolves[index_wolf] = malloc(sizeof(Wolf));
        init_wolf(poids_wolves[index_wolf]);
        charger_poids_loup(poids_wolves[index_wolf], "poids_loup.txt");
    }
    int cycles_effectues = 0;

    for (int index_cycle = 1; index_cycle <= nombre_cycles; index_cycle++)
    {
        Trajectoire *trajectoires_fermier = malloc(nombre_episodes * sizeof(Trajectoire));
        Trajectoire *trajectoires_loups = malloc(nombre_episodes * NB_LOUPS * sizeof(Trajectoire));

        if (simple_ou_multi_coeur == 1)
        {
            for (int index_episode = 0; index_episode < nombre_episodes; index_episode++)
            {
                EpisodeThreads arguments;
                arguments.episode_id = index_episode;
                arguments.trajectoires_fermier = trajectoires_fermier;
                arguments.trajectoires_loups = trajectoires_loups;
                arguments.fermier_poids = poids_fermier;
                arguments.loups_poids = poids_wolves;
                arguments.max_steps = nombre_etapes_max;

                lancer_un_episode(&arguments);
            }
        }
        else
        {
            int taille_groupe = 8;
            for (int index_episode = 0; index_episode < nombre_episodes; index_episode += taille_groupe)
            {
                int nombre_threads_a_lancer = taille_groupe;
                if (index_episode + nombre_threads_a_lancer > nombre_episodes)
                    nombre_threads_a_lancer = nombre_episodes - index_episode;
                thrd_t threads_handles[taille_groupe];
                EpisodeThreads arguments_threads[taille_groupe];

                // On lance les threads
                for (int i = 0; i < nombre_threads_a_lancer; i++)
                {
                    arguments_threads[i].episode_id = index_episode + i;
                    arguments_threads[i].trajectoires_fermier = trajectoires_fermier;
                    arguments_threads[i].fermier_poids = poids_fermier;
                    arguments_threads[i].trajectoires_loups = trajectoires_loups;
                    arguments_threads[i].loups_poids = poids_wolves;
                    arguments_threads[i].max_steps = nombre_etapes_max;

                    thrd_create(&threads_handles[i], lancer_un_episode, &arguments_threads[i]);
                }
                // on attend la fin de tout le groupe
                for (int i = 0; i < nombre_threads_a_lancer; i++)
                {
                    int code_retour_thread = 0;
                    thrd_join(threads_handles[i], &code_retour_thread);
                }
            }
        }

        // Appliquer reinforce
        mise_a_jour_reinforce_fermier(poids_fermier, trajectoires_fermier, nombre_episodes, alpha, gamma);
        mise_a_jour_reinforce_loups(poids_wolves, NB_LOUPS, trajectoires_loups, nombre_episodes, alpha, gamma);

        // Libération de la mémoire des trajectoires
        for (int index_episode = 0; index_episode < nombre_episodes; index_episode++)
        {
            liberer_trajectoire(&trajectoires_fermier[index_episode]);
            for (int index_wolf = 0; index_wolf < NB_LOUPS; index_wolf++)
            {
                liberer_trajectoire(&trajectoires_loups[index_episode * NB_LOUPS + index_wolf]);
            }
        }
        free(trajectoires_fermier);
        free(trajectoires_loups);

        sauvegarder_poids_fermier(poids_fermier, "poids_fermier.txt");
        sauvegarder_poids_loup(poids_wolves[0], "poids_loup.txt");
        cycles_effectues++;
    }

    sauvegarder_poids_fermier(poids_fermier, "poids_fermier.txt");
    sauvegarder_poids_loup(poids_wolves[0], "poids_loup.txt");
    free(poids_fermier);
    for (int index_wolf = 0; index_wolf < NB_LOUPS; index_wolf++)
        free(poids_wolves[index_wolf]);
    time_t temps_fin = time(NULL);
    printf("\nEntairnement terminé ! %d cyles effectués en %ld minutes et %ld secondes\n", cycles_effectues, ((unsigned long)difftime(temps_fin, temps_debut)) / 60, 
    ((unsigned long)difftime(temps_fin, temps_debut)) % 60);
}

/**
 * @brief Point d'entrée principal du programme
 * @details Parse les arguments en ligne de commande pour lancer soit l'entraînement des agents, soit le mode démonstration ou test interactif avec affichage graphique SDL2
 * @param[in] argc Nombre d'arguments
 * @param[in] argv Tableau de chaînes de caractères contenant les arguments
 * @return Code de statut de sortie (0 pour succès)
 */
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
    monde *monde_courant = creer_monde(LARGEUR, HAUTEUR);
    monde_courant = generer_un_monde(monde_courant);

    if (argc > 1 && strcmp(argv[1], "test") == 0)
    {
        monde_courant->mode = 1;
    }

    // Charger les poids s'ils existent
    charger_poids_fermier(monde_courant->fermiers, "poids_fermier.txt");
    for (int index_wolf = 0; index_wolf < monde_courant->nb_wolf; index_wolf++)
    {
        charger_poids_loup(monde_courant->wolfs_tab[index_wolf], "poids_loup.txt");
    }

    int quitter_programme = 0;
    int indicateur_pause = 1;
    monde_courant->en_pause = 1;
    int tick_animation = 0;
    init_affichage();

    while (quitter_programme == 0)
    {
        interaction_utilisateur *interaction = malloc(sizeof(interaction_utilisateur));
        if (interaction)
        {
            interaction->x_deplacement = 0;
            interaction->y_deplacement = 0;
            interaction->pause = 0;
            interaction->quitter = 0;
            interaction->switch_mode = 0;
            interaction = recuperer_mouvement(interaction);
            quitter_programme = interaction->quitter;

            if (interaction->pause)
            {
                indicateur_pause = !indicateur_pause;
                monde_courant->en_pause = !monde_courant->en_pause;
            }
            if (interaction->switch_mode)
            {
                if (monde_courant->mode)
                {
                    monde_courant->mode = 0;
                }
                else
                {
                    monde_courant->mode = 1;
                }
            }
            if (!indicateur_pause)
            {
                tick_animation++;
                monde_courant = mis_à_jour_monde(monde_courant, tick_animation, interaction->x_deplacement, interaction->y_deplacement);
            }

            afficher_monde(monde_courant);
            free(interaction);
        }
        SDL_Delay(16);
    }

    quitter_affichage();
    nettoyer_monde(monde_courant);
    return 0;
}