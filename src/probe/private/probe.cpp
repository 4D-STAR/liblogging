/* ***********************************************************************
//
//   Copyright (C) 2025 -- The 4D-STAR Collaboration
//   File Author: Emily Boudreaux
//   Last Modified: February 23, 2025
//
//   4DSSE is free software; you can use it and/or modify
//   it under the terms and restrictions the GNU General Library Public
//   License version 3 (GPLv3) as published by the Free Software Foundation.
//
//   4DSSE is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU Library General Public License for more details.
//
//   You should have received a copy of the GNU Library General Public License
//   along with this software; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// *********************************************************************** */
#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/Logger.h"
#include "quill/sinks/ConsoleSink.h"
#include "quill/sinks/FileSink.h" 
#include <stdexcept>              
#include <string>
#include <iostream>
#include <chrono>

#include "mfem.hpp"

#include "config.h" 
#include "probe.h" 


namespace Probe {

void pause() {
  std::cout << "Execution paused. Please press enter to continue..."
            << std::endl; // Use endl to flush
  std::cin.get();
}

void wait(int seconds) {
  std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void glVisView(mfem::GridFunction& u, mfem::Mesh& mesh,
               const std::string& windowTitle) {
  Config& config = Config::getInstance();
  if (config.get<bool>("Probe:GLVis:Visualization", true)) {
    std::string vishost = config.get<std::string>("Probe:GLVis:Host", "localhost");
    int visport = config.get<int>("Probe:GLVis:Port", 19916); // Changed default port
    mfem::socketstream sol_sock(vishost.c_str(), visport);
    sol_sock.precision(8);
    sol_sock << "solution\n" << mesh << u
             << "window_title '" << windowTitle << "'\n" << std::flush; // Added title
  }
}

LogManager::LogManager() {
  Config& config = Config::getInstance();
  quill::Backend::start();
  auto CLILogger = quill::Frontend::create_or_get_logger(
      "root",
      quill::Frontend::create_or_get_sink<quill::ConsoleSink>("sink_id_1"));

  newFileLogger(config.get<std::string>("Probe:LogManager:DefaultLogName", "4DSSE.log"), "log");
  loggerMap.emplace("stdout", CLILogger);
}

LogManager::~LogManager() = default;

quill::Logger* LogManager::getLogger(const std::string& loggerName) {
  auto it = loggerMap.find(loggerName); // Find *once*
  if (it == loggerMap.end()) {
    throw std::runtime_error("Cannot find logger " + loggerName);
  }
  return it->second; // Return the raw pointer from the shared_ptr
}

std::vector<std::string> LogManager::getLoggerNames() {
  std::vector<std::string> loggerNames;
  loggerNames.reserve(loggerMap.size());
  for (const auto& pair : loggerMap) { // Use range-based for loop and const auto&
    loggerNames.push_back(pair.first);
  }
  return loggerNames;
}

std::vector<quill::Logger*> LogManager::getLoggers() {
  std::vector<quill::Logger*> loggers;
  loggers.reserve(loggerMap.size());
  for (const auto& pair : loggerMap) {
     loggers.push_back(pair.second); // Get the raw pointer
  }
  return loggers;
}

quill::Logger* LogManager::newFileLogger(const std::string& filename,
                                        const std::string& loggerName) {
  auto file_sink = quill::Frontend::create_or_get_sink<quill::FileSink>(
      filename,
      []() {
        quill::FileSinkConfig cfg;
        cfg.set_open_mode('w');
        return cfg;
      }(),
      quill::FileEventNotifier{});
  // Get the raw pointer.
  quill::Logger* rawLogger = quill::Frontend::create_or_get_logger(loggerName, std::move(file_sink));

  // Create a shared_ptr from the raw pointer.
  loggerMap.emplace(loggerName, rawLogger);
  return rawLogger;
}

} // namespace Probe