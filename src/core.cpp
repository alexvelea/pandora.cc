#include <pandora/core.hpp>
#include <pandora/bundle.hpp>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <thread>
#include <vector>


#include <EasyFlags.hpp>
AddArgument(std::vector<std::string>, defaultLoadedServices).Name("load_on_start")
        .Description("Load specified services on startup");
thread_local int thread_index = -1;

ServiceState state;

void consume(int q_number) {
    thread_index = q_number;
    std::function<void()>* callee;
    while (state.running_queues) {
        if (state.queues[q_number].can_dequeue()) {
            state.queues[q_number].dequeue(callee);
            (*callee)();
            delete callee;
        }
    }
}

void stop_queues() {
    state.running_queues = false;
}

std::vector<std::thread> start_queues() {
    state.running_queues = true;

    std::vector<std::thread> queues;
    for (int i = 0; i < 2; i += 1) {
        queues.emplace_back(std::thread([=]() { consume(i); }));
    }

    return queues;
}

void InitPandora(int argc, char** argv) {
    easyflags::ParseEasyFlags(argc, argv);
    //    get_state = &get_state_internal;
    void* handle;
    vector<pair<string, ServiceCExtern>> (*get_internal_handlers)(void);

    size_t last_handler = 0;

    for (const auto& service_name : defaultLoadedServices) {
        handle = dlopen(("./lib" + service_name + ".so").c_str(), RTLD_LAZY);

        if (!handle) {
            /* fail to load the library */
            fprintf(stderr, "Error (failed to load): %s\n", dlerror());
            exit(EXIT_FAILURE);
        }

        *(void**)(&get_internal_handlers) = dlsym(handle, "getInternalHandlers");
        if (!get_internal_handlers) {
            /* no such symbol */
            fprintf(stderr, "Error (failed to get symbol): %s\n", dlerror());
            dlclose(handle);
            exit(EXIT_FAILURE);
        }

        auto handler_list = get_internal_handlers();
        for (; last_handler < handler_list.size(); last_handler += 1) {
            auto& itr = handler_list[last_handler];
            *(state.EnsureInternalHandler(itr.first)) = itr.second;
            std::cerr << "Got handler for:" << itr.first << '\n';
        }
    }

    auto handler_list = getInternalHandlers();
    for (; last_handler < handler_list.size(); last_handler += 1) {
        auto& itr = handler_list[last_handler];
        *(state.EnsureInternalHandler(itr.first)) = itr.second;
        std::cerr << "Got handler for:" << itr.first << '\n';
    }
}