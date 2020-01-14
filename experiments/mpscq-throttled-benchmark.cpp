#include <iostream>
#include <future>
#include <atomic>
#include "mpsc-queue.hpp"

using std::cerr;
using std::atomic;

int kNumIters;

bool IsPrime(int x) {
    return true;
}

mpsc_queue_t<int> q;
atomic<bool> set;

void consume_is_prime() {
    int x;
    int remaining = kNumIters;
    while (remaining) {
        if (q.can_dequeue()) {
            q.dequeue(x);
            set = true;
            remaining -= 1;
        }
    }
}

void Enqueue(int x) {
    q.enqueue(x);
}


int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: ./exe num_requests\n";
        return 0;
    }

    kNumIters = atoi(argv[1]);

    std::thread consumer(consume_is_prime);

    set = true;
    for (int i = 0; i < kNumIters; i += 1) {
        while (set == false) ;
        set = false;
        Enqueue(i % 10);
    }

    consumer.join();
    return 0;
}