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
#include <iomanip>

#include "logger.h"

Record makeRecord(const std::string& message, const std::string& context, DiagnosticLevel level) {
    return {message, context, std::chrono::system_clock::now(), level};
}

std::string timestepToString(const std::chrono::time_point<std::chrono::system_clock>& timestamp) {
    std::time_t timeT = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tmStruct = *std::localtime(&timeT);

    std::ostringstream oss;
    oss << std::put_time(&tmStruct, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::unordered_map<std::string, Logger*> Logger::instances;
std::mutex Logger::mapMutex;

Logger::Logger(const std::string& filename) : filename(filename), logFile(filename, std::ios::app) {
    logThread = std::thread(&Logger::processesLogs, this);
}

void Logger::processesLogs() {
    while (running || !logQueue.empty()) {
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCondition.wait(lock, [this] { return !logQueue.empty() || !running; });

        while (!logQueue.empty()) {
            logFile << logQueue.front() << std::endl;
            logQueue.pop();
        }
    }
}

std::string Logger::levelAsString(DiagnosticLevel level) {
    switch (level) {
        case DiagnosticLevel::DEBUG:
            return "DEBUG";
        case DiagnosticLevel::INFO:
            return "INFO";
        case DiagnosticLevel::WARNING:
            return "WARNING";
        case DiagnosticLevel::ERROR:
            return "ERROR";
        case DiagnosticLevel::CRITICAL:
            return "CRITICAL";
        case DiagnosticLevel::NONE:
            return "NONE";
    }
    return "";
}

Logger& Logger::getInstance(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mapMutex);
    auto it = instances.find(filename);
    if (it == instances.end()) {
        it = instances.emplace(filename, new Logger(filename)).first;
    }
    return *it->second;
}

void Logger::log(const Record& record) {
    std::stringstream ss;
    ss << levelAsString(record.level) << " @ " << timestepToString(record.timestamp) << " [" << record.context << "] :" << record.message;
    std::lock_guard<std::mutex> lock(queueMutex);
    logQueue.push(ss.str());
    queueCondition.notify_one();
}

void Logger::log(const std::string& message, const std::string& context, DiagnosticLevel level) {
    log(makeRecord(message, context, level));
}

void Logger::log(const std::string& message, DiagnosticLevel level) {
    log(makeRecord(message, "", level));
}
void Logger::log(const std::string& message) {
    log(makeRecord(message, "", DiagnosticLevel::INFO));
}
Logger::~Logger() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        running = false;
    }
    queueCondition.notify_one();
    if (logThread.joinable()) {
        logThread.join();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // wait for the log thread to finish writing
}