#include "ir/objc3_ir_semantic_optimization_policy.h"

std::vector<std::string> BuildObjc3IRSemanticOptimizationPassOrder() {
  return {
      "semantic-precondition-gate",
      "nil-receiver-folding",
      "direct-dispatch-exact-call",
      "arc-retained-result-cleanup",
      "runtime-dispatch-preservation",
      "devirtualization",
      "method-inlining",
      "cache-aware-dispatch",
      "ir-cleanup-verifier",
  };
}

std::vector<Objc3IRSemanticOptimizationProofContract>
BuildObjc3IRSemanticOptimizationProofContracts() {
  return {
      {"semantic-precondition-gate",
       "typed sema handoff, lowering replay key, and parse readiness",
       "REJECT_FAIL_CLOSED",
       true,
       false,
       false,
       false},
      {"nil-receiver-folding",
       "compile_time_nil_receiver plus side-effect-free optional-send lowering",
       "SKIP_FAIL_CLOSED",
       true,
       true,
       false,
       false},
      {"direct-dispatch-exact-call",
       "exact callee symbol, signature, argument types, and result ownership",
       "REJECT_FAIL_CLOSED",
       true,
       true,
       true,
       false},
      {"arc-retained-result-cleanup",
       "ARC mode, method-family retained result, and explicit cleanup transfer",
       "SKIP_FAIL_CLOSED",
       true,
       true,
       false,
       false},
      {"runtime-dispatch-preservation",
       "canonical runtime dispatch ownership and disabled retired routes",
       "REJECT_FAIL_CLOSED",
       true,
       false,
       false,
       false},
      {"devirtualization",
       "closed-world receiver finality, closed override set, and ABI publication",
       "SKIP_FAIL_CLOSED",
       true,
       false,
       false,
       false},
      {"method-inlining",
       "callee body identity, scalar subset, ownership and side-effect summaries, source-map and diagnostic preservation, ABI/package identity, depth and recursion limits, callee generation snapshot, and invalidation completeness",
       "REJECT_FAIL_CLOSED",
       true,
       true,
       true,
       false},
      {"cache-aware-dispatch",
       "runtime cache invalidation semantics, ABI-stable helper symbol, strict status envelope, semantic miss replay, and source-map debug preservation",
       "REJECT_FAIL_CLOSED",
       true,
       true,
       true,
       false},
      {"ir-cleanup-verifier",
       "deterministic pass trace and semantic equivalence verdict",
       "REJECT_FAIL_CLOSED",
       true,
       false,
       false,
       false},
  };
}

bool IsObjc3IRSemanticOptimizationProofContractFailClosed(
    const Objc3IRSemanticOptimizationProofContract &contract,
    std::string &reason) {
  if (contract.pass_id.empty()) {
    reason = "semantic optimization proof contract is missing pass id";
    return false;
  }
  if (!contract.semantic_preserving) {
    reason = "semantic optimization proof contract is not preserving semantics";
    return false;
  }
  if (contract.required_proof.empty()) {
    reason = "semantic optimization proof contract is missing proof text";
    return false;
  }
  if (contract.missing_proof_action != "SKIP_FAIL_CLOSED" &&
      contract.missing_proof_action != "REJECT_FAIL_CLOSED") {
    reason = "semantic optimization proof contract has unsafe missing-proof action";
    return false;
  }
  if (contract.allow_success_claim_on_skip) {
    reason = "semantic optimization proof contract allows skip success claim";
    return false;
  }
  if (contract.pass_id == "direct-dispatch-exact-call" &&
      !contract.invalidates_global_proof_state) {
    reason =
        "direct dispatch semantic optimization must invalidate global proof state";
    return false;
  }
  if (contract.pass_id == "method-inlining" &&
      !contract.invalidates_global_proof_state) {
    reason =
        "method inlining semantic optimization must invalidate global proof state";
    return false;
  }
  if (contract.pass_id == "cache-aware-dispatch" &&
      !contract.invalidates_global_proof_state) {
    reason =
        "cache-aware dispatch semantic optimization must invalidate global proof state";
    return false;
  }
  reason.clear();
  return true;
}
