#pragma once

#include <string>
#include <vector>

#include "clang_foundation/interface_contract.h"

namespace objc3::clang_foundation {

struct InterfaceArtifactWriteResult {
  bool success = false;
  std::string source_path;
  std::string sidecar_path;
  std::vector<InterfaceContractDiagnostic> diagnostics;
};

// Writes a canonical .objc3i/.objc3i.json pair to the explicitly selected
// source path. The sidecar is derived by appending ".json" to that path.
// Existing files are backed up in a private sibling staging directory and
// restored if either publication step fails.
[[nodiscard]] InterfaceArtifactWriteResult WriteSemanticInterfaceArtifact(
    const InterfaceEmissionArtifact &artifact,
    const std::string &explicit_source_path);

// Publishes an already-staged compiler object together with the interface
// pair. The staged object must be a regular sibling of the final object path.
// On success the staged path has been renamed to final_object_path; on failure
// it remains available to the caller (including after rollback).
[[nodiscard]] InterfaceArtifactWriteResult
WriteSemanticInterfaceArtifactWithObject(
    const InterfaceEmissionArtifact &artifact,
    const std::string &explicit_source_path,
    const std::string &staged_object_path,
    const std::string &final_object_path);

}  // namespace objc3::clang_foundation
