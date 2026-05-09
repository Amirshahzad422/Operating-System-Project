CC = cc
CFLAGS = -Wall -Wextra -Werror -Wno-deprecated-declarations -std=c11 -O2 -pthread

SRC = main.c types.c parking.c traffic.c controller.c logger.c
OBJ = $(SRC:.c=.o)
GUI_SRC = gui.c
GUI_OBJ = $(GUI_SRC:.c=.gui.o)
GUI_CORE_OBJ = $(SRC:.c=.gui.o)
SDL_CFLAGS = $(shell pkg-config --cflags sdl2 2>/dev/null)
SDL_LIBS = $(shell pkg-config --libs sdl2 2>/dev/null)
SDL_TTF_CFLAGS = $(shell pkg-config --cflags SDL2_ttf 2>/dev/null)
SDL_TTF_LIBS = $(shell pkg-config --libs SDL2_ttf 2>/dev/null)

ifeq ($(strip $(SDL_TTF_LIBS)),)
TTF_DEFS =
else
TTF_DEFS = -DUSE_SDL_TTF
endif

TARGET = os_project_sim
GUI_TARGET = os_project_sim_gui

.PHONY: all clean run run-gui

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(GUI_TARGET): $(GUI_CORE_OBJ) $(GUI_OBJ)
	$(CC) $(CFLAGS) -DUSE_SDL2 $(TTF_DEFS) $(SDL_CFLAGS) $(SDL_TTF_CFLAGS) -o $(GUI_TARGET) $(GUI_CORE_OBJ) $(GUI_OBJ) $(SDL_LIBS) $(SDL_TTF_LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.gui.o: %.c
	$(CC) $(CFLAGS) -DUSE_SDL2 $(TTF_DEFS) $(SDL_CFLAGS) $(SDL_TTF_CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) 15

run-gui: $(GUI_TARGET)
	./$(GUI_TARGET) 15 --gui

clean:
	rm -f $(OBJ) $(GUI_CORE_OBJ) $(GUI_OBJ) $(TARGET) $(GUI_TARGET)
