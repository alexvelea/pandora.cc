#include "multiplication.hpp"
#include "addition.hpp"

void Multiplication::handle_request(const Multiplication::Request& request, Multiplication::Response* response) {
    response->x = 0;
    for (int i = 1; i <= request.a; i += 1) {
        response->x = make_async_rpc_call<Addition>({response->x, request.b}).get().x;
    }
}

Multiplication::Multiplication(int) { }

DefineService(Multiplication)