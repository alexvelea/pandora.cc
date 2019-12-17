#pragma once

#include <pandora/services.hpp>

struct Addition {
    struct Request {
        int a, b;
    };

    struct Response {
        int x;
    };

    Response operator() (const Request&);
};

extern Addition addition;

