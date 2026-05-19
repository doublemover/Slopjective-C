#pragma once

#include <filesystem>
#include <string>

#include "driver/objc3_cli_options.h"

struct Objc3DriverConformanceValidationArtifactNames {
  std::string report_artifact_path;
  std::string publication_artifact_path;
  std::string validation_artifact_path;
  std::string release_evidence_operation_artifact_path;
  std::string dashboard_artifact_path;
  std::string advanced_feature_gate_artifact_path;
};

Objc3DriverConformanceValidationArtifactNames
BuildObjc3DriverConformanceValidationArtifactNames(
    const Objc3CliOptions &cli_options,
    const std::filesystem::path &publication_path);
