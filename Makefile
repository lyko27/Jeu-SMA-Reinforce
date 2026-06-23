CC = gcc
CFLAGS = -Wall -Wextra $(shell sdl2-config --cflags | sed 's/include\/SDL2/include/g') $(shell sdl2-config --cflags) -O3
LDFLAGS = $(shell sdl2-config --libs) -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lm -g

ifeq ($(OS),Windows_NT)
    EXE = .exe
    RM = del /Q
else
    EXE =
    RM = rm -f
endif

EXEC = farmer$(EXE)
SRC = maitre_du_jeu.c goat.c utilisateur.c affichage.c fermier.c loup.c monde.c reinforce.c monde_goat.c monde_wolf.c monde_fermier.c

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(SRC) -o $(EXEC) $(CFLAGS) $(LDFLAGS)

clean:
	$(RM) $(EXEC)
