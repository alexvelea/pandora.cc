#pragma once

#include <atomic>
#include <future>

template<typename T>
struct FutureBase {
    std::atomic<bool> is_set;
    std::atomic<int> counter;
    T value;

    void decrement_counter() {
        if (counter.fetch_sub(1) == 1) {
            delete this;
        }
    }

    FutureBase() = delete;

    static FutureBase<T>* create() {
        auto p = (FutureBase<T>*)malloc(sizeof(FutureBase<T>));
        p->is_set = false;
        p->counter = 0;
        return p;
    }

    FutureBase(const FutureBase&) = delete;
    FutureBase(FutureBase&&) = delete;

    FutureBase& operator=(const FutureBase&) = delete;
    FutureBase& operator=(FutureBase&&) = delete;
};

template<typename T>
struct Future {
    FutureBase<T>* f;

    Future(FutureBase<T>* f) : f(f) {
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
            f->decrement_counter();
        }
    }

    T get() const {
        while (f->is_set == false) ;
        return f->value;
    }
};

template<typename T>
struct Promise {
    FutureBase<T>* f;

    Promise() {
        f = FutureBase<T>::create();
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
            f->decrement_counter();
        }
    }

    void set_value(T value) const {
        f->value = value;
        f->is_set = true;
    }

    Future<T> get_future() const {
        return Future<T>(f);
    }
};