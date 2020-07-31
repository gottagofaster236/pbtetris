#include "game.hpp"
#include "highscore.hpp"
#include <algorithm>
#include <sstream>

Game game;

Game::Game() : falling_tetromino(NULL), next_tetromino(NULL) { }

void Game::new_game(){
    score = 0;
    high_score = load_high_score();
    redraw_score();
    game_over = false;
    paused = true;

    delete falling_tetromino;
    falling_tetromino = NULL;
    delete next_tetromino;
    next_tetromino = create_random_tetromino();

    bool first_time_play = true;
    for(int i = 0; i < TetrisField::WIDTH; i++){
        for(int j = 0; j < TetrisField::HEIGHT; j++){
            if(draw_state.colors[i][j] != -1){
                first_time_play = false;
                break;
            }
        }
    }
    field.clear();
    toggle_pause();

    if(!first_time_play)
        FullUpdate();
}

void Game::toggle_pause(){
    if(paused && !game_over){
        paused = false;
        game_step_timer();
    }
    else{
        paused = true;
        ClearTimer(game_step_timer);
    }
}

bool Game::is_paused(){
    return paused;
}

void Game::one_step(){
    if(falling_tetromino == NULL){
        falling_tetromino = next_tetromino;
        next_tetromino = create_random_tetromino();
        redraw_next_tetromino();

        if(falling_tetromino->intersects_with(field))
            set_game_over();
    }
    else{
        falling_tetromino->position.y++;
        if(falling_tetromino->intersects_with(field)){
            // tetromino has fallen down
            falling_tetromino->position.y--;
            combine_tetromino_into_game_field();

            check_for_line_clear();
        }
    }

    redraw();
}

void Game::check_for_line_clear(){
    int lines_cleared = 0;
    bool cleared_one_line = false;

    do{
        cleared_one_line = false;

        for(int line = TetrisField::HEIGHT - 1; line >= 0; line--){
            bool clear = true;
            for(int x = 0; x < TetrisField::WIDTH; x++){
                if(field.colors[x][line] == -1){
                    clear = false;
                    break;
                }
            }

            if(!clear){
                continue;
            }
            else{
                lines_cleared++;
                cleared_one_line = true;
            }

            // сдвигаем всё вниз
            for(int j = line; j >= 1; j--){
                for(int i = 0; i < TetrisField::WIDTH; i++){
                    field.colors[i][j] = field.colors[i][j - 1];
                }
            }

            // верхний ряд теперь пустой
            for(int i = 0; i < TetrisField::WIDTH; i++)
                field.colors[i][0] = -1;
        }
    }
    while(cleared_one_line);

    if(lines_cleared > 0){
        switch(lines_cleared){
        case 1:
            score += 40;
            break;
        case 2:
            score += 100;
            break;
        case 3:
            score += 300;
            break;
        case 4:
            score += 1200;
            break;
        }
        redraw_score();
        redraw();
        FullUpdate();
    }
}

const int FRAME_DELAY = 250;

void Game::game_step_timer(){
    game.one_step();
    if(!game.paused && !game.game_over)
        SetHardTimer("GAME_STEP_TIMER", game_step_timer, FRAME_DELAY);
}

void Game::rotate_tetromino(int direction){
    if(falling_tetromino == NULL)
        return;
    falling_tetromino->rotate(direction);
    if(falling_tetromino->intersects_with(field)){
        falling_tetromino->rotate(-direction);
    }
}

void Game::move_tetromino(int direction){
    if(falling_tetromino == NULL)
        return;
    falling_tetromino->position.x += direction;
    if(falling_tetromino->intersects_with(field)){
        falling_tetromino->position.x -= direction;
    }
}

void Game::put_tetromino_down(){
    if(falling_tetromino == NULL)
        return;
    do{
       falling_tetromino->position.y++;
    }
    while(!falling_tetromino->intersects_with(field));

    falling_tetromino->position.y--;
    combine_tetromino_into_game_field();
    check_for_line_clear();
}

void Game::set_game_over(){
    game_over = true;
    combine_tetromino_into_game_field();
    redraw();
    ClearTimer(game_step_timer);
    Message(ICON_INFORMATION, "Game Over", "You lost!", 5000);
}

int get_block_draw_x(int x, int offset = FIELD_X){
    return BLOCK_WIDTH * x + offset;
}

int get_block_draw_y(int y, int offset = FIELD_Y){
    return BLOCK_WIDTH * y + offset;
}

void Game::redraw(){
    draw_state.swap(prev_draw_state);
    draw_state = field;
    if(falling_tetromino)
        combine_tetromino_into_field(*falling_tetromino, draw_state);

    // calculating the screen update area
    int min_x = TetrisField::WIDTH;
    int min_y = TetrisField::HEIGHT;
    int max_x = 0;
    int max_y = 0;

    for(int i = 0; i < TetrisField::WIDTH; i++){
        for(int j = 0; j < TetrisField::HEIGHT; j++){
            if(draw_state.colors[i][j] != prev_draw_state.colors[i][j]){
                Block block(i, j);
                redraw_block(block, draw_state.colors[i][j]);
                min_x = std::min(min_x, i);
                max_x = std::max(max_x, i);
                min_y = std::min(min_y, j);
                max_y = std::max(max_y, j);
            }
        }
    }

    if(min_x <= max_x){
        int x = get_block_draw_x(min_x);
        int y = get_block_draw_y(min_y);
        int width = (max_x - min_x + 1) * BLOCK_WIDTH;
        int height = (max_y - min_y + 1) * BLOCK_WIDTH;
        PartialUpdate(x, y, width, height);
        //debug << "x, y, w, h: " << x << " " << y << " " << width << " " << height << std::endl;
    }
}

extern const ibitmap block_0, block_1, block_2;

void Game::redraw_block(const Block block, int color, int offset_x, int offset_y){
    int x = get_block_draw_x(block.x, offset_x);
    int y = get_block_draw_y(block.y, offset_y);
    switch (color) {
    case -1:
        // нет блока
        FillArea(x, y, BLOCK_WIDTH, BLOCK_WIDTH, WHITE);
        break;
    case 0:
        DrawBitmap(x, y, &block_0);
        break;
    case 1:
        DrawBitmap(x, y, &block_1);
        break;
    case 2:
        DrawBitmap(x, y, &block_2);
        break;
    }
}

extern ifont* font;
const int TEXT_X = -130;

void Game::draw_background(){
    // drawing field border
    int x = FIELD_X - 3;
    int y = FIELD_Y - 3;
    int width = TetrisField::WIDTH * BLOCK_WIDTH + 6;
    int height = TetrisField::HEIGHT * BLOCK_WIDTH + 6;
    DrawRect(x, y, width, height, BLACK);
    DrawRect(x + 1, y + 1, width - 2, height - 2, BLACK);
    DrawRect(x + 2, y + 2, width - 4, height - 4, BLACK);
    // drawing "Score:"
    SetFont(font, BLACK);
    DrawString(ScreenWidth() + TEXT_X, 80, "Score:");
    SetFont(font, BLACK);
    DrawString(ScreenWidth() + TEXT_X, 160, "Highscore:");
    SetFont(font, BLACK);
    DrawString(ScreenWidth() + TEXT_X, 240, "Next piece:");
    FullUpdate();
}

void draw_score(int score, int y){
    int x = ScreenWidth() + TEXT_X;
    int width = -TEXT_X, height = 40;

    std::ostringstream oss;
    oss << score;
    FillArea(x, y, width, height, WHITE);
    SetFont(font, BLACK);
    DrawString(x, y, oss.str().c_str());
    PartialUpdate(ScreenWidth() - 137, y, 137, height);
}

void Game::redraw_score(){
    if(score > high_score){
        high_score = score;
        save_high_score(high_score);
    }

    draw_score(score, 120);
    draw_score(high_score, 200);
}

const int NEXT_TETROMINO_X = -133, NEXT_TETROMINO_Y = 280;

void Game::redraw_next_tetromino(){
    int x = ScreenWidth() + NEXT_TETROMINO_X, y = NEXT_TETROMINO_Y;
    int width = -NEXT_TETROMINO_X, height = BLOCK_WIDTH * 2;
    FillArea(x, y, width, height, WHITE);

    int draw_x;
    if(next_tetromino->rotations_count == 4)
        draw_x = x + 49;
    else
        draw_x = x + 64;

    const std::vector<Block> &blocks = next_tetromino->get_blocks();
    for(size_t i = 0; i < blocks.size(); i++){

        redraw_block(blocks[i], next_tetromino->color, draw_x, y);
    }

    PartialUpdate(x, y, width, height);
}

void Game::combine_tetromino_into_field(const Tetromino &tetromino, TetrisField &field){
    const std::vector<Block> &blocks = tetromino.get_blocks();
    for(size_t i = 0; i < blocks.size(); i++){
        Block b = blocks[i] + tetromino.position;
        if(field.in_bounds(b)){
            field.colors[b.x][b.y] = tetromino.color;
        }
    }
}

void Game::combine_tetromino_into_game_field(){
    combine_tetromino_into_field(*falling_tetromino, field);
    delete falling_tetromino;
    falling_tetromino = NULL;
}

Tetromino *create_i_tetromino(){
    Block rotation[] = {Block(-2, 0), Block(-1, 0), Block(0, 0), Block(1, 0)};
    return new Tetromino(std::vector<Block>(rotation, rotation + 4), 2, 2);
}

Tetromino *create_o_tetromino(){
    Block rotation[] = {Block(-1, 0), Block(-1, 1), Block(0, 0), Block(0, 1)};
    return new Tetromino(std::vector<Block>(rotation, rotation + 4), 2, 1);
}

Tetromino *create_t_tetromino(){
    Block rotation[] = {Block(-1, 0), Block(0, 0), Block(1, 0), Block(0, 1)};
    return new Tetromino(std::vector<Block>(rotation, rotation + 4), 2);
}

Tetromino *create_s_tetromino(){
    Block rotation[] = {Block(-1, 1), Block(0, 1), Block(0, 0), Block(1, 0)};
    return new Tetromino(std::vector<Block>(rotation, rotation + 4), 0);
}

Tetromino *create_z_tetromino(){
    Block rotation[] = {Block(-1, 0), Block(0, 0), Block(0, 1), Block(1, 1)};
    return new Tetromino(std::vector<Block>(rotation, rotation + 4), 1);
}

Tetromino *create_j_tetromino(){
    Block rotation[] = {Block(-1, 0), Block(0, 0), Block(1, 0), Block(1, 1)};
    return new Tetromino(std::vector<Block>(rotation, rotation + 4), 0);
}

Tetromino *create_l_tetromino(){
    Block rotation[] = {Block(-1, 0), Block(0, 0), Block(1, 0), Block(-1, 1)};
    return new Tetromino(std::vector<Block>(rotation, rotation + 4), 1);
}

Tetromino *Game::create_random_tetromino(){
    int index = rand() % 7;
    switch(index){
    case 0:
        return create_i_tetromino();
    case 1:
        return create_o_tetromino();
    case 2:
        return create_t_tetromino();
    case 3:
        return create_s_tetromino();
    case 4:
        return create_z_tetromino();
    case 5:
        return create_j_tetromino();
    default:
        return create_l_tetromino();
    }
}
