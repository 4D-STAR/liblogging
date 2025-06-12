/* ***********************************************************************
//
//   Copyright (C) 2025 -- The 4D-STAR Collaboration
//   File Author: Emily Boudreaux
//   Last Modified: April 03, 2025
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
//=== Probe.h ===
#pragma once

#include <string>
#include <map>
#include <vector>
#include <utility>

#include "mfem.hpp"
#include "quill/Logger.h"

/**
 * @brief The Probe namespace contains utility functions for debugging and logging.
 */
namespace serif::probe {
  /**
   * @brief Pause the execution and wait for user input.
   */
  void pause();

  /**
   * @brief Wait for a specified number of seconds.
   * @param seconds The number of seconds to wait.
   */
  void wait(int seconds);

  /**
   * @brief Visualize a solution using GLVis.
   * @param u The GridFunction to visualize.
   * @param mesh The mesh associated with the GridFunction.
   * @param windowTitle The title of the visualization window.
   * @param keyset The keyset to use for visualization.
   */
  void glVisView(mfem::GridFunction& u, mfem::Mesh& mesh,
    const std::string& windowTitle = "grid function", const std::string& keyset="");

  /**
   * @brief Visualize a vector using GLVis.
   * @param vec The vector to visualize.
   * @param mesh The mesh associated with the vector.
   * @param windowTitle The title of the visualization window.
   * @param keyset The keyset to use for visualization.
   */
  void glVisView(mfem::Vector &vec, mfem::FiniteElementSpace &fes,
    const std::string &windowTitle = "vector", const std::string& keyset="");

  double getMeshRadius(mfem::Mesh& mesh);

  std::pair<std::vector<double>, std::vector<double>> getRaySolution(mfem::GridFunction& u, mfem::Mesh& mesh,
    const std::vector<double>& rayDirection, int numSamples, std::string filename="");

  std::pair<std::vector<double>, std::vector<double>> getRaySolution(mfem::Vector &vec, mfem::FiniteElementSpace &fes,
    const std::vector<double>& rayDirection, int numSamples, std::string filename="");


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