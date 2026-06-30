# --- Variables de compilation ---
CC = gcc
EXE =

# Détection de l'OS pour la compatibilité Windows/Mac/Linux
ifeq ($(OS),Windows_NT)
    EXE = .exe
    RM = del /Q
else
    EXE =
    RM = rm -f
endif

# Nom de l'exécutable final
EXEC = farmer$(EXE)

# fichiers sources
SRC = maitre_du_jeu.c goat.c utilisateur.c affichage.c fermier.c loup.c monde.c reinforce.c monde_goat.c monde_wolf.c monde_fermier.c
OBJ = $(SRC:.c=.o)

CFLAGS_COMMON = -Wall -Wextra -pthread

SDL_CFLAGS = $(shell sdl2-config --cflags) $(shell sdl2-config --cflags | sed 's/include\/SDL2/include/g')
SDL_LIBS = $(shell sdl2-config --libs) -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lm

# dev quotidien :
# -g debogage
# -O0 pas d'optimisation (soutenance 1)
# -fsanitize=address,undefined fuite de mémoire
CFLAGS = $(CFLAGS_COMMON) $(SDL_CFLAGS) -g -O0 
LDLIBS = -pthread $(SDL_LIBS)

# Entrainement reinforce, on optimise
# -O3 optimizer
# CFLAGS = $(CFLAGS_COMMON) $(SDL_CFLAGS) -O3
# LDLIBS = -pthread $(SDL_LIBS)


# règles
all: $(EXEC)

# Règle pour lier les fichiers objets et générer l'exécutable
# $@ rle nom de la cible (ici $(EXEC))
# $^ toutes les dépendances (ici $(OBJ))
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDLIBS)

# Règle générique pour compiler chaque fichier .c en fichier objet .o
# $< représente la première dépendance (le fichier source .c)
# $@ représente la cible à générer (le fichier objet .o)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers générés (fichiers objets et exécutable)
clean:
	$(RM) $(OBJ) $(EXEC)

# ne pas créer des fichier par erreur
.PHONY: all clean

