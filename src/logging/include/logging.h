/* ***********************************************************************
//
//   Copyright (C) 2025 -- The 4D-STAR Collaboration
//   File Author: Emily Boudreaux
//   Last Modified: April 03, 2025
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
//=== Probe.h ===
#pragma once

#include <string>
#include <map>
#include <vector>
#include <utility>

#include "quill/Logger.h"

/**
 * @brief The Probe namespace contains utility functions for debugging and logging.
 */
namespace fourdst::logging {
  /**
   * @brief Class to manage logging operations.
   */
  class LogManager {
  private:
      /**
       * @brief Private constructor for singleton pattern.
       */
      LogManager();

      /**
       * @brief Destructor.
       */
      ~LogManager();

      // Map to store pointers to quill loggers (raw pointers as quill deals with its own memory managment in a seperated, detatched, thread)
      std::map<std::string, quill::Logger*> loggerMap;

      // Prevent copying and assignment (Rule of Zero)
      LogManager(const LogManager&) = delete;
      LogManager& operator=(const LogManager&) = delete;

  public:
      /**
       * @brief Get the singleton instance of LogManager.
       * @return The singleton instance of LogManager.
       */
      static LogManager& getInstance() {
          static LogManager instance;
          return instance;
      }

      /**
       * @brief Get a logger by name.
       * @param loggerName The name of the logger.
       * @return A pointer to the logger.
       */
      quill::Logger* getLogger(const std::string& loggerName);

      /**
       * @brief Get the names of all loggers.
       * @return A vector of logger names.
       */
      std::vector<std::string> getLoggerNames();

      /**
       * @brief Get all loggers.
       * @return A vector of pointers to the loggers.
       */
      std::vector<quill::Logger*> getLoggers();

      /**
       * @brief Create a new file logger.
       * @param filename The name of the log file.
       * @param loggerName The name of the logger.
       * @return A pointer to the new logger.
       */
      quill::Logger* newFileLogger(const std::string& filename,
                                  const std::string& loggerName);
  };

} // namespace Probe
