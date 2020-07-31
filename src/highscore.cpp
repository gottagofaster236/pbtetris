#include "highscore.hpp"
#include "inkview.h"
#include <sstream>

const char *SAVE_PATH = FLASHDIR "/pbtetris-highscore.dat";

int load_high_score(){
    bool file_exists = (iv_access(SAVE_PATH, F_OK) != -1);
    if(file_exists){
        const int BUF_SIZE = 32;
        char buf[BUF_SIZE];

        FILE *fp = iv_fopen(SAVE_PATH, "r");
        iv_fgets(buf, BUF_SIZE, fp);
        iv_fclose(fp);

        return atoi(buf);
    }
    else{
        return 0;
    }
}

void save_high_score(int new_high_score){
    std::ostringstream oss;
    oss << new_high_score;
    std::string score = oss.str();

    FILE *fp = iv_fopen(SAVE_PATH, "w");
    iv_fwrite(score.c_str(), sizeof(char), score.size(), fp);
    iv_fclose(fp);
}
