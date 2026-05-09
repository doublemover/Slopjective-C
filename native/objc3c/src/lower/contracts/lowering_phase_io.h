#pragma once

#include "ast/objc3_ast_declarations.h"
#include "lower/contracts/lowering_diagnostics.h"

#include <string>
#include <vector>

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
};

struct Objc3LoweringPhaseInput {
  const Objc3Program *program = nullptr;
  std::string source_path;
  std::string module_name;
  bool arc_mode_enabled = false;
  Objc3LoweringArtifactPlan artifacts;
};

struct Objc3LoweringPhaseOutput {
  bool ready = false;
  std::vector<Objc3LoweringDiagnostic> diagnostics;
  Objc3LoweringArtifactPlan artifacts;
  std::string replay_key;
};

Objc3LoweringArtifactPlan Objc3BuildLoweringArtifactPlan(
    const std::string &output_directory, const std::string &emit_prefix,
    bool emit_ir, bool emit_object, bool emit_manifest,
    bool emit_runtime_metadata);
Objc3LoweringPhaseInput Objc3BuildLoweringPhaseInput(
    const Objc3Program &program, const std::string &source_path,
    bool arc_mode_enabled, const Objc3LoweringArtifactPlan &artifacts);
bool Objc3LoweringPhaseInputIsReady(const Objc3LoweringPhaseInput &input);
std::string Objc3LoweringArtifactPlanReplayKey(
    const Objc3LoweringArtifactPlan &plan);
std::string Objc3LoweringPhaseInputReplayKey(
    const Objc3LoweringPhaseInput &input);
std::string Objc3LoweringPhaseOutputReplayKey(
    const Objc3LoweringPhaseOutput &output);
