#include <pandora/services.hpp>

#include <thread>
#include <vector>

thread_local int thread_index = -1;

MPSCQueue<std::function<void()>*> queues[128];

bool running_queues;

void consume(int q_number) {
    thread_index = q_number;
    std::function<void()>* callee;
    while (running_queues) {
        if (queues[q_number].can_dequeue()) {
            queues[q_number].dequeue(callee);
            (*callee)();
            delete callee;
        }
    }
}

void stop_queues() {
    running_queues = false;
}

std::vector<std::thread> start_queues() {
    running_queues = true;

    std::vector<std::thread> queues;
    for (int i = 0; i < 2; i += 1) {
        queues.emplace_back(std::thread([=]() {
            consume(i);
        }));
    }

    return queues;
}
