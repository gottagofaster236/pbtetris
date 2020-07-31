#include "buttons.hpp"
#include "game.hpp"
#include "inkview.h"

enum Button {
    BUTTON_LEFT, BUTTON_RIGHT, BUTTON_UP, BUTTON_DOWN, BUTTON_PAUSE, BUTTON_NEW_GAME
};

const int BUTTON_REPEAT_TIME = 250;

Button last_button_pressed;
void repeat_button_press_timer();

void on_button_press(Button button){
    last_button_pressed = button;

    switch(button){
    case BUTTON_LEFT:
        game.move_tetromino(-1);
        break;
    case BUTTON_RIGHT:
        game.move_tetromino(1);
        break;
    case BUTTON_UP:
        game.rotate_tetromino(1);
        break;
    case BUTTON_DOWN:
        game.put_tetromino_down();
        break;
    case BUTTON_PAUSE:
        game.toggle_pause();
        break;
    case BUTTON_NEW_GAME:
        game.new_game();
        break;
    }

    on_key_up();
    if(button <= BUTTON_UP)
        SetHardTimer("REPEAT_BUTTON_PRESS", repeat_button_press_timer, BUTTON_REPEAT_TIME);
}

void repeat_button_press_timer(){
    on_button_press(last_button_pressed);
    SetHardTimer("REPEAT_BUTTON_PRESS", repeat_button_press_timer, BUTTON_REPEAT_TIME);
}

void on_key_down(int key){
    switch(key){
    case KEY_LEFT:
    case KEY_PREV:
        on_button_press(BUTTON_LEFT);
        break;
    case KEY_RIGHT:
    case KEY_NEXT:
        on_button_press(BUTTON_RIGHT);
        break;
    case KEY_UP:
    case KEY_MENU:
        on_button_press(BUTTON_UP);
        break;
    case KEY_DOWN:
        on_button_press(BUTTON_DOWN);
        break;
    }
}

extern const ibitmap buttons_arrows, pause_button, new_game_button;

const int ARROWS_Y = -150;
const int MENU_Y = 10;
const int PAUSE_BUTTON_X = -132;
const int NEW_GAME_BUTTON_X = -66;

void draw_bitmap(int x, int y, const ibitmap &bmp){
    DrawBitmap(x, y, &bmp);
    PartialUpdate(x, y, bmp.width, bmp.height);
}

void draw_buttons(){
    draw_bitmap(0, ScreenHeight() + ARROWS_Y, buttons_arrows);
    draw_bitmap(ScreenWidth() + PAUSE_BUTTON_X, MENU_Y, pause_button);
    draw_bitmap(ScreenWidth() + NEW_GAME_BUTTON_X, MENU_Y, new_game_button);
}

void on_pointer_down(int x, int y){
    if(y >= ScreenHeight() + ARROWS_Y){
        // clicked one of arrows
        int button_number = x / 150;
        switch (button_number) {
        case 0:
            on_key_down(KEY_LEFT);
            break;
        case 1:
            on_key_down(KEY_RIGHT);
            break;
        case 2:
            on_key_down(KEY_UP);
            break;
        case 3:
            on_key_down(KEY_DOWN);
            break;
        }
    }
    else if(ScreenWidth() + PAUSE_BUTTON_X <= x && x < ScreenWidth() + PAUSE_BUTTON_X + pause_button.width
            && MENU_Y <= y && y < MENU_Y + pause_button.height){
        game.toggle_pause();
    }
    else if(ScreenWidth() + NEW_GAME_BUTTON_X <= x && x < ScreenWidth() + NEW_GAME_BUTTON_X + new_game_button.width
            && MENU_Y <= y && y < MENU_Y + new_game_button.height){
        game.new_game();
    }
}


void on_key_up(){
    ClearTimer(repeat_button_press_timer);
}

void on_pointer_up(){
    on_key_up();
}
