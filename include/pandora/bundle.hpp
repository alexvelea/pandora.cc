#pragma once

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


struct ServiceCExtern {
    bool is_set = false;
    string name = "";
    FuncHandleRequest handler = nullptr;
    FuncCreateObj builder = nullptr;
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
            return {0};
        } else {
            return {1};
        }
    }

};

extern ServiceState state;

void addInternalHandler(string names, ServiceCExtern service);

extern "C" vector<pair<string, ServiceCExtern>> getInternalHandlers();

#define DeclareService(ServiceName) \
    ServiceName() = delete;         \
    explicit ServiceName(int);      \
    void handle_request(const Request&, Response*)

#define DefineService(ServiceName)                                                                                                                  \
    namespace {                                                                                                                                     \
    void handler(ServiceName* callee, const ServiceName::Request* request, ServiceName::Response* response) {                                       \
        callee->handle_request(*request, response);                                                                                                 \
    }                                                                                                                                               \
                                                                                                                                                    \
    ServiceName* builder(int thread_id) {                                                                                                           \
        return new ServiceName(thread_id);                                                                                                          \
    }                                                                                                                                               \
                                                                                                                                                    \
    void __attribute__((constructor)) init() {                                                                                                      \
        addInternalHandler(std::string(#ServiceName), {true, std::string(#ServiceName), (FuncHandleRequest)(&handler), (FuncCreateObj)(&builder)}); \
    }                                                                                                                                               \
    }
