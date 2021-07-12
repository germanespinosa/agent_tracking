#include <thread>
#include <atomic>
class Input_handler{
public:
    Input_handler ();
    std::string get_string();
    std::atomic<bool> ready;
    ~Input_handler();
private:
    std::atomic<bool>  running;
    std::string pipe_buffer;
    std::thread pipe_thread;
    static void pipe_process(Input_handler *);
};
