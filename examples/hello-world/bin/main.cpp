#include <iostream>

#include <pandora/services.hpp>
#include <services/multiplication.hpp>

int main() {
    auto q_threads = start_queues();


    auto f = enqueue<Multiplication>({(int)3e4, 1});

    std::cerr << "Got result: " << f.get().x << '\n';

    stop_queues();
    for (auto& thread : q_threads) {
        thread.join();
    }
}