#include <pandora/bundle.hpp>

vector<pair<string, ServiceCExtern>> *handlers;

void ensureHandlers() {
    if (handlers == nullptr) {
        handlers = new vector<pair<string, ServiceCExtern>>();
    }
}

void addInternalHandler(string names, ServiceCExtern service) {
    ensureHandlers();
    handlers->emplace_back(names, service);
}

extern "C" vector<pair<string, ServiceCExtern>> getInternalHandlers() {
    ensureHandlers();
    return *handlers;
}