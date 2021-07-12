#include <time_stamp.h>

using namespace std;

Time_stamp::Time_stamp():
        check_point(chrono::high_resolution_clock::now()) {
}

void Time_stamp::reset() {
    check_point = chrono::high_resolution_clock::now();
}

double Time_stamp::to_seconds() {
    auto time_stamp = chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration<double, std::milli>(time_stamp-check_point).count();
    return ((double) ms) / 1000;
}
