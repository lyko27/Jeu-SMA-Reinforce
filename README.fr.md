[Read in English](README.md)

# Jeu-SMA-Reinforce

### Simulation Multi-Agents Proies-Prédateurs en C avec Apprentissage par Renforcement et Multi-Threading POSIX

<div align="center">

[![Langage](https://img.shields.io/badge/Langage-C99-00599C?style=flat-square&logo=c&logoColor=white)](https://fr.wikipedia.org/wiki/C99)
[![Rendu](https://img.shields.io/badge/Rendu-SDL2-red?style=flat-square&logo=sdl&logoColor=white)](https://www.libsdl.org/)
[![Concurrence](https://img.shields.io/badge/Concurrence-POSIX_Threads-333333?style=flat-square&logo=linux&logoColor=white)](https://fr.wikipedia.org/wiki/Pthreads)
[![Compilation](https://img.shields.io/badge/Compilation-GNU_Make-blue?style=flat-square&logo=gnu)](Makefile)
[![Documentation](https://img.shields.io/badge/Documentation-Doxygen-2C4156?style=flat-square)](doc/html/index.html)
[![Licence](https://img.shields.io/badge/Licence-MIT-green?style=flat-square)](LICENSE)

<p align="center">
  <img src="images/sma.gif" alt="Démo de la simulation" width="720"/>
</p>

</div>

---

## Présentation

**Jeu-SMA-Reinforce** est une simulation 2D d'un Système Multi-Agents (SMA) développée en C avec SDL2. Elle modélise un écosystème où prédateurs et protecteur développent des comportements émergents par Apprentissage par Renforcement (Policy Gradient / REINFORCE).

- **Chèvres (Proies)** : Agents réactifs qui broutent et fuient à l'approche des prédateurs.
- **Loups (Prédateurs)** : Agents autonomes apprenant à chasser les chèvres tout en esquivant le fermier.
- **Fermier (Protecteur)** : Contrôlable manuellement au clavier ou en mode autonome via une politique entraînée pour défendre le troupeau.

---

## Fonctionnalités

- **Système Multi-Agents** : Entités autonomes avec boucle perception-action décentralisée sur une carte 2D avec obstacles physiques.
- **Apprentissage par Renforcement** : Algorithme REINFORCE (Monte-Carlo Policy Gradient), sélection d'action Softmax et reward shaping.
- **Entraînement Parallélisé** : Mode sans rendu graphique accéléré par threads POSIX (`pthreads`) sur plusieurs cœurs CPU (~6x plus rapide).
- **Double Mode de Jeu** : Bascule instantanée en temps réel entre contrôle manuel et pilotage par IA autonome.
- **Rendu Temps Réel** : Pipeline visuel fluide à 60 FPS avec SDL2, feuilles de sprites, police pixel art et affichage dynamique du HUD.
- **Qualité de Code** : Gestion rigoureuse de la mémoire vérifiée avec AddressSanitizer et documentation générée via Doxygen.

---

## Fonctionnement

### Comportement des Agents

| Entité | Contrôle | Rôle |
| :--- | :--- | :--- |
| **Fermier** | Clavier manuel ou Politique RL | Intercepter les loups et protéger les chèvres |
| **Loups** | Politique RL apprise | Traquer les chèvres en évitant le fermier |
| **Chèvres** | Mouvement réactif (Flocking) | Brouter et fuir face au danger |

### Apprentissage par Renforcement

Les agents perçoivent un vecteur normalisé (distances aux cibles, obstacles et bordures) et sélectionnent leurs actions selon une politique Softmax. Les trajectoires sont mémorisées au cours des épisodes et les poids de politique sont mis à jour par montée de gradient (REINFORCE).

### Entraînement Parallèle

La commande `./farmer train -m` exécute des épisodes de simulation indépendants sans affichage graphique via un pool de 8 threads ouvriers (`pthreads`), accélérant significativement l'apprentissage.

---

## Démarrage Rapide

### Prérequis

- GCC (support C99 et POSIX)
- SDL2, SDL2_image, SDL2_ttf
- Doxygen *(optionnel)*

```bash
# Ubuntu / Debian
sudo apt-get update
sudo apt-get install -y gcc make libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev doxygen

# macOS (Homebrew)
brew install gcc make sdl2 sdl2_image sdl2_ttf doxygen
```

### Compilation

```bash
make          # Compilation standard
make debug    # Compilation avec AddressSanitizer et UndefinedBehaviorSanitizer
make opt      # Compilation optimisée pour l'entraînement (-O3)
make doc      # Génération de la documentation Doxygen
make clean    # Suppression des fichiers objets et binaires
```

### Lancement

```bash
./farmer           # Démo interactive (Fermier contrôlé au clavier par défaut)
./farmer test      # Démo autonome (Fermier piloté par la politique RL)
./farmer train -m   # Entraînement parallèle multi-threadé sans rendu
./farmer train     # Entraînement simple cœur sans rendu
```

### Contrôles en Jeu

| Touche | Action |
| :---: | :--- |
| `Espace` | Mettre en pause / Reprendre la simulation |
| `M` | Changer de mode (Contrôle manuel vs. IA autonome) |
| `Z / Q / S / D` ou `W / A / S / D` | Déplacer le fermier (mode manuel) |
| `Flèches directionnelles` | Déplacer le fermier (touches alternatives) |
| `Échap` | Quitter le jeu |

---

## Architecture du Projet

```text
Jeu-SMA-Reinforce/
├── affichage.c / .h        # Moteur de rendu SDL2, textures, HUD et textes
├── utilisateur.c / .h      # Gestion des événements et entrées utilisateur
├── maitre_du_jeu.c         # Boucle de jeu, gestion des threads et entraînement
├── monde.c / .h            # Gestion du monde, entités et détection de collisions
├── monde_fermier.c / .h    # Cinématique et calcul de perception du fermier
├── monde_goat.c / .h       # Comportement de troupeau et fuite des chèvres
├── monde_wolf.c / .h       # Logique de traque et perception des loups
├── fermier.c / .h          # Structures de données et actions du fermier
├── loup.c / .h             # Structures de données et actions des loups
├── goat.c / .h             # Structures de données et états des chèvres
├── reinforce.c / .h        # Algorithme REINFORCE et mise à jour du gradient
├── poids_fermier.txt       # Poids de politique sauvegardés pour le fermier
├── poids_loup.txt          # Poids de politique sauvegardés pour les loups
├── fonts/                  # Polices TrueType et pixel art
├── images/                 # Spritesheets, textures de map et GIF de démo
├── doc/html/               # Documentation d'API générée par Doxygen
├── Makefile                # Script de compilation
├── LICENSE                 # Licence MIT
├── README.fr.md            # Documentation en français
└── README.md               # Documentation en anglais
```

---

## Collaborateurs

Projet réalisé par :
- **Natéo Gadaix** ([lyko27](https://github.com/lyko27))
- **Nicolas Bertrand** ([nicolas-btd](https://github.com/nicolas-btd))
- **Sohail Labied** ([sohail-lbd](https://github.com/sohail-lbd))

---

## Licence

Ce projet est sous licence MIT. Consultez le fichier [LICENSE](LICENSE) pour plus de détails.
