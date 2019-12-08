#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define PORT 9000
#define BUFF 512

typedef struct {
    int socket;
    bool isUploading;
} client_t;

int main() {
 
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
 
    struct sockaddr_in server_address;
    int len = sizeof(server_address);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;
 
    socklen_t addr_size = sizeof(server_address);

    if(bind(server_socket, (struct sockaddr*) &server_address, addr_size) != 0) {
        printf("Bind failed\n");
        exit(1);
    }
 
    listen(server_socket, 5);
 
    char message[200] = "";
    int client_socket;
    int clients[20];
    int max_clients = 10;
    int i;
    fd_set readfds;
    struct sockaddr_in client_address;

    for (i=0; i<max_clients; i++){
        clients[i] = 0;
    }
   
 
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        int maxfd = server_socket;

        for (i = 0; i<max_clients; i++){
            if (clients[i] > 0){
                FD_SET(clients[i], &readfds);
            }

            if(clients[i] > maxfd){
                maxfd = clients[i];
            }
        }

        int ready;
        ready = select(maxfd + 1, &readfds, NULL, NULL, NULL);

        if(FD_ISSET(server_socket, &readfds)){

            client_socket = accept(server_socket, (struct sockaddr*) &server_address, (socklen_t*)&addr_size);

            for (i = 0; i< max_clients; i++){
                if (clients[i] == 0){
                    clients[i] = client_socket;
                    break;
                }
            }
        }

        for (i = 0; i<max_clients; i++){
            if(FD_ISSET(clients[i], &readfds)){
                if((read(clients[i], message, 200) == 0)){
                    printf("socket %d disconnected\n", clients[i]);
                    close(clients[i]);
                    clients[i] = 0;
                } else {

                    for (int j = 0; j<max_clients; j++){
                        if (j == i || clients[j]<1){
                            continue;
                        }
                        printf("Sending to socket %d\n", clients[j]);
                        send(clients[j], message, strlen(message), 0);
                    }
                    memset(message, 0, sizeof(message));
                } 
            }
        }
    
    }
    return 0;
}