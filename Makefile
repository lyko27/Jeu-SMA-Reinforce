# --- Variables de compilation ---
CC = gcc
EXE =

# Détection de l'OS pour la compatibilité Windows/Mac/Linux
ifeq ($(OS),Windows_NT)
    EXE = .exe
    RM = del /Q
    RMDIR = rmdir /S /Q
else
    EXE =
    RM = rm -f
    RMDIR = rm -rf
endif

# Nom de l'exécutable final
EXEC = farmer$(EXE)

# Fichiers sources et objets
SRC = maitre_du_jeu.c goat.c utilisateur.c affichage.c fermier.c loup.c monde.c reinforce.c monde_goat.c monde_wolf.c monde_fermier.c
OBJ = $(SRC:.c=.o)

CFLAGS_COMMON = -Wall -Wextra -pthread

SDL_CFLAGS = $(shell sdl2-config --cflags) $(shell sdl2-config --cflags | sed 's/include\/SDL2/include/g')
SDL_LIBS = $(shell sdl2-config --libs) -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lm

# Mode par défaut : développement avec symboles de débogage
CFLAGS ?= $(CFLAGS_COMMON) $(SDL_CFLAGS) -g -O0
LDLIBS ?= -pthread $(SDL_LIBS)

# Règle par défaut
all: $(EXEC)

# Règle pour lier les fichiers objets et générer l'exécutable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDLIBS)

# Règle générique de compilation (.c -> .o)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Mode Débogage avec Sanitizers (AddressSanitizer + UndefinedBehaviorSanitizer)
debug: CFLAGS = $(CFLAGS_COMMON) $(SDL_CFLAGS) -g -O0 -fsanitize=address,undefined
debug: LDLIBS = -pthread $(SDL_LIBS) -fsanitize=address,undefined
debug: clean $(EXEC)

# Mode Optimisé pour l'entraînement intensif REINFORCE
opt: CFLAGS = $(CFLAGS_COMMON) $(SDL_CFLAGS) -O3 -funroll-loops
opt: LDLIBS = -pthread $(SDL_LIBS)
opt: clean $(EXEC)

# Génération de la documentation Doxygen
doc:
	doxygen Doxyfile

# Nettoyage des fichiers objets, exécutables et répertoires de débogage
clean:
	$(RM) $(OBJ) $(EXEC)
	$(RMDIR) $(EXEC).dSYM

# Aide interactive
help:
	@echo "Cibles disponibles dans ce Makefile :"
	@echo "  make        : Compile le jeu farmer (mode standard, -g -O0)"
	@echo "  make debug  : Compile avec sanitizers (-fsanitize=address,undefined)"
	@echo "  make opt    : Compile avec optimisations maximales (-O3 pour entrainement)"
	@echo "  make doc    : Génère la documentation HTML/LaTeX avec Doxygen"
	@echo "  make clean  : Supprime les fichiers objets et exécutables"

.PHONY: all debug opt doc clean help
