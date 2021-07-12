#include <input_handler.h>
#include <iostream>
#include <fstream>

using namespace std;

int main(){
    Input_handler input;
    ofstream f;
    f.open("/maze/controller/logs/agent_logs.txt");
    while (true) {
        if (input.ready){
            auto s = input.get_string();
            if ( s == "exit"){
                break;
            } else{
                f << s << endl << flush;
            }
        }
    }
    f.close();
    exit(0);
}