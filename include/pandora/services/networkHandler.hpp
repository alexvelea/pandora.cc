#pragma once
#include "../bundle.hpp"

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <vector>
#include <JSON.hpp>

struct NetworkHandler {
    struct Request { };

    struct Response { };

    int server_fd;
    struct sockaddr_in address;
    int addrlen;

    std::vector<int> sockets;

    DeclareService(NetworkHandler);

    static void send(int, const autojson::JSON&);
};