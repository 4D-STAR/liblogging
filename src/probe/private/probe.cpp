/* ***********************************************************************
//
//   Copyright (C) 2025 -- The 4D-STAR Collaboration
//   File Author: Emily Boudreaux
//   Last Modified: March 18, 2025
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
#include "quill/LogMacros.h"

#include <stdexcept>              
#include <string>
#include <iostream>
#include <chrono>
#include <cmath>
#include <vector>
#include <utility>
#include <filesystem>

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
               const std::string& windowTitle, const std::string& keyset) {
  Config& config = Config::getInstance();
  quill::Logger* logger = LogManager::getInstance().getLogger("log");
  if (config.get<bool>("Probe:GLVis:Visualization", true)) {
    std::string usedKeyset;
    LOG_INFO(logger, "Visualizing solution using GLVis...");
    LOG_INFO(logger, "Window title: {}", windowTitle);
    if (keyset.empty()) {
      usedKeyset = config.get<std::string>("Probe:GLVis:DefaultKeyset", "");
    } else {
      usedKeyset = keyset;
    }
    LOG_INFO(logger, "Keyset: {}", usedKeyset);
    const auto vishost = config.get<std::string>("Probe:GLVis:Host", "localhost");
    const auto visport = config.get<int>("Probe:GLVis:Port", 19916);
    mfem::socketstream sol_sock(vishost.c_str(), visport);
    sol_sock.precision(8);
    sol_sock << "solution\n" << mesh << u
             << "window_title '" << windowTitle <<
             "'\n" << "keys " << usedKeyset << "\n";
    sol_sock << std::flush;
  }
}

void glVisView(mfem::Vector &vec, mfem::FiniteElementSpace &fes, const std::string &windowTitle, const std::string& keyset) {
  mfem::GridFunction gf(&fes);
  
  DEPRECATION_WARNING_OFF
    gf.SetData(vec);
  DEPRECATION_WARNING_ON
  glVisView(gf, *fes.GetMesh(), windowTitle, keyset);
}

double getMeshRadius(mfem::Mesh& mesh) {
  mesh.EnsureNodes();
  const mfem::GridFunction *nodes = mesh.GetNodes();
  double *node_data = nodes->GetData();  // THIS IS KEY

  int data_size = nodes->Size();
  double max_radius = 0.0;

  for (int i = 0; i < data_size; ++i) {
    if (node_data[i] > max_radius) {
      max_radius = node_data[i];
    }
  }
  return max_radius;

}

std::pair<std::vector<double>, std::vector<double>> getRaySolution(mfem::GridFunction& u, mfem::Mesh& mesh,
                                   const std::vector<double>& rayDirection,
                                   int numSamples, std::string filename) {
  Config& config = Config::getInstance();
  Probe::LogManager& logManager = Probe::LogManager::getInstance();
  quill::Logger* logger = logManager.getLogger("log");
  LOG_INFO(logger, "Getting ray solution...");
  // Check if the directory to write to exists
  // If it does not exist and MakeDir is true create it
  // Otherwise throw an exception
  bool makeDir = config.get<bool>("Probe:GetRaySolution:MakeDir", true);
  std::filesystem::path path = filename;

  if (makeDir) {
    std::filesystem::path dir = path.parent_path();
    if (!std::filesystem::exists(dir)) {
      LOG_INFO(logger, "Creating directory {}", dir.string());
      std::filesystem::create_directories(dir);
    }
  } else {
    if (!std::filesystem::exists(path.parent_path())) {
      throw(std::runtime_error("Directory " + path.parent_path().string() + " does not exist"));
    }
  }

  std::vector<double> samples;
  samples.reserve(numSamples);
  double x, y, z, r, sampleValue;
  double radius = getMeshRadius(mesh);
  mfem::Vector rayOrigin(3); rayOrigin = 0.0;
  mfem::DenseMatrix rayPoints(3, numSamples);
  std::vector<double> radialPoints;
  radialPoints.reserve(numSamples);
  mfem::ElementTransformation* Trans = nullptr;
  mfem::Vector physicalCoords;

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
    Trans = mesh.GetElementTransformation(elementIds[i]);
    Trans->Transform(ips[i], physicalCoords);
    double r = std::sqrt(physicalCoords[0] * physicalCoords[0] +
                         physicalCoords[1] * physicalCoords[1] +
                         physicalCoords[2] * physicalCoords[2]);
    radialPoints.push_back(r);

    int elementId = elementIds[i];
    mfem::IntegrationPoint ip = ips[i];
    if (elementId >= 0) { // Check if the point was found in an element
        sampleValue = u.GetValue(elementId, ip);
        LOG_DEBUG(logger, "Probe::getRaySolution() : Ray point {} found in element {} with r={:0.2f} and theta={:0.2f}", i, elementId, r, sampleValue);
        samples.push_back(sampleValue);
    } else { // If the point was not found in an element
        LOG_INFO(logger, "Probe::getRaySolution() : Ray point {} not found", i);
        samples.push_back(0.0); 
    }
  }
  std::pair<std::vector<double>, std::vector<double>> samplesPair(radialPoints, samples);

  if (!filename.empty()) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "r,u\n";
        for (int i = 0; i < numSamples; i++) {
            file << samplesPair.first[i] << "," << samplesPair.second[i] << "\n";
        }
        file << std::endl;
        file.close();
    } else {
      throw(std::runtime_error("Could not open file " + filename));
    }
  }

  return samplesPair;
}

std::pair<std::vector<double>, std::vector<double>> getRaySolution(mfem::Vector &vec, mfem::FiniteElementSpace &fes,
                                   const std::vector<double>& rayDirection,
                                   int numSamples, std::string filename) {
  mfem::GridFunction gf(&fes);
  DEPRECATION_WARNING_OFF
    gf.SetData(vec);
  DEPRECATION_WARNING_ON
  return getRaySolution(gf, *fes.GetMesh(), rayDirection, numSamples, filename);
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