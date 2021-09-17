//
// Created by german on 11/21/19.
//

#ifndef JS_H
#define JS_H

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>

struct Joystick{
    struct Button{
        enum button_state{
            none,
            pressed,
            released
        };
        button_state read_event ();
        void record_event(button_state);
        button_state state;
        std::queue<button_state> _events;
    };
    explicit Joystick(std::string &);
    std::vector<int32_t> axes;
    std::vector<Button> buttons;

private:
    std::thread _update_thread;
    static void _update_(Joystick &);
    bool _active = true;
    int _js;
};

#endif //JS_H
