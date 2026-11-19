#include "Log.h"

class Log {
    char* TAG = "CPU";
    
    Log() {}

    void d(std::string msg) {
        printf("%c :::: %c", TAG, msg);
    }

};