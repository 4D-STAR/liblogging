#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <fstream>
#include <regex>
#include "quill/LogMacros.h"

#include "logging.h"

std::string getLastLine(const std::string& filename) {
    std::ifstream file(filename);
    std::string line, lastLine;

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    while (std::getline(file, line)) {
        lastLine = line;
    }

    return lastLine;  // Returns the last non-empty line
}

std::string stripTimestamps(const std::string& logLine) {
    std::regex logPattern(R"(\d+:\d+:\d+\.\d+\s+\[\d+\]\s+loggingTest\.cpp:\d+\s+LOG_INFO\s+[A-Za-z]*\s+(.*))");
    std::smatch match;
    if (std::regex_match(logLine, match, logPattern) && match.size() > 1) {
        return match[1].str();  // Extract log message after timestamp
    }
    return logLine;  // Return as-is if pattern doesn't match
}


class loggingTest : public ::testing::Test {};

TEST_F(loggingTest, DefaultConstructorTest) {
    EXPECT_NO_THROW(fourdst::logging::LogManager::getInstance());
}

TEST_F(loggingTest, getLoggerTest) {
    fourdst::logging::LogManager& logManager = fourdst::logging::LogManager::getInstance();
    const std::string loggerName = "testLog";
    const std::string filename = "test.log";
    quill::Logger* logger = logManager.newFileLogger(filename, loggerName);
    EXPECT_NE(logger, nullptr);
    LOG_INFO(logger, "This is a test message");
    // Wait for the log to be written by calling getLastLine until it is non empty
    std::string lastLine;
    while (lastLine.empty()) {
        lastLine = getLastLine("test.log");
    }
    EXPECT_EQ(stripTimestamps(lastLine), "This is a test message");
}

TEST_F(loggingTest, newFileLoggerTest) {
    fourdst::logging::LogManager& logManager = fourdst::logging::LogManager::getInstance();
    const std::string loggerName = "newLog";
    const std::string filename = "newLog.log";
    quill::Logger* logger = logManager.newFileLogger(filename, loggerName);
    EXPECT_NE(logger, nullptr);
    LOG_INFO(logger, "This is a new test message");
    // Wait for the log to be written by calling getLastLine until it is non empty
    std::string lastLine;
    while (lastLine.empty()) {
        lastLine = getLastLine(filename);
    }
    EXPECT_EQ(stripTimestamps(lastLine), "This is a new test message");
}

TEST_F(loggingTest, getLoggerNames) {
    fourdst::logging::LogManager& logManager = fourdst::logging::LogManager::getInstance();
    std::vector<std::string> loggerNames = logManager.getLoggerNames();
    EXPECT_EQ(loggerNames.size(), 4);
    EXPECT_EQ(loggerNames.at(0), "log");
    EXPECT_EQ(loggerNames.at(1), "newLog");
    EXPECT_EQ(loggerNames.at(2), "stdout");
    EXPECT_EQ(loggerNames.at(3), "testLog");
}
