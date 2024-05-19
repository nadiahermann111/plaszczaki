#ifndef WORKER_RELATIONS_H
#define WORKER_RELATIONS_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Worker {
public:
    std::string name;
    bool isRightHanded;
    std::unordered_set<std::string> likes;

    Worker() : name(""), isRightHanded(false) {}
    Worker(std::string name, bool isRightHanded);

    //needed for unordered_set
    Worker(const Worker& other) = default;
    Worker& operator=(const Worker& other) = default;
    Worker(Worker&& other) = default;
    Worker& operator=(Worker&& other) = default;

    bool operator==(const Worker& other) const {
        return name == other.name && isRightHanded == other.isRightHanded;
    }

    bool operator<(const Worker& other) const {
        return name < other.name || (name == other.name && isRightHanded < other.isRightHanded);
    }

    bool operator!=(const Worker& other) const {
        return !(*this == other);
    }
};

//needed for unordered_set
namespace std {
    template<>
    struct hash<Worker> {
        std::size_t operator()(const Worker& worker) const {
            return std::hash<std::string>()(worker.name) ^ std::hash<bool>()(worker.isRightHanded);
        }
    };
}

class WorkerRelations {
private:
    std::unordered_map<std::string, std::unordered_set<std::string>> entryPairs[2]; //0 for left, 1 for right, seems like a more efficient way of handling which hand the worker has
    std::unordered_map<Worker, std::unordered_set<Worker>> finalPairs;
    std::vector<std::pair<Worker, Worker>> pairs;

    bool bpm(const Worker& worker, std::unordered_map<Worker, Worker>& pairR, std::unordered_set<Worker>& seen);    //used for Hopcroft-Karp algorithm
    int maxBPM(std::unordered_map<Worker, Worker>& pairR);  //used for Hopcroft-Karp algorithm

public:
    void addWorker(const std::string& name, bool isRightHanded);
    void addLikes(const std::string& name, const std::vector<std::string>& likes);
    void parseTxt(const std::string& filename);
    void printPairs();
    std::vector<std::pair<Worker, Worker>> hopcroftKarp();  //efficient way of finding the maximum number of pairs
};

#endif // WORKER_RELATIONS_H
