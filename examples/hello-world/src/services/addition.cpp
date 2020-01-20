#include "addition.hpp"

Addition::Request Addition::from_network(autojson::JSON& j) {
    return {
            int(j["a"]),
            int(j["b"])
    };
}

autojson::JSON Addition::to_network_response(const Addition::Response* res) {
    return {"x", res->x};
}

void Addition::handle_request(const Addition::Request& request, Addition::Response* response) {
    response->x = request.a + request.b;
}

Addition::Addition(int) { }

DefineService(Addition)