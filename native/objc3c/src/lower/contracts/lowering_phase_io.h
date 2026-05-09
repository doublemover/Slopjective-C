#pragma once

#include "ast/objc3_ast_declarations.h"
#include "lower/contracts/lowering_backend_handoff.h"
#include "lower/contracts/lowering_artifact_publication.h"
#include "lower/contracts/lowering_diagnostics.h"
#include "lower/contracts/runtime_metadata_handoff.h"
#include "lower/contracts/typed_sema_lowering_boundary.h"

#include <string>
#include <vector>

struct Objc3LoweringPhaseInput {
  const Objc3Program *program = nullptr;
  std::string source_path;
  std::string module_name;
  bool arc_mode_enabled = false;
  Objc3LoweringArtifactPlan artifacts;
  Objc3TypedSemaToLoweringBoundary typed_boundary;
  Objc3RuntimeMetadataLoweringHandoff runtime_metadata_handoff;
  Objc3LoweringBackendHandoff backend_handoff;
};

struct Objc3LoweringPhaseOutput {
  bool ready = false;
  std::vector<Objc3LoweringDiagnostic> diagnostics;
  Objc3LoweringArtifactPlan artifacts;
  std::string replay_key;
};

Objc3LoweringPhaseInput Objc3BuildLoweringPhaseInput(
    const Objc3Program &program, const std::string &source_path,
    bool arc_mode_enabled, const Objc3LoweringArtifactPlan &artifacts);
bool Objc3LoweringPhaseInputIsReady(const Objc3LoweringPhaseInput &input);
std::string Objc3LoweringPhaseInputReplayKey(
    const Objc3LoweringPhaseInput &input);
std::string Objc3LoweringPhaseOutputReplayKey(
    const Objc3LoweringPhaseOutput &output);
