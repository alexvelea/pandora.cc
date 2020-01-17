#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <iostream>
#include <vector>
using namespace std;

#define PORT 8080 
#define BUFFER_SIZE (1 << 16)

long long get_time_ms() {
    struct timespec tend={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tend);

    return (tend.tv_sec * 1e9 + tend.tv_nsec) / 1e6;
}

int main(int argc, char const *argv[]) { 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[BUFFER_SIZE + 1] = {0}; 
    char *hello = "Hello from server"; 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) <= 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }

    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    if (listen(server_fd, 3) < 0) { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 

    vector<int> sockets;

    long long sum = 0;
    long long last_time = get_time_ms();


    while (1) {
        new_socket = accept4(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen, SOCK_NONBLOCK);
        if (new_socket == -1) {
            if (errno == EWOULDBLOCK) {
                // cerr << "Would block\n";
            } else if (errno == EAGAIN) {
                // cerr << "Again\n";
            } else {
                perror("accept");
                exit(EXIT_FAILURE);
            }
        } else {
            sockets.push_back(new_socket);
            cerr << "NewSocket = " << new_socket << '\n';
        }

        // cerr << "cycle\n";
        // sleep(1);

        long long time = get_time_ms();
        if (time - last_time > 200 && sum) {
          cerr << "Sum     :" << sum << '\n';
          cerr << "Sum(KB) :" << sum / 1024 << '\n';
          cerr << "Sum(MB) :" << sum / 1024 / 1024 << '\n';
          sum = 0;
        }

        for (auto new_socket : sockets) {
            valread = read(new_socket , buffer, BUFFER_SIZE);
            if (valread == -1) {
              
              if (errno == EAGAIN) {
              } else {
                perror("listen");   
              }
            }
            if (valread > 0) {
              
              
              sum += valread;
              last_time = time;
              buffer[valread - 1] = '\0';
              // cerr << valread << '\t';
              // printf("%s\n",buffer);
              // send(new_socket, hello, strlen(hello), 0);
              // printf("Hello message sent\n");
            }
        }
    }
    return 0;
}