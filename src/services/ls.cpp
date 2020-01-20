#include <pandora/services/ls.hpp>
#include <pandora/core.hpp>

LS::Request LS::from_network(autojson::JSON& j) {
    return {};
}

autojson::JSON LS::to_network_response(const LS::Response* res) {
    return {"available_handlers", res->available_services};
}

void LS::handle_request(const LS::Request& request, LS::Response* response) {
    for (auto& itr : state.internalHandlers) {
        response->available_services.emplace_back(itr.first);
    }
}

LS::LS(int) { }

DefineService(LS)