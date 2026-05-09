#pragma once

#include "lower/contracts/lowering_ownership_contracts.h"

#include <string>

struct Objc3LoweringArtifactPlan {
  bool emit_ir = false;
  bool emit_object = false;
  bool emit_manifest = false;
  bool emit_runtime_metadata = false;
  std::string output_directory;
  std::string emit_prefix = "module";
  std::string ir_relative_path;
  std::string object_relative_path;
  std::string manifest_relative_path;
  std::string runtime_metadata_relative_path;
  std::string publication_owner = kObjc3LoweringArtifactPublicationOwner;
  std::string publication_owner_model = kObjc3LoweringNoFallbackOwnerModel;
  bool publication_owner_explicit = false;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
};

Objc3LoweringArtifactPlan Objc3BuildLoweringArtifactPlan(
    const std::string &output_directory, const std::string &emit_prefix,
    bool emit_ir, bool emit_object, bool emit_manifest,
    bool emit_runtime_metadata);
bool Objc3LoweringArtifactPlanPublicationOwnerIsReady(
    const Objc3LoweringArtifactPlan &plan);
std::string Objc3LoweringArtifactPlanReplayKey(
    const Objc3LoweringArtifactPlan &plan);
