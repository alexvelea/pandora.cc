#pragma once

#include "services/loadService.hpp"
#include <vector>
#include <thread>

// used to soft-stop the queue
std::vector<std::thread> start_queues();
void stop_queues();

void InitPandora(int argc, char** argv);

void loadServiceInternal(const LoadService::Request& request, LoadService::Response* response);