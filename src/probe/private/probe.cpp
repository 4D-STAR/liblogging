#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/Logger.h"
#include "quill/sinks/ConsoleSink.h"
#include "quill/sinks/FileSink.h" 
#include "quill/LogMacros.h"
#include <stdexcept>              
#include <string>
#include <iostream>
#include <chrono>
#include <cmath>

#include "mfem.hpp"

#include "config.h" 
#include "probe.h" 

#include "warning_control.h"


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
    std::cout << "GLVis visualization enabled. Opening GLVis window... " << std::endl;
    std::cout << "Using host: " << vishost << " and port: " << visport << std::endl;
    mfem::socketstream sol_sock(vishost.c_str(), visport);
    sol_sock.precision(8);
    sol_sock << "solution\n" << mesh << u
             << "window_title '" << windowTitle << "'\n" << std::flush; // Added title
  }
}

void glVisView(mfem::Vector &vec, mfem::FiniteElementSpace &fes, const std::string &windowTitle) {
  mfem::GridFunction gf(&fes);
  
  DEPRECATION_WARNING_OFF
    gf.SetData(vec);
  DEPRECATION_WARNING_ON
  glVisView(gf, *fes.GetMesh(), windowTitle);
}

double getMeshRadius(mfem::Mesh& mesh) {
  int numVertices = mesh.GetNV(); // Number of vertices
  double maxRadius = 0.0;

  for (int i = 0; i < numVertices; i++) {
      double* vertex; 
      vertex = mesh.GetVertex(i); // Get vertex coordinates

      // Compute the Euclidean distance from the origin
      double radius = std::sqrt(vertex[0] * vertex[0] +
                                vertex[1] * vertex[1] +
                                vertex[2] * vertex[2]);

      if (radius > maxRadius) {
          maxRadius = radius;
      }
  }
  return maxRadius;
}

std::vector<double> getRaySolution(mfem::GridFunction& u, mfem::Mesh& mesh,
                                   const std::vector<double>& rayDirection,
                                   int numSamples) {
  LogManager& logManager = LogManager::getInstance();
  quill::Logger* logger = logManager.getLogger("polyTest");
  std::vector<double> samples;
  samples.reserve(numSamples);
  double x, y, z, r, sampleValue;
  double radius = getMeshRadius(mesh);
  mfem::Vector rayOrigin(3); rayOrigin = 0.0;
  mfem::DenseMatrix rayPoints(3, numSamples);

  for (int i = 0; i < numSamples; i++) {
    r = i * radius / numSamples;
    // Let rayDirection = (theta, phi) that the ray will be cast too
    x = r * std::sin(rayDirection[0]) * std::cos(rayDirection[1]);
    y = r * std::sin(rayDirection[0]) * std::sin(rayDirection[1]);
    z = r * std::cos(rayDirection[0]);
    rayPoints(0, i) = x;
    rayPoints(1, i) = y;
    rayPoints(2, i) = z;
  }

  mfem::Array<int> elementIds;
  mfem::Array<mfem::IntegrationPoint> ips;
  mesh.FindPoints(rayPoints, elementIds, ips);
  for (int i = 0; i < elementIds.Size(); i++) {
    int elementId = elementIds[i];
    mfem::IntegrationPoint ip = ips[i];

    if (elementId >= 0) { // Check if the point was found in an element
        sampleValue = u.GetValue(i, ip);
        LOG_INFO(logger, "Sample {}: Value = {:0.2E}", i, sampleValue);
        samples.push_back(sampleValue);
    } else { // If the point was not found in an element
        samples.push_back(0.0); 
    }
  }
  return samples;
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