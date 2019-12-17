#include "addition.hpp"

Addition::Response Addition::operator() (const Addition::Request& request) {
    return {request.a + request.b};
}

Addition addition;