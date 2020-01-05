#include <iostream>
#include <future>
#include "mpsc-queue.hpp"

using std::cerr;

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
    while (true) {

        if (q.can_dequeue()) {
            q.dequeue(x);
            cerr << "Checking for " << x->request << '\n';
            x->response.set_value(IsPrime(x->request));
            delete x;
            cerr << "Deleted response" << '\n';
            break;
        }
    }
}

std::future<bool> Enqueue(int x) {
    reqres* r = new reqres;
    r->request = x;
    q.enqueue(r);

    return r->response.get_future();
}

int main() {
    std::thread consumer(consume_is_prime);
    cerr << "Enqueueing" << '\n';
    auto ans = Enqueue(1e9+7);
    cerr << "Is prime?" << '\t' << IsPrime(1e9+7) << '\n';
    cerr << "Is prime?" << '\t' << IsPrime(1e9+7) << '\n';

    cerr << "waiting .... \n";
    cerr << "Again: " << ans.get() << '\n';
    consumer.join();
    return 0;
}