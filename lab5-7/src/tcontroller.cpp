#include "tcontroller.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <unistd.h> // execl, fork

TControllerNode::TControllerNode() {}

bool TControllerNode::Init(const std::string& endpoint) {
    if (!messaging_.InitController(endpoint)) {
        std::cerr << "Controller: Failed to init messaging.\n";
        return false;
    }
    return true;
}

void TControllerNode::Run() {
    // Поток приёма сообщений
    std::thread recv_thread([this]() {
        while (running_) {
            int nid;
            std::string msg;
            if (!messaging_.RecvFromAnyWorker(nid, msg)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            if (msg.rfind("HB", 0) == 0) {
                TNodeInfo* node = topology_.GetNode(nid);
                if (node) {
                    node->last_heartbeat = std::chrono::steady_clock::now();
                }
            } else {
                std::cout << msg << "\n";
            }
        }
    });
    recv_thread.detach();

    while (running_) {
        std::cout << "> ";
        std::string line;
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string cmd; 
        iss >> cmd;

        if (cmd == "create") {
            int id; 
            int parent = -1;
            iss >> id >> parent;
            HandleCreate(id, parent);
        } else if (cmd == "exec") {
            int id; 
            iss >> id;
            HandleExec(id);
        } else if (cmd == "heartbit") {
            int t; 
            iss >> t;
            HandleHeartbit(t);
        } else if (cmd == "quit") {
            Quit();
        } else {
            std::cout << "Unknown command\n";
        }

        CheckHeartbeats();
    }
}

void TControllerNode::HandleCreate(int id, int parent) {
    if (topology_.NodeExists(id)) {
        std::cout << "Error: Already exists\n";
        return;
    }
    if (parent != -1 && !topology_.NodeExists(parent)) {
        std::cout << "Error: Parent not found\n";
        return;
    }
    if (parent != -1 && !IsNodeAvailable(parent)) {
        std::cout << "Error: Parent is unavailable\n";
        return;
    }

    const char* workerPath = std::getenv("WORKER_PATH");
    if (!workerPath) {
        std::cout << "Error: WORKER_PATH environment variable is not set\n";
        return;
    }

    std::string endpoint = "tcp://127.0.0.1:" + std::to_string(6000 + id);
    pid_t pid = fork();
    if (pid == 0) {
        execl(workerPath, "worker", std::to_string(id).c_str(), std::to_string(parent).c_str(), endpoint.c_str(), (char*)NULL);
        std::cerr << "Error: Failed to exec worker from WORKER_PATH\n";
        _exit(1);
    } else if (pid < 0) {
        std::cout << "Error: Failed to create process\n";
        return;
    }

    if (!topology_.AddNode(id, parent, endpoint)) {
        std::cout << "Error: Internal AddNode failed\n";
        return;
    }
    std::cout << "Ok: " << pid << "\n";
}

void TControllerNode::HandleExec(int id) {
    if (!topology_.NodeExists(id)) {
        std::cout << "Error:" << id << ": Not found\n";
        return;
    }
    if (!IsNodeAvailable(id)) {
        std::cout << "Error:" << id << ": Node is unavailable\n";
        return;
    }

    std::cout << "(text_string): ";
    std::string text; 
    if (!std::getline(std::cin, text) || text.empty()) std::getline(std::cin, text);
    std::cout << "(pattern_string): ";
    std::string pattern; 
    if (!std::getline(std::cin, pattern) || pattern.empty()) std::getline(std::cin, pattern);

    std::string msg = "EXEC\n" + text + "\n" + pattern;
    if(!messaging_.SendToWorker(id, msg)) {
        std::cout << "Error:" << id << ": Failed to send message\n";
    }
}

void TControllerNode::HandleHeartbit(int time_ms) {
    heartbeat_time_ = time_ms;
    std::cout << "Ok\n";
}

void TControllerNode::Quit() {
    running_ = false;
    std::cout << "Exiting...\n";
    // Опционально убить все воркеры
    // for (auto& [id, info] : topology_.getAllNodes()) {
    //     // можно послать сообщение на завершение узла, если нужна чистота
    // }
}

bool TControllerNode::IsNodeAvailable(int id) {
    TNodeInfo* node = topology_.GetNode(id);
    if (!node) return false;
    if (!node->alive) return false;
    if (heartbeat_time_ > 0) {
        auto now = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - node->last_heartbeat).count();
        if (diff > 4 * heartbeat_time_) {
            return false;
        }
    }
    return true;
}

void TControllerNode::CheckHeartbeats() {
    if (heartbeat_time_ <= 0) return;
    auto now = std::chrono::steady_clock::now();
    for (auto& [nid, info] : topology_.getAllNodes()) {
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - info.last_heartbeat).count();
        if (diff > 4 * heartbeat_time_ && info.alive) {
            info.alive = false;
            std::cout << "Heartbit: node " << nid << " is unavailable now\n";
        }
    }
}
