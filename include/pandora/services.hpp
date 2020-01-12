#pragma once

#include <functional>
#include <thread>
#include <vector>

#include "primitives/future.hpp"
#include "primitives/mpsc-queue.hpp"

extern MPSCQueue<std::function<void()>> queues[2];

template<typename T, typename Response = typename T::Response, typename Request = typename T::Request>
Future<Response> enqueue(const Request& request) {
    Promise<Response> promise;
    Future<Response> r = promise.get_future();

    queues[T::q_number].enqueue([=](){
        Response r;
        T::caller.call_blocking(request, r);
        promise.set_value(r);
    });

    return std::move(r);
}

// used to soft-stop the queue
extern bool running_queues;

void stop_queues();

std::vector<std::thread> start_queues();