#include "addition.hpp"

void Addition::handle_request(const Addition::Request& request, Addition::Response* response) {
    response->x = request.a + request.b;
}

Addition::Addition(int) { }

DefineService(Addition)