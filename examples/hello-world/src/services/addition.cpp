#include "addition.hpp"

void Addition::call_blocking(const Addition::Request& request, Addition::Response& response) {

    response.x = request.a + request.b;
}

std::future<Addition::Response> Addition::operator() (const Addition::Request& request) {
    return enqueue<Addition>(request);
}

Addition Addition::caller;