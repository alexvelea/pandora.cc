#include <iostream>
#include <future>
#include <atomic>
#include <future>
#include "mpsc-queue.hpp"

using std::cerr;
using std::promise;
using std::future;

int kNumIters;

bool IsPrime(int x) {
    return true;
}

struct reqres {
    int request;
    promise<int> response;
};

mpsc_queue_t<reqres*> q;

void consume_is_prime() {
    reqres* x;
    int remaining = kNumIters;
    while (remaining) {
        if (q.can_dequeue()) {
            q.dequeue(x);
            x->response.set_value(IsPrime(x->request) + x->request * 10);
            delete x;
            remaining -= 1;
        }
    }
}

future<int> Enqueue(int x) {
    reqres* r = new reqres;
    r->request = x;
    auto f = r->response.get_future();
    q.enqueue(r);
    return f;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: ./exe num_requests\n";
        return 0;
    }

    kNumIters = atoi(argv[1]);

    std::thread consumer(consume_is_prime);
    int ans = 0;
    for (int i = 0; i < kNumIters; i += 1) {
        ans += Enqueue(i % 10).get();
    }

    consumer.join();
    return 0;
}