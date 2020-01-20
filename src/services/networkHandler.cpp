#include <pandora/services/networkHandler.hpp>
#include <pandora/services.hpp>

#define PORT 8080
#define BUFFER_SIZE (1 << 14)

using autojson::JSON;

void make_async_rpc_call(JSON* request, int sock_num) {
    if (not request->exists("RPC")) {
        NetworkHandler::send(sock_num, {
                {"success", false},
                {"reason", "RPC handler not specified"}
        });
        return;
    }

    string name = (*request)["RPC"];

    auto run_ids = state.get_run_ids(name);
    auto service_handler = state.EnsureInternalHandler((*request)["RPC"]);

    if (service_handler->network_handler == nullptr) {
        NetworkHandler::send(sock_num, {
                {"success", false},
                {"reason", "RPC handler not present on system"}
        });
        return;
    }

    state.enqueue(run_ids[0], new std::function<void()>([=]() {
        autojson::JSON response;

        // std::cerr << "Request = " << ((*request)["request"]).stringify(true) << '\n';

        service_handler->network_handler(service_handler->builder(thread_index), &((*request)["request"]), &response);

        NetworkHandler::send(sock_num, {
                {"success", true},
                {"response", response}
        });
    }));
}


void NetworkHandler::handle_request(const NetworkHandler::Request& request, NetworkHandler::Response* response) {
    char buffer[BUFFER_SIZE + 1] = {0};

    int new_socket = accept4(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen, SOCK_NONBLOCK);
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
        std::cerr << "NewSocket = " << new_socket << '\n';
    }

    for (auto new_socket : sockets) {
        auto valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread == -1) {
            if (errno == EAGAIN) {
            } else {
                perror("listen");
            }
        }

        if (valread > 0) {
            // sum += valread;
            // last_time = time;
            buffer[valread - 1] = '\0';
            // handle valread!
            // cerr << valread << '\t';
            printf("Received on network '%s'\n", buffer);
            char* head = buffer;
            auto j_stack = JSON::parse(head);
            // std::cerr << j_stack.stringify(false) << '\n';

            auto j = new JSON(std::move(j_stack));
            make_async_rpc_call(j, new_socket);
            // send(new_socket, hello, strlen(hello), 0);
            // printf("Hello message sent\n");
        }
    }

    make_async_rpc_call<NetworkHandler>({});
}

NetworkHandler::NetworkHandler(int) {
    int opt = 1;
    addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) <= 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
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
}

namespace {
    void __attribute__((constructor)) networkInit() {
        // std::cerr << "Helloooo from networkInit?\n";
        // make_async_rpc_call<NetworkHandler>({});
    }
}

void NetworkHandler::send(int socket, const autojson::JSON& j) {
    auto j_str = j.stringify(false);
    ::send(socket, j_str.c_str(), j_str.size(), 0);
}

DefineService(NetworkHandler)
