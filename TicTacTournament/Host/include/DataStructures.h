//
// Created by rewin on 21.06.2020.
//

#ifndef TICTACHOST_DATASTRUCTURES_H
#define TICTACHOST_DATASTRUCTURES_H

#define O_MOVE 0
#define X_MOVE 1

#define GAME_DATA_CMD 1
#define GAME_MOVE_CMD 2
#define GAME_RESET_CMD 3

struct GameState {
    int w;
    int move;
    char *data;
};

void initGameState(GameState *state, int w);
void freeGameState(GameState *state);
char* serializeGameState(GameState *state);
void deserializeGameState(GameState *state, char *serialized, bool alloc = false);

struct GameMove {
    int x, y;
    int move;
};

char* serializeGameMove(GameMove move);
GameMove deserializeGameMove(char *serialized);

#endif //TICTACHOST_DATASTRUCTURES_H
