#include <pandora/services/loadService.hpp>
#include <pandora/core.hpp>

LoadService::Request LoadService::from_network(autojson::JSON& j) {
    return {
            string(j["path"] | "./"),
            string(j["name"])
    };
}

autojson::JSON LoadService::to_network_response(const LoadService::Response* res) {
    if (res->success) {
        return {
                {"success", true},
                {"loaded_handlers", res->loaded_handlers}
        };
    } else {
        return {
                {"success", false},
                {"error",   res->error}
        };
    }
}

void LoadService::handle_request(const LoadService::Request& request, LoadService::Response* response) {
    loadServiceInternal(request, response);
}

LoadService::LoadService(int) { }

DefineService(LoadService)