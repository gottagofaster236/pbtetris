#pragma once
#include <vector>
#include "inkview.h"

struct Block {
    int x, y;
    
    Block(int x, int y);
    Block operator+(const Block&) const;
};


struct TetrisField {
    static const int WIDTH = 10, HEIGHT = 20;
    int colors[WIDTH][HEIGHT];  // -1 means no block. Else 0, 1, or 2.
    
    TetrisField();
    static bool in_bounds(const Block&);
    bool contains_block(const Block&) const;
    void operator=(const TetrisField&);
    void swap(TetrisField &);
    void clear();
};


class Tetromino {
public:
    Block position;
    int color;  // 0, 1 or 2
    const int rotations_count;
    
    Tetromino(const std::vector<Block> &blocks, int color, int rotations_count = 4);
    
    const std::vector<Block>& get_blocks() const;
    void rotate(int direction);  // 1 - clockwise, -1 - counterclockwise
    bool intersects_with(const TetrisField&) const;
private:
    std::vector<Block> blocks;
    int current_rotation;
};
