#include <iostream>

#include <pandora/core.hpp>
#include <services/multiplication.hpp>


int main(int argc, char** argv) {
    InitPandora(argc, argv);
    auto q_threads = start_queues();

    auto f = make_async_rpc_call<Multiplication>({(int)1e6, 1});
    std::cerr.flush();
    std::cout << "Got result: " <<  f.get().x << '\n';

    stop_queues();
    for (auto& thread : q_threads) {
        thread.join();
    }
}