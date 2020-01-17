#pragma once

#include <cxxabi.h>
#include <functional>
#include <iostream>
#include <random>

#include "bundle.hpp"
#include "primitives/future.hpp"
#include "primitives/mpsc-queue.hpp"

template <typename T>
struct TypeName {
    static std::string Get() {
        static std::string ret_ans;
        static bool ok = true;
        if (ok) {
            ok = false;
            char* name = nullptr;
            int status;
            name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
            std::string ans = (name != nullptr) ? name : typeid(T).name();
            free(name);
            std::cerr << "Template Name: \"" << ans << "\"\n";
            ret_ans = ans;
        }
        return ret_ans;
    }
};

// running on current thread
extern thread_local int thread_index;

template <typename T>
T* get_thread_worker() {
    static thread_local bool searched_worker = false;
    static thread_local T* worker = nullptr;

    if (not searched_worker) {
        searched_worker = true;
        bool found = false;

        static auto run_ids = state.get_run_ids(TypeName<T>::Get());

        for (auto& itr : run_ids) {
            if (itr == thread_index) {
                found = true;
            }
        }

        if (found) {
            ServiceCExtern* sce = state.EnsureInternalHandler(TypeName<T>::Get());
            if (sce->is_set) {
                auto builder = reinterpret_cast<T* (*)(int)>(sce->builder);
                worker = builder(thread_index);
                std::cerr << "Service loaded!\t" << TypeName<T>::Get() << '\n';
            } else {
                std::cerr << "Service not loaded!\t" << TypeName<T>::Get() << '\n';
            }
        }
    }

    return worker;
}

template <typename T, typename Response = typename T::Response, typename Request = typename T::Request>
void make_blocking_rpc_call(const Request& request, Response* response) {
    auto worker = get_thread_worker<T>();
    if (worker == nullptr) {
        // raise error here
        std::cerr << "Error! Can't call blocking since no worker lives on this thread" << '\n';
    } else {
        worker->handle_request(request, response);
    }
}

template <typename T, typename Response = typename T::Response, typename Request = typename T::Request>
Future<Response> make_async_rpc_call(const Request& request) {
    //    static thread_local std::random_device rd;  //Will be used to obtain a seed for the random number engine
    //    static thread_local std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    //    static thread_local std::uniform_int_distribution<> dis(0, (int)T::run_ids.size() - 1);
    //    int worker_index = dis(gen);

    Promise<Response> promise;
    Future<Response> r = promise.get_future();

    static auto run_ids = state.get_run_ids(TypeName<T>::Get());
    static auto handler = reinterpret_cast<void (*)(T*, const Request*, Response*)>(state.EnsureInternalHandler(TypeName<T>::Get())->handler);

    state.enqueue(run_ids[0], new std::function<void()>([=]() {
                      Response r;
                      handler(get_thread_worker<T>(), &request, &r);
                      promise.set_value(r);
                  }));

    return std::move(r);
}
