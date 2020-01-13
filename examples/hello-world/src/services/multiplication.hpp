#pragma once
#include <pandora/services.hpp>

struct Multiplication {
    static std::string rpc_name;
    static std::vector<int> run_ids;

    struct Request {
        int a, b;
    };

    struct Response {
        int x;
    };

    static Future<Response> async(const Request&);
    static void blocking(const Request&, Response*);

    Multiplication(int) { }

    void internal_blocking(const Request&, Response*);
};

