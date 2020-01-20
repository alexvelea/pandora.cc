#include <pandora/core.hpp>
#include <pandora/services.hpp>
#include <pandora/services/networkHandler.hpp>

#include <dlfcn.h>
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
    for (int i = 0; i < 3; i += 1) {
        queues.emplace_back(std::thread([=]() { consume(i); }));
    }

    return queues;
}

size_t last_handler = 0;

void InitPandora(int argc, char** argv) {
    easyflags::ParseEasyFlags(argc, argv);

    auto handler_list = getInternalHandlers();

    for (; last_handler < handler_list.size(); last_handler += 1) {
        auto& itr = handler_list[last_handler];
        *(state.EnsureInternalHandler(itr.first)) = itr.second;
        std::cerr << "Got default loaded handler for:" << itr.first << '\n';
    }

    for (const auto& libname : defaultLoadedServices) {
        LoadService::Response response;
        loadServiceInternal({"./", libname}, &response);
        if (response.success == false) {
            std::cerr << response.error << '\n';
        }
    }

    std::cerr << "Listing on network for RPC calls\n";
    make_async_rpc_call<NetworkHandler>({});
}

void loadServiceInternal(const LoadService::Request& request, LoadService::Response* response) {
    void* handle;
    vector<pair<string, ServiceCExtern>> (*get_internal_handlers)();

    string lib_name = request.path + "lib" + request.name + ".so";

    if (state.handlers[lib_name]) {
        dlclose(state.handlers[lib_name]);
        state.handlers[lib_name] = nullptr;
    }

    handle = dlopen(lib_name.c_str(), RTLD_LAZY);
    state.handlers[lib_name] = handle;

    if (!handle) {
        /* fail to load the library */
        response->success = false;
        response->error = "failed to load" + string(dlerror());
        return;
    }

    *(void**)(&get_internal_handlers) = dlsym(handle, "getInternalHandlers");
    if (!get_internal_handlers) {
        /* no such symbol */
        response->success = false;
        response->error = "failed to get symbol" + string(dlerror());
        dlclose(handle);
        return;
    }

    response->success = true;

    auto handler_list = get_internal_handlers();
    for (; last_handler < handler_list.size(); last_handler += 1) {
        auto& itr = handler_list[last_handler];
        *(state.EnsureInternalHandler(itr.first)) = itr.second;
        response->loaded_handlers.emplace_back(itr.first);
        std::cerr << "Got handler for:" << itr.first << '\n';
    }
}