//Allows a user to establish a TCP connection with a partner and send values over the connection
#include "../include/ptcp.h"

int sock = 0;
struct sockaddr_in serv_addr;
char buffer[32] = {0};

int tcp_connect() {
 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nTCP: Socket creation error \n");
        return -1;
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, IP_ADDR, &serv_addr.sin_addr)<=0) 
    {
        printf("\nTCP: Invalid address/ Address not supported \n");
        return -1;
    }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nTCP: Connection Failed \n");
        return -1;
    }

    return 0;
}


int send_values(motor_data md) {
    //TODO: remove this 8 but make sure the sizeof(struct motor_data) is correct
    if (send(sock, (void*)&md, sizeof(motor_data), 0 ) < 0 ){
        return -1;
    }
    return 0;
}

int print_rcv_buffer() {
    if ( read( sock , &buffer, 8) < 0 ) {
        return -1;
    }
    return 0;
    printf("%s\n", buffer);
}