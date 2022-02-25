#pragma once
#include <cell_world.h>
#include <tcp_messages.h>
#include <agent_tracking/tracking_client.h>

namespace agent_tracking {
    struct Tracking_service : tcp_messages::Message_service {
        Routes(
                Allow_subscription();
        )

        //unrouted
        void unrouted_message(const tcp_messages::Message &) override;

        //connection events
        void on_connect() override;
        void on_disconnect() override;

        //static
        static int get_port();
    };

    struct Tracking_server : tcp_messages::Message_server<Tracking_service>{
        void send_step(const cell_world::Step &);

        template< typename T, typename... Ts>
        T &create_local_client(Ts... vs){
            static_assert(std::is_base_of<Tracking_client, T>::value, "T must inherit from Tracking_client");
            auto new_local_client = new T{ vs... };
            local_clients.push_back((Tracking_client *) new_local_client);
            new_local_client->local_server = this;
            return *new_local_client;
        }

        bool subscribe_local( Tracking_client *client) {
            subscribed_local_clients.push_back(client);
            return true;
        }

        bool unsubscribe_local(Tracking_client *client) {
            subscribed_local_clients.erase(std::remove(subscribed_local_clients.begin(), subscribed_local_clients.end(), client));
            return true;
        }

        bool remove_local_client(Tracking_client *client) {
            subscribed_local_clients.erase(std::remove(subscribed_local_clients.begin(), subscribed_local_clients.end(), client));
            local_clients.erase(std::remove(local_clients.begin(), local_clients.end(), client));
            delete client;
            return true;
        }

        std::vector<Tracking_client * > local_clients;
        std::vector<Tracking_client * > subscribed_local_clients;
    };
}