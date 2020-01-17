// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h> 
#define PORT 8080 
   
#define SEND_SIZE (1<<16)
char buffer[SEND_SIZE + 1] = {0}; 

int main(int argc, char const *argv[]) 
{ 
    int num = atoi(argv[1]);
    int got_addr = false;
    int b = 1;
    if (argc >= 3) {
        got_addr = true;
    }

    if (argc >= 4) {
        b = atoi(argv[3]);
    }

    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    char *hello = "Hello from client"; 
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if (got_addr)
        if(inet_pton(AF_INET, "0.0.0.0", &serv_addr.sin_addr) <= 0) { 
            printf("\nInvalid address/ Address not supported \n"); 
            return -1; }
    else
        if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr) <= 0) { 
            printf("\nInvalid address/ Address not supported \n"); 
            return -1; }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { 
        printf("\nConnection Failed \n"); 
        return -1; 
    }

    for (int j = 0; j < 16; j += 1) {
        for (int i = 0; i < 1024 * b; i += 1) { 
           if (send(sock, hello, num, 0) == -1) {
            perror("send");
           }
        }
        printf("Hello message sent %d/16\n", j + 1);
        // valread = read( sock , buffer, 1024);
        // printf("%s\n",buffer );
        // sleep(1);
    }
    return 0; 
} 