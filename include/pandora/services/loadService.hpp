#pragma once
#include "../bundle.hpp"

struct LoadService {
    struct Request {
        std::string path = "./";
        std::string name;
    };

    struct Response {
        bool success;
        std::string error;
        std::vector<std::string> loaded_handlers;
    };

    DeclareService(LoadService);
};