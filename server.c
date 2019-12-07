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

#define PORT 9000
#define BUFF 512
 
 
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
    int sockets[20];
    int max_clients = 10;
    int i;
    fd_set readfds;
    struct sockaddr_in client_address;

    for (i=0; i<max_clients; i++){

        sockets[i] = 0;
    
    }
   
 
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        int maxfd = server_socket;

        for (i = 0; i<max_clients; i++){
            if (sockets[i] > 0){
                FD_SET(sockets[i], &readfds);
            }

            if(sockets[i] > maxfd){
                maxfd = sockets[i];
            }
        }

        int ready;
        ready = select(maxfd + 1, &readfds, NULL, NULL, NULL);

        if(FD_ISSET(server_socket, &readfds)){

            client_socket = accept(server_socket, (struct sockaddr*) &server_address, (socklen_t*)&addr_size);

            for (i = 0; i< max_clients; i++){
                if (sockets[i] == 0){
                    sockets[i] = client_socket;
                    break;
                }
            }
        }

        for (i = 0; i<max_clients; i++){
            if(FD_ISSET(sockets[i], &readfds)){
                if((read(sockets[i], message, 200) == 0)){
                    close(sockets[i]);
                    sockets[i] = 0;
                } else {
                    for (int j = 0; j<max_clients; j++){
                        if (j == i || sockets[j]<1){
                            continue;
                        }
                        printf("Sending to socket %d\n", sockets[j]);
                        send(sockets[j], message, strlen(message), 0);
                    }
                } 
            }
        }
    
    }
    return 0;
}