//=== Probe.h ===
#ifndef PROBE_H
#define PROBE_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

#include "mfem.hpp"
#include "quill/Logger.h"

/**
 * @brief The Probe namespace contains utility functions for debugging and logging.
 */
namespace Probe {
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
   */
  void glVisView(mfem::GridFunction& u, mfem::Mesh& mesh,
                const std::string& windowTitle = "solution");

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
#endif