#pragma once

#include <string>
#include <vector>

struct Objc3IRSemanticOptimizationProofContract {
  std::string pass_id;
  std::string required_proof;
  std::string missing_proof_action;
  bool semantic_preserving = true;
  bool rewrites_ir = false;
  bool invalidates_global_proof_state = false;
  bool allow_success_claim_on_skip = false;
};

std::vector<std::string> BuildObjc3IRSemanticOptimizationPassOrder();

std::vector<Objc3IRSemanticOptimizationProofContract>
BuildObjc3IRSemanticOptimizationProofContracts();

bool IsObjc3IRSemanticOptimizationProofContractFailClosed(
    const Objc3IRSemanticOptimizationProofContract &contract,
    std::string &reason);
