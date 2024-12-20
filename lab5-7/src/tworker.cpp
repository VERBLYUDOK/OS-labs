#include "tworker.h"
#include "tsearch.h"
#include <iostream>
#include <chrono>
#include <pthread.h>

TWorkerNode::TWorkerNode(int id, int parent_id, const std::string& endpoint)
: id_(id), parent_id_(parent_id), endpoint_(endpoint) {}

bool TWorkerNode::Init() {
    if (!messaging_.InitWorker("tcp://127.0.0.1:5555", id_)) {
        std::cerr << "Worker " << id_ << ": Failed to init messaging\n";
        return false;
    }
    return true;
}

void* TWorkerNode::HeartbeatThreadStatic(void* arg) {
    TWorkerNode* self = static_cast<TWorkerNode*>(arg);
    while (true) {
        self->SendHeartbeat();
        usleep(2000 * 1000); // 2000 ms = 2 s
    }
    return NULL;
}

void TWorkerNode::Run() {
    pthread_t hb_thread;
    pthread_create(&hb_thread, NULL, &HeartbeatThreadStatic, this);
    pthread_detach(hb_thread);

    while (true) {
        std::string msg;
        if (!messaging_.RecvFromController(msg)) {
            usleep(100 * 1000); // 100 ms задержка
            continue;
        }
        if (msg.rfind("EXEC", 0) == 0) {
            size_t pos1 = msg.find('\n');
            size_t pos2 = msg.find('\n', pos1+1);
            if (pos1 == std::string::npos || pos2 == std::string::npos) {
                messaging_.SendToController("Error:" + std::to_string(id_) + ": Invalid EXEC format");
                continue;
            }
            std::string text = msg.substr(pos1+1, pos2 - (pos1+1));
            std::string pattern = msg.substr(pos2+1);
            HandleExec(text, pattern);
        } else {
            messaging_.SendToController("Error:" + std::to_string(id_) + ": Unknown command");
        }
    }
}

void TWorkerNode::HandleExec(const std::string& text, const std::string& pattern) {
    auto positions = TSearch::BoyerMooreSearch(text, pattern);
    std::string result;
    if (positions.size() == 1 && positions[0] == -1) {
        result = "Ok:" + std::to_string(id_) + ": -1";
    } else {
        result = "Ok:" + std::to_string(id_) + ":";
        for (int i = 0; i < (int)positions.size(); i++) {
            if (i > 0) result += ";";
            result += std::to_string(positions[i]);
        }
    }
    messaging_.SendToController(result);
}

void TWorkerNode::SendHeartbeat() {
    std::string hb = "HB " + std::to_string(id_);
    messaging_.SendToController(hb);
}