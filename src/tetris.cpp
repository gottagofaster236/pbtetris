#include "tetris.hpp"
#include <algorithm>

// Block

Block::Block(int x_, int y_): x(x_), y(y_) {}

Block Block::operator+(const Block& b) const {
    return Block(x + b.x, y + b.y);
}

// TetrisField

TetrisField::TetrisField(){
    clear();
}

bool TetrisField::in_bounds(const Block& b) {
    return 0 <= b.x && b.x < WIDTH && 0 <= b.y && b.y < HEIGHT;
}

bool TetrisField::contains_block(const Block& b) const{
    return in_bounds(b) && colors[b.x][b.y] != -1;
}

void TetrisField::operator=(const TetrisField &other){
    std::copy(&other.colors[0][0], &other.colors[WIDTH - 1][HEIGHT - 1] + 1, &colors[0][0]);
}

void TetrisField::swap(TetrisField &other){
    for(int i = 0; i < WIDTH; i++){
        for(int j = 0; j < HEIGHT; j++){
            std::swap(colors[i][j], other.colors[i][j]);
        }
    }
}

void TetrisField::clear(){
    for(int i = 0; i < WIDTH; i++){
        for(int j = 0; j < HEIGHT; j++){
            colors[i][j] = -1;
        }
    }
}

// Tetromino

Tetromino::Tetromino(const std::vector<Block> &blocks_, int color_, int rotations_count_) :
    position(TetrisField::WIDTH / 2, 0),
    color(color_),
    rotations_count(rotations_count_),
    blocks(blocks_),
    current_rotation(0)
{ }

const std::vector<Block>& Tetromino::get_blocks() const{
    return blocks;
}

void Tetromino::rotate(int direction){
    if(rotations_count == 1)
        return;
    current_rotation += direction;
    if(rotations_count == 2){  // i-tetromino
        if(current_rotation % 2 == 0)
            direction = -1;
        else
            direction = 1;
    }

    for(size_t i = 0; i < blocks.size(); i++){
        Block &b = blocks[i];
        int x = b.x, y = b.y;
        b.x = -y * direction;
        b.y = x * direction;
    }
}

bool Tetromino::intersects_with(const TetrisField& field) const{
   const std::vector<Block> &blocks = get_blocks();
   for(size_t i = 0; i < blocks.size(); i++){
       Block b = position + blocks[i];
       if(b.x < 0 || b.x >= TetrisField::WIDTH || b.y >= TetrisField::HEIGHT
               || field.contains_block(b)){
           return true;
       }
   }
   return false;
}
