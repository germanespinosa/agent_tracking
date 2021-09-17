#include "joystick.h"
#include <iostream>
using namespace std;

Joystick::Joystick(string &device_path) {
    _js = open(device_path.c_str(), O_RDONLY);

    __u8 count;
    if (ioctl(_js, JSIOCGAXES, &count) != -1){
        for (int i = 0; i < count; i++){
            axes.push_back(0);
        }
    }
    if (ioctl(_js, JSIOCGBUTTONS, &count) != -1){
        for (int i = 0; i < count; i++){
            buttons.push_back({Button::button_state::none, {}});
        }
    }
    _update_thread = std::thread(_update_, std::ref(*this));
}

void Joystick::_update_(Joystick &j) {
    while (j._active) {
        js_event event{0,0,0};
        ssize_t bytes = read(j._js, &event, sizeof(event));
        if (bytes == sizeof(event)){
            switch (event.type)
            {
                case JS_EVENT_BUTTON: {
                    j.buttons[event.number].record_event (event.value ? Button::pressed : Button::released);
                    break;
                }
                case JS_EVENT_AXIS: {
                    j.axes[event.number] = event.value;
                }
                default:{
                    /* Ignore init events. */
                    break;
                }
            }
        }
    }
}

void Joystick::Button::record_event(Joystick::Button::button_state s) {
    state = s;
    if (s != none){
        _events.push(s);
    }
}

Joystick::Button::button_state Joystick::Button::read_event() {
    if (_events.empty()){
        return none;
    }else {
        Joystick::Button::button_state r = _events.front();
        _events.pop();
        return r;
    }
}
