#pragma once
#include <pandora/services.hpp>

struct Multiplication {
    struct Request {
        int a, b;
    };

    struct Response {
        int x;
    };

    DeclareService(Multiplication);
};
