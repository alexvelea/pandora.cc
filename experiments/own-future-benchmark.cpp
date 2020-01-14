#include <iostream>
#include <future>
#include <atomic>
#include "mpsc-queue.hpp"

using std::cerr;

struct FutureBase {
    std::atomic<bool> is_set;
    std::atomic<int> counter;
    bool is_main = true;
    int val;

    void check() {
        if (counter.fetch_sub(1) == 1) {
            delete this;
        }
    }
};

struct Future {
    FutureBase* f;

    Future(FutureBase* f) : f(f) {
        f->counter++;
    }

    Future(const Future& f) : f(f.f) {
        this->f->counter++;
    }

    Future(Future&& f) : f(f.f) {
        f.f = nullptr;
    }

    Future& operator=(Future&&) = delete;
    Future& operator=(const Future&) = delete;

    ~Future() {
        if (f) {
            f->check();
        }
    }

    int get() {
        while (f->is_set == false) {
            continue;
        }
        return f->val;
    }
};

struct Promise {
    FutureBase* f;

    Promise() {
        f = new FutureBase();
        f->counter++;
    }

    Promise(const Promise& p) {
        p.f->counter++;
        f = p.f;
    }

    Promise(Promise&& p) {
        f = p.f;
        p.f = nullptr;
    }

    Promise& operator=(const Promise&) = delete;
    Promise& operator=(Promise&&) = delete;

    ~Promise() {
        if (f) {
            f->check();
        }
    }

    void set_value(int val) {
        f->val = val;
        f->is_set = true;
    }

    Future get_future() {
        return Future(f);
    }
};

int kNumIters;

bool IsPrime(int x) {
    return true;
}

struct reqres {
    int request;
    Promise response;
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

Future Enqueue(int x) {
    reqres* r = new reqres;
    r->request = x;
    auto f = r->response.get_future();
    q.enqueue(r);
    return std::move(f);
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