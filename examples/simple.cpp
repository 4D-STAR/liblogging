#include <string>
#include <iostream>

#include "quill/LogMacros.h"

#include "fourdst/logging/logging.h"


int main() {
  auto& logManager = fourdst::logging::LogManager::getInstance();
  quill::Logger* logger = logManager.newFileLogger("test.log", "logger");

  std::cout << "Logging...\n";
  LOG_INFO(logger, "This is an info message: {}", 42);
  std::cout << "Done Logging\n";
}
