#include "WorkerRelations.h"
#include <functional>
#include <utility>
#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>

bool xnor(bool a, bool b) {
    return (a && b) || (!a && !b);
}

Worker::Worker(std::string  name, bool isRightHanded) : name(std::move(name)), isRightHanded(isRightHanded) {}

void WorkerRelations::addWorker(const std::string& name, bool isRightHanded) {
    entryPairs[isRightHanded].emplace(name, std::unordered_set<std::string>{}); //Each worker is placed in the correct map, with an empty unordered_set
}

void WorkerRelations::addLikes(const std::string &name, const std::vector<std::string> &likes) {
    int n = likes.size();
    for(int i = 0; i < n; i++) {
        const std::string& like = likes[i];
        bool a = entryPairs[0].find(name) == entryPairs[0].end();
        bool b = entryPairs[0].find(like) == entryPairs[0].end();
        if(xnor(a, b)) {    //if both workers are in the same map, that means they can't work together and we don't need to process further.
            continue;
        }

        if(entryPairs[b][like].find(name) != entryPairs[b][like].end()) {   //Worker A likes worker B. So if worker B already likes worker A, then we can instantly add the pair to the final map.
            finalPairs[Worker(name, a)].insert(Worker(like, b));
        }
        else {
            entryPairs[a][name].insert(like);   //Otherwise, we add the one-sided relation to the entryPairs map.
        }
    }
}

void WorkerRelations::parseTxt(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    int n;
    file >> n;
    for (int i = 0; i < n; ++i) {
        std::string name;
        bool isRightHanded;
        file >> name >> isRightHanded;
        addWorker(name, isRightHanded);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line == "0") {
            break;
        }
        std::istringstream iss(line);
        std::string name;
        iss >> name;
        std::vector<std::string> likes;
        std::string like;
        while (iss >> like) {
            likes.push_back(like);
        }
        addLikes(name, likes);
    }

    file.close();
}

void WorkerRelations::printPairs() {
    for (const auto& pair : finalPairs) {
        const Worker& worker = pair.first;
        std::cout << worker.name << " likes: ";
        for (const auto& likedWorker : pair.second) {
            std::cout << likedWorker.name << " ";
        }
        std::cout << std::endl;
    }
}

bool bfs(const std::unordered_map<Worker, Worker>& pairU, const std::unordered_map<Worker, Worker>& pairV,
         const std::unordered_map<Worker, std::vector<Worker>>& adj,
         std::unordered_map<Worker, int>& dist, const std::vector<Worker>& leftWorkers) {
    std::queue<Worker> q;
    for (const auto& u : leftWorkers) {
        if (pairU.find(u) == pairU.end()) {
            dist[u] = 0;
            q.push(u);
        } else {
            dist[u] = std::numeric_limits<int>::max();
        }
    }

    bool foundAugmentingPath = false;
    while (!q.empty()) {
        Worker u = q.front();
        q.pop();

        for (const auto& v : adj.at(u)) {
            if (pairV.find(v) == pairV.end()) {
                foundAugmentingPath = true;
            } else {
                const Worker& uPrime = pairV.at(v);
                if (dist[uPrime] == std::numeric_limits<int>::max()) {
                    dist[uPrime] = dist[u] + 1;
                    q.push(uPrime);
                }
            }
        }
    }

    return foundAugmentingPath;
}

bool dfs(const Worker& u, std::unordered_map<Worker, Worker>& pairU, std::unordered_map<Worker, Worker>& pairV,
         const std::unordered_map<Worker, std::vector<Worker>>& adj,
         std::unordered_map<Worker, int>& dist) {
    if (dist[u] != std::numeric_limits<int>::max()) {
        for (const auto& v : adj.at(u)) {
            if (pairV.find(v) == pairV.end() || (dist[pairV.at(v)] == dist[u] + 1 && dfs(pairV.at(v), pairU, pairV, adj, dist))) {
                pairU[u] = v;
                pairV[v] = u;
                return true;
            }
        }
        dist[u] = std::numeric_limits<int>::max();
        return false;
    }
    return false;
}

std::vector<std::pair<Worker, Worker>> WorkerRelations::hopcroftKarp() {
    std::unordered_map<Worker, std::vector<Worker>> adj;
    std::vector<Worker> leftWorkers, rightWorkers;

    // Construct the adjacency list and the sets of left and right workers
    for (const auto& pair : finalPairs) {
        const Worker& u = pair.first;
        for (const auto& v : pair.second) {
            adj[u].push_back(v);
            adj[v].push_back(u);

            if (std::find(leftWorkers.begin(), leftWorkers.end(), u) == leftWorkers.end()) {
                leftWorkers.push_back(u);
            }

            if (std::find(rightWorkers.begin(), rightWorkers.end(), v) == rightWorkers.end()) {
                rightWorkers.push_back(v);
            }
        }
    }

    std::unordered_map<Worker, Worker> pairU, pairV;
    std::unordered_map<Worker, int> dist;

    while (bfs(pairU, pairV, adj, dist, leftWorkers)) {
        for (const auto& u : leftWorkers) {
            if (pairU.find(u) == pairU.end()) {
                dfs(u, pairU, pairV, adj, dist);
            }
        }
    }

    std::unordered_set<Worker> matchedWorkers;
    std::vector<std::pair<Worker, Worker>> matching;

    for (const auto& pair : pairU) {
        const Worker& u = pair.first;
        const Worker& v = pair.second;

        if (matchedWorkers.find(u) == matchedWorkers.end() && matchedWorkers.find(v) == matchedWorkers.end()) {
            matching.emplace_back(u, v);
            matchedWorkers.insert(u);
            matchedWorkers.insert(v);
        }
    }

    return matching;
}