#pragma once

#include <iostream>
#include <functional>
#include <thread>
#include <vector>
#include <random>

#include "primitives/future.hpp"
#include "primitives/mpsc-queue.hpp"

extern MPSCQueue<std::function<void()>*> queues[128];

extern thread_local int thread_index;

template <typename T>
T* get_thread_worker() {
    static thread_local bool searched_worker = false;
    static thread_local T* worker = nullptr;

    if (not searched_worker) {
        searched_worker = true;
        bool found = false;

        for (auto& itr : T::run_ids) {
            if (itr == thread_index) {
                found = true;
            }
        }

        if (found) {
            worker = new T(thread_index);
        }
    }

    return worker;
}

template<typename T, typename Response = typename T::Response, typename Request = typename T::Request>
void make_blocking_rpc_call(const Request& request, Response* response) {
    auto worker = get_thread_worker<T>();
    if (worker == nullptr) {
        // raise error here
        std::cerr << "Error! Can't call blocking since no worker lives on this thread" << '\n';
    } else {
        worker->internal_blocking(request, response);
    }
}

template<typename T, typename Response = typename T::Response, typename Request = typename T::Request>
Future<Response> make_async_rpc_call(const Request& request) {
    static thread_local std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static thread_local std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    static thread_local std::uniform_int_distribution<> dis(0, (int)T::run_ids.size() - 1);

    int worker_index = dis(gen);

    Promise<Response> promise;
    Future<Response> r = promise.get_future();

    queues[T::run_ids[worker_index]].enqueue(new std::function<void()>([=](){
        Response r;
        get_thread_worker<T>()->internal_blocking(request, &r);
        promise.set_value(r);
    }));

    return std::move(r);
}

// used to soft-stop the queue
extern bool running_queues;

void stop_queues();

std::vector<std::thread> start_queues();