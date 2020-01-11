#include <iostream>
#include <future>
#include "mpsc-queue.hpp"

using std::cerr;


const int kNumIters = 1e6;

bool IsPrime(int x) {
    for (int i = 2; i < x; i += 1) {
        if (x % i == 0) {
            return false;
        }
    }

    return true;
}

struct reqres {
    int request;
    std::promise<bool> response;
};

mpsc_queue_t<reqres*> q;

void consume_is_prime() {
    reqres* x;
    int remaining = kNumIters;
    while (remaining) {
        if (q.can_dequeue()) {
            q.dequeue(x);
            x->response.set_value(IsPrime(x->request));
            delete x;
            remaining -= 1;
        }
    }
}

std::future<bool> Enqueue(int x) {
    reqres* r = new reqres;
    r->request = x;
    auto f = r->response.get_future();
    q.enqueue(r);
    return std::move(f);
}

int main() {
    std::thread consumer(consume_is_prime);
    int ans = 0;
    for (int i = 0; i < kNumIters; i += 1) {
        ans += Enqueue(i % 10).get();
    }

    consumer.join();
    return 0;
}