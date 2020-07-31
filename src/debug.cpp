#include "debug.hpp"
#include "inkview.h"
#include <streambuf>
#include <string>


const int x = 0;
int y = 150;
extern ifont* font;

void debug_print(std::string s){
    SetFont(font, BLACK);
    DrawString(x, y, s.c_str());
    const int inc = 30;
    SoftUpdate();
    y += inc;

    if(y >= 650){
        y = 30;
        FillArea(x, y, 137 - x, 650 - y, WHITE);
        PartialUpdate(x, y, 137 - x, 650 - y);
    }
}

class DebugBuf : public std::streambuf {
public:
    virtual int overflow(int c) {
        if(c != '\n')
            current_line += c;
        else{
            debug_print(current_line);
            current_line.clear();
        }
        return traits_type::to_int_type(c);
    }
private:
    std::string current_line;
};

DebugBuf buf;
std::ostream debug(&buf);


