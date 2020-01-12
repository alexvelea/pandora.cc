#include "multiplication.hpp"

#include "addition.hpp"

void Multiplication::call_blocking(const Multiplication::Request& request, Multiplication::Response& response) {
    int answer = 0;

    for (int i = 1; i <= request.a; i += 1) {
        answer = Addition::caller({answer, request.b}).get().x;
    }

    response.x = answer;
}

Future<Multiplication::Response> Multiplication::operator() (const Multiplication::Request& request) {
    return enqueue<Multiplication>(request);
}

Multiplication Multiplication::caller;