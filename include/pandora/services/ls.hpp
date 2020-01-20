#pragma once
#include "../bundle.hpp"
#include <vector>
#include <string>

struct LS {
    struct Request { };

    struct Response {
        std::vector<std::string> available_services;
    };

    DeclareService(LS);
};