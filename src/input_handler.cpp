#include <input_handler.h>
#include <iostream>

using namespace std;

std::string Input_handler::get_string() {
    if (ready){
        std::string s = pipe_buffer;
        pipe_buffer = "";
        ready = false;
        return s;
    } else {
        return "";
    }
}

void Input_handler::pipe_process(Input_handler *pipe){
    while (pipe->running){
        if (!pipe->ready) {
            char a;
            std::cin.read(&a,1);
            if (a == 10) {
                pipe->ready = true;
            } else {
                if  (a!=13)
                    pipe->pipe_buffer += a;
            }
        }
    }
}

Input_handler::~Input_handler(){
    running = false;
    std::cin.clear();
    pipe_thread.join();
}

Input_handler::Input_handler():
        running(true),
        pipe_buffer(""),
        ready(false),
        pipe_thread(Input_handler::pipe_process, this){
}