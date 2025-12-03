/* ***********************************************************************
//
//   Copyright (C) 2025 -- The 4D-STAR Collaboration
//   File Author: Emily Boudreaux
//   Last Modified: March 18, 2025
//
//   liblogging is free software; you can use it and/or modify
//   it under the terms and restrictions the GNU General Library Public
//   License version 3 (GPLv3) as published by the Free Software Foundation.
//
//   liblogging is distributed in the hope that it will be useful,
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
#include <vector>
#include <utility>

#if defined(__EMSCRIPTEN__)
 #include <ranges>
#endif


#include "fourdst/logging/logging.h"


namespace fourdst::logging {

LogManager::LogManager() {
  quill::Backend::start();
  
  auto CLILogger = quill::Frontend::create_or_get_logger(
      "root",
      quill::Frontend::create_or_get_sink<quill::ConsoleSink>("sink_id_1"));

  newFileLogger("fourdst.log", "log");
  
  loggerMap.emplace("stdout", CLILogger);
}

#if defined(__EMSCRIPTEN__)

LogManager::~LogManager() {
  for (const auto& logger : loggerMap | std::views::values) {
    logger->flush_log();
  }
  quill::Backend::stop();
}

#else

LogManager::~LogManager() = default;

#endif

quill::Logger* LogManager::getLogger(const std::string& loggerName) {
  auto it = loggerMap.find(loggerName); 
  if (it == loggerMap.end()) {
    throw std::runtime_error("Cannot find logger " + loggerName);
  }
  return it->second;
}

std::vector<std::string> LogManager::getLoggerNames() {
  std::vector<std::string> loggerNames;
  loggerNames.reserve(loggerMap.size());
  for (const auto& pair : loggerMap) { 
    loggerNames.push_back(pair.first);
  }
  return loggerNames;
}

std::vector<quill::Logger*> LogManager::getLoggers() {
  std::vector<quill::Logger*> loggers;
  loggers.reserve(loggerMap.size());
  for (const auto& pair : loggerMap) {
     loggers.push_back(pair.second); 
  }
  return loggers;
}

quill::Logger* LogManager::newFileLogger(const std::string& filename,
                                         const std::string& loggerName) {

#if defined(__EMSCRIPTEN__)
  auto proxy_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("sink_id_1");
  quill::Logger* rawLogger = quill::Frontend::create_or_get_logger(loggerName, std::move(proxy_sink));

  loggerMap.emplace(loggerName, rawLogger);
  return rawLogger;
#else
  auto file_sink = quill::Frontend::create_or_get_sink<quill::FileSink>(
      filename,
      []() {
        quill::FileSinkConfig cfg;
        cfg.set_open_mode('w');
        return cfg;
      }(),
      quill::FileEventNotifier{});
  
  quill::Logger* rawLogger = quill::Frontend::create_or_get_logger(loggerName, std::move(file_sink));

  loggerMap.emplace(loggerName, rawLogger);
  return rawLogger;
#endif
}

} 

