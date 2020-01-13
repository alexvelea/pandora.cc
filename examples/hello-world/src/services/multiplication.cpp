#include "multiplication.hpp"

#include "addition.hpp"

std::string Multiplication::rpc_name = "multiplication";
std::vector<int> Multiplication::run_ids = {1};

Future<Multiplication::Response> Multiplication::async(const Multiplication::Request& request) {
    return make_async_rpc_call<Multiplication>(request);
}

void Multiplication::blocking(const Multiplication::Request& request, Multiplication::Response* response) {
    make_blocking_rpc_call<Multiplication>(request, response);
}

void Multiplication::internal_blocking(const Multiplication::Request& request, Multiplication::Response* response) {
    response->x = 0;
    for (int i = 1; i <= request.a; i += 1) {
        response->x = Addition::async({response->x, request.b}).get().x;
    }
}