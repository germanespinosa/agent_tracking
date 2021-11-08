#include <easy_tcp.h>

struct Agent_tracking_service : easy_tcp::Service{
    void on_connect() override;
    void on_disconnect() override;
    void on_incoming_data(const std::string &) override;
    static int get_port();
    int consumer_id = -1;
};