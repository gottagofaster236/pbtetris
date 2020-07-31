#pragma once
#include "tetris.hpp"

static const int BLOCK_WIDTH = 32, FIELD_X = 140, FIELD_Y = 4;

class Game {
public:
    Game();
    void new_game();
    void toggle_pause();
    bool is_paused();
    void draw_background();
    void rotate_tetromino(int direction);  // 1 is clockwise, -1 is counterclockwise
    void move_tetromino(int direction);  // -1 is left,  1 is right
    void put_tetromino_down();  // when user presses the down button
private:
    void one_step();
    static void game_step_timer();
    void check_for_line_clear();
    static Tetromino *create_random_tetromino();
    void set_game_over();

    void redraw();
    static void redraw_block(const Block b, int color,
                             int offset_x = FIELD_X, int offset_y = FIELD_Y);
    void redraw_score();
    void redraw_next_tetromino();
    static void combine_tetromino_into_field(const Tetromino&, TetrisField&);
    void combine_tetromino_into_game_field();

    int score, high_score;
    Tetromino *falling_tetromino, *next_tetromino;
    TetrisField field;
    TetrisField draw_state, prev_draw_state;
    bool game_over;
    bool paused;
};

extern Game game;
