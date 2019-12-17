#include "multiplication.hpp"

#include "addition.hpp"

Multiplication::Response Multiplication::operator() (const Multiplication::Request& request) {
    int answer = 0;
    for (int i = 1; i <= request.a; i += 1) {
        answer = addition({answer, request.b}).x;
    }
    return {answer};
}

Multiplication multiplication;