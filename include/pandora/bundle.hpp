#pragma once

#include <JSON.hpp>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "primitives/mpsc-queue.hpp"

using std::map;
using std::pair;
using std::string;
using std::vector;

struct ServiceState;

typedef void (*FuncHandleRequest)(void*, const void*, void*);
typedef void* (*FuncCreateObj)(int);
typedef void (*FuncHandleRequestFromNetwork)(void*, autojson::JSON*, autojson::JSON*);

struct ServiceCExtern {
    bool is_set = false;
    string name = "";
    FuncHandleRequest handler = nullptr;
    FuncCreateObj builder = nullptr;
    FuncHandleRequestFromNetwork network_handler = nullptr;
};

struct ServiceState {
    // running the tasks
    MPSCQueue<std::function<void()>*> queues[128];
    bool running_queues;

    void enqueue(int q_id, std::function<void()>* func) {
        queues[q_id].enqueue(func);
    }

    // handling service calls
    std::map<std::string, ServiceCExtern*> internalHandlers;
    std::map<std::string, void*> handlers;

    ServiceCExtern* EnsureInternalHandler(const std::string& name) {
        auto& handler = internalHandlers[name];
        if (handler == nullptr) {
            handler = new (ServiceCExtern)();
        }

        return handler;
    }

    // config things
    std::vector<int> get_run_ids(const std::string& name) {
        if (name == "Multiplication") {
            return {1};
        } else if (name == "Addition") {
            return {2};
        } else {
            return {0};
        }
    }
};

extern ServiceState state;

void addInternalHandler(string names, ServiceCExtern service);

extern "C" vector<pair<string, ServiceCExtern>> getInternalHandlers();

#define DeclareService(ServiceName) \
    ServiceName() = delete;         \
    explicit ServiceName(int);      \
    void handle_request(const Request&, Response*); \
    void network_handler(void* callee, autojson::JSON* j_req, autojson::JSON* j_res); \
    static ServiceName::Request from_network(autojson::JSON&); \
    static autojson::JSON to_network_response(const ServiceName::Response*)

#define DefineService(ServiceName)                                                                                                                                         \
    namespace {                                                                                                                                                            \
    void handler(ServiceName* callee, const ServiceName::Request* request, ServiceName::Response* response) {                                                              \
        callee->handle_request(*request, response);                                                                                                                        \
    }                                                                                                                                                                      \
                                                                                                                                                                           \
    ServiceName* builder(int thread_id) {                                                                                                                                  \
        return new ServiceName(thread_id);                                                                                                                                 \
    }                                                                                                                                                                      \
                                                                                                                                                                           \
    void network_handler(ServiceName* callee_, autojson::JSON* j_req, autojson::JSON* j_res) {                                                                             \
        auto callee = static_cast<ServiceName*>(callee_);                                                                                                                  \
        ServiceName::Request req = ServiceName::from_network(*j_req);                                                                                                      \
        ServiceName::Response res;                                                                                                                                         \
        callee->handle_request(req, &res);                                                                                                                                 \
        *j_res = ServiceName::to_network_response(&res);                                                                                                                   \
    }                                                                                                                                                                      \
                                                                                                                                                                           \
    void __attribute__((constructor)) init() {                                                                                                                             \
        addInternalHandler(std::string(#ServiceName),                                                                                                                      \
                           {true, std::string(#ServiceName), (FuncHandleRequest)(&handler), (FuncCreateObj)(&builder), (FuncHandleRequestFromNetwork)(&network_handler)}); \
    }                                                                                                                                                                      \
    }
