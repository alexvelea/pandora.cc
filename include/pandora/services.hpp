#pragma once

#include <future>
#include <thread>
#include <vector>

#include "primitives/mpsc-queue.hpp"

extern MPSCQueue<std::function<void()>> queues[2];

template<typename T, typename Response = typename T::Response, typename Request = typename T::Request>
std::future<Response> enqueue(const Request& request) {
    auto promise = new std::promise<Response>();
    std::future<Response> r = promise->get_future();

    queues[T::q_number].enqueue([=](){
        Response r;
        T::caller.call_blocking(request, r);
        promise->set_value(r);
        delete promise;
    });

    return std::move(r);
}

// used to soft-stop the queue
extern bool running_queues;

void stop_queues();

std::vector<std::thread> start_queues();