#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <map>
#include <vector>
#include <chrono>

struct TNodeInfo {
    int id;
    int parent_id;
    std::string endpoint;
    bool alive = true;
    std::chrono::steady_clock::time_point last_heartbeat;
};

#endif
