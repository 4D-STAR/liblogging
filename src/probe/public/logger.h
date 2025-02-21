#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <atomic>
#include <unordered_map>

enum class DiagnosticLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
    NONE
};

struct Record {
    std::string message;
    std::string context;
    std::chrono::time_point<std::chrono::system_clock> timestamp;
    DiagnosticLevel level;
};

Record makeRecord(const std::string& message, const std::string& context, DiagnosticLevel level);

std::string timestepToString(const std::chrono::time_point<std::chrono::system_clock>& timestamp);

class Logger {
private:
    static std::unordered_map<std::string, Logger*> instances;
    static std::mutex mapMutex;

    std::queue<std::string> logQueue;
    std::mutex queueMutex;

    std::condition_variable queueCondition;
    std::thread logThread;
    std::atomic<bool> running;
    std::string filename;
    std::ofstream logFile;

    Logger(const std::string& filename);
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void processesLogs();

    std::string levelAsString(DiagnosticLevel level);

public:
    static Logger& getInstance(const std::string& filename);

    void log(const Record& record);

    void log(const std::string& message, const std::string& context, DiagnosticLevel level);

    void log(const std::string& message, DiagnosticLevel level);

    void log(const std::string& message);

    ~Logger();
};

#endif