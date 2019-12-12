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

typedef enum {
    READY,
    UPLOADING
} state_t;

typedef struct {
    state_t state;
    int socket;
    FILE *recieved_file;
    ssize_t remaining_data;
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
 
    if(listen(server_socket, 5) != 0) {
        printf("Listen failed\n");
        exit(1);
    }
    listen(server_socket, 5);
 
    char message[200] = "";
    int client_socket;
    client_t clients[20];
    int max_clients = 10;
    int i;
    fd_set readfds;
    struct sockaddr_in client_address;

    for (i=0; i<max_clients; i++){
        clients[i].socket = 0;
        clients[i].state = READY;
    }
   
 
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        int maxfd = server_socket;

        for (i = 0; i<max_clients; i++){
            if (clients[i].socket > 0){
                FD_SET(clients[i].socket, &readfds);
            }

            if(clients[i].socket > maxfd){
                maxfd = clients[i].socket;
            }
        }

        int ready;
        ready = select(maxfd + 1, &readfds, NULL, NULL, NULL);

        if(FD_ISSET(server_socket, &readfds)){

            client_socket = accept(server_socket, (struct sockaddr*) &server_address, (socklen_t*)&addr_size);

            for (i = 0; i< max_clients; i++){
                if (clients[i].socket == 0){
                    clients[i].socket = client_socket;
                    break;
                }
            }
        }

        for (i = 0; i<max_clients; i++){
            if(FD_ISSET(clients[i].socket, &readfds)){
                ssize_t bytes_read = read(clients[i].socket, message, 200);
                if((bytes_read) == 0){
                    printf("socket %d disconnected\n", clients[i].socket);
                    close(clients[i].socket);
                    clients[i].socket = 0;
                } 
                else {
                        
                    if(clients[i].state == UPLOADING){
                        fwrite(message, 1, bytes_read, clients[i].recieved_file);
                        clients[i].remaining_data -= bytes_read;
                        if (clients[i].remaining_data == 0){
                            clients[i].state == READY;
                            fclose(clients[i].recieved_file);
                        }
                    }

                    else if (message[0] == 'M'){
                        for (int j = 0; j<max_clients; j++){
                            if (j == i || clients[j].socket<1){
                                continue;
                            }
                            printf("Sending to socket %d\n", clients[j].socket);
                            send(clients[j].socket, message+2, strlen(message)-2, 0);
                        }
                    }

                    else if (message[0] == 'U'){
                        size_t file_len;
                        char file_name[100];
                        sscanf(message,"%*c %s %ld", file_name, &clients[i].remaining_data);
                        printf("User wants to send file %s size: %ld\n", file_name, clients[i].remaining_data);
                        //otworzyc plik(o ile sie da)
                        
                        if ((clients[i].recieved_file=fopen(file_name, "w"))==NULL) {
                            printf ("Opening file failed\n");
                            exit(1);
                        } 
                        clients[i].state = UPLOADING;

                    }
                    
                    memset(message, 0, sizeof(message));
                } 
            }
        }
    
    }
    return 0;
}