#pragma once

#include "lower/contracts/lowering_artifact_publication.h"

#include <string>

struct Objc3LoweringBackendHandoff {
  bool ir_requested = false;
  bool object_requested = false;
  bool manifest_requested = false;
  bool runtime_metadata_requested = false;
  bool deterministic_paths = false;
  bool object_backend_route_ready = false;
  bool artifact_publication_route_ready = false;
  bool owner_split_explicit = false;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  std::string output_directory;
  std::string emit_prefix;
  std::string ir_relative_path;
  std::string object_relative_path;
  std::string manifest_relative_path;
  std::string runtime_metadata_relative_path;
  std::string backend_handoff_owner = kObjc3LoweringBackendHandoffOwner;
  std::string backend_handoff_owner_model = kObjc3LoweringNoRetiredRouteOwnerModel;
  std::string replay_key;
};

Objc3LoweringBackendHandoff Objc3BuildLoweringBackendHandoff(
    const Objc3LoweringArtifactPlan &plan);
bool Objc3LoweringBackendHandoffIsReady(
    const Objc3LoweringBackendHandoff &handoff);
std::string Objc3LoweringBackendHandoffReplayKey(
    const Objc3LoweringBackendHandoff &handoff);
