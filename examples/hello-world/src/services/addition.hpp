#pragma once

#include <future>

#include <pandora/services.hpp>

struct Addition {
    static const int q_number = 1;

    struct Request {
        int a, b;
    };

    struct Response {
        int x;
    };

    std::future<Response> operator() (const Request&);
    void call_blocking(const Request&, Response&);

    static Addition caller;
};

