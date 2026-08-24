# Jeu-SMA-Reinforce

### A Multi-Agent System simulation featuring farmers, goats, and wolves, powered by Reinforcement Learning.

<p align="center">
  <img src="images/sma.gif" alt="Simulation Demo" width="700"/>
</p>

---

## Technical Stack

- **Language**: C
- **Graphics**: SDL2
- **Concurrency**: POSIX Threads
- **Build System**: Makefile
- **AI / Logic**: Reinforcement Learning algorithms
- **Documentation**: Doxygen

## Features

- **Multi-Agent System (SMA)**: Simulates autonomous entities (farmers, goats, wolves) making independent decisions in a shared ecosystem.
- **Reinforcement Learning Advantage**: Unlike hardcoded logic, agents leverage Reinforcement Learning to dynamically adapt their survival and optimization strategies. By learning from environmental feedback (rewards and penalties), they organically improve their movements, hunting, or evading behaviors over time.
- **Real-Time Rendering**: 2D graphical visualization of the world and agents' actions using the SDL2 library.
- **Parallel Training**: Multi-threading implementation (pthreads) used to run multiple training episodes in parallel, significantly accelerating the reinforcement learning process.
- **Comprehensive Documentation**: The codebase is fully documented using Doxygen, making it easy to generate HTML/LaTeX documentation and explore the project's architecture.
