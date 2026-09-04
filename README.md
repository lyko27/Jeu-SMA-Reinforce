[Lire en français](README.fr.md)

# Jeu-SMA-Reinforce

### Multi-Agent Predator-Prey Simulation in C with Reinforcement Learning and POSIX Multi-Threading

<div align="center">

[![Language](https://img.shields.io/badge/Language-C99-00599C?style=flat-square&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C99)
[![Graphics](https://img.shields.io/badge/Graphics-SDL2-red?style=flat-square&logo=sdl&logoColor=white)](https://www.libsdl.org/)
[![Concurrency](https://img.shields.io/badge/Concurrency-POSIX_Threads-333333?style=flat-square&logo=linux&logoColor=white)](https://en.wikipedia.org/wiki/Pthreads)
[![Build](https://img.shields.io/badge/Build-GNU_Make-blue?style=flat-square&logo=gnu)](Makefile)
[![Documentation](https://img.shields.io/badge/Documentation-Doxygen-2C4156?style=flat-square)](doc/html/index.html)
[![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)](LICENSE)

<p align="center">
  <img src="images/sma.gif" alt="Simulation Demo" width="720"/>
</p>

</div>

---

## Overview

**Jeu-SMA-Reinforce** is an interactive 2D Multi-Agent System (MAS) simulation written in C using SDL2. It models an ecosystem where predators and protectors learn emergent behaviors through Reinforcement Learning (Policy Gradient / REINFORCE).

- **Goats (Prey)**: Reactive agents that graze and flee approaching predators.
- **Wolves (Predators)**: Autonomous agents learning to hunt goats while avoiding the farmer.
- **Farmer (Protector)**: Can be controlled manually or set to autonomous mode driven by a trained policy to protect the goats.

---

## Features

- **Multi-Agent Simulation**: Autonomous entities operating with decentralized perception-action loops on a 2D grid with obstacles.
- **Reinforcement Learning**: Monte-Carlo Policy Gradient (REINFORCE) with Softmax action selection and reward shaping.
- **Multi-Threaded Training**: Headless batch training accelerated with POSIX Threads (`pthreads`) across parallel CPU cores (~6x speedup).
- **Dual Play Modes**: Seamless real-time hot-swapping between manual keyboard controls and autonomous AI policy.
- **Real-Time Graphics**: 60 FPS rendering pipeline with SDL2, sprite sheets, custom pixel fonts, and dynamic HUD stats.
- **Code Quality**: Strict memory management verified with AddressSanitizer and documented via Doxygen.

---

## How It Works

### Agent Behavior

| Entity | Control | Objective |
| :--- | :--- | :--- |
| **Farmer** | Manual keyboard or RL Policy | Intercept wolves and protect the herd |
| **Wolves** | Trained RL Policy | Hunt goats while keeping distance from the farmer |
| **Goats** | Reactive flocking | Graze peacefully and flee threats |

### Reinforcement Learning

Agents observe a normalized perception vector (distances to targets, obstacles, and boundaries) and select actions using a Softmax policy. Trajectories are recorded during episodes, and policy weights are updated using the REINFORCE gradient ascent algorithm.

### Parallel Training

Running `./farmer train -m` launches independent headless simulation episodes across a pool of 8 worker threads (`pthreads`), aggregating policy updates without graphical overhead.

---

## Quickstart

### Build

```bash
make          # Standard build
make debug    # Build with AddressSanitizer & UndefinedBehaviorSanitizer
make opt      # Optimized build for training (-O3)
make doc      # Generate Doxygen documentation
make clean    # Remove build artifacts
```

### Run

```bash
./farmer           # Interactive demo (Manual farmer control by default)
./farmer test      # Autonomous demo (Farmer controlled by RL policy)
./farmer train -m   # Fast multi-threaded headless training
./farmer train     # Single-threaded headless training
```

### Controls

| Key | Action |
| :---: | :--- |
| `Space` | Pause / Resume simulation |
| `M` | Toggle mode (Manual keyboard vs. Autonomous AI) |
| `Z / Q / S / D` or `W / A / S / D` | Move farmer (manual mode) |
| `Escape` | Quit |

---

## Project Structure

```text
Jeu-SMA-Reinforce/
├── affichage.c / .h        # SDL2 rendering engine, textures, HUD and text
├── utilisateur.c / .h      # User inputs and event processing
├── maitre_du_jeu.c         # Game loop, thread pool and training orchestrator
├── monde.c / .h            # World state, entity management and collision physics
├── monde_fermier.c / .h    # Farmer kinematics and perception logic
├── monde_goat.c / .h       # Goat flocking, grazing and avoidance behaviors
├── monde_wolf.c / .h       # Wolf tracking and hunting perception logic
├── fermier.c / .h          # Farmer data structures and actions
├── loup.c / .h             # Wolf data structures and actions
├── goat.c / .h             # Goat data structures and states
├── reinforce.c / .h        # REINFORCE algorithm and policy gradient updates
├── poids_fermier.txt       # Trained policy weights for the farmer
├── poids_loup.txt          # Trained policy weights for wolves
├── fonts/                  # Pixel art and TrueType fonts
├── images/                 # Sprites, map tiles and demo GIF
├── doc/html/               # Doxygen API documentation
├── Makefile                # Build system
├── LICENSE                 # MIT License
├── README.fr.md            # French documentation
└── README.md               # English documentation
```

---

## Collaborators

This project was developed by:
- **Natéo Gadaix** ([lyko27](https://github.com/lyko27))
- **Nicolas Bertrand** ([nicolas-btd](https://github.com/nicolas-btd))
- **Sohail Labied** ([sohail-lbd](https://github.com/sohail-lbd))

---

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
