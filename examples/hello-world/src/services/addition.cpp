#include "addition.hpp"

std::string Addition::rpc_name = "addition";
std::vector<int> Addition::run_ids = {0};

Future<Addition::Response> Addition::async(const Addition::Request& request) {
    return make_async_rpc_call<Addition>(request);
}

void Addition::blocking(const Addition::Request& request, Addition::Response* response) {
    make_blocking_rpc_call<Addition>(request, response);
}

void Addition::internal_blocking(const Addition::Request& request, Addition::Response* response) {
    response->x = request.a + request.b;
}
