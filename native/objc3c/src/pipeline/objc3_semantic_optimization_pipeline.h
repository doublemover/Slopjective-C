#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "pipeline/objc3_frontend_types.h"

enum class Objc3SemanticOptimizationPassMode {
  kEnabled,
  kReserved,
  kVerifierOnly,
};

struct Objc3SemanticOptimizationPassContract {
  std::string pass_id;
  std::size_t ordinal = 0;
  std::string stage;
  Objc3SemanticOptimizationPassMode mode =
      Objc3SemanticOptimizationPassMode::kReserved;
  std::string input_contract;
  std::string output_contract;
  std::vector<std::string> required_preconditions;
  std::string invalidation_contract;
  bool semantic_preserving = true;
  bool rewrites_ir = false;
  bool invalidates_global_proof_state = false;
  bool verifies_after_pass = true;
  bool fail_closed_when_preconditions_missing = true;
  bool emits_success_claim_on_skip = false;
  std::string failure_diagnostic;
};

const char *Objc3SemanticOptimizationPassModeName(
    Objc3SemanticOptimizationPassMode mode);

std::vector<Objc3SemanticOptimizationPassContract>
BuildObjc3SemanticOptimizationPassRegistry();

bool IsObjc3SemanticOptimizationPassRegistryDeterministic(
    const std::vector<Objc3SemanticOptimizationPassContract> &passes,
    std::string &reason);

bool IsObjc3SemanticOptimizationPassContractFailClosed(
    const Objc3SemanticOptimizationPassContract &pass,
    std::string &reason);

std::string BuildObjc3SemanticOptimizationPipelineKey(
    const Objc3SemanticOptimizationPipelineSurface &surface);

Objc3SemanticOptimizationPipelineSurface
BuildObjc3SemanticOptimizationPipelineSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options);

bool IsObjc3SemanticOptimizationPipelineSurfaceReady(
    const Objc3SemanticOptimizationPipelineSurface &surface,
    std::string &reason);
