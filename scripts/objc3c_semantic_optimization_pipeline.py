"""Validator model for the Objective-C 3.0 semantic optimization pipeline."""

from __future__ import annotations

from dataclasses import dataclass
import hashlib
from pathlib import Path
import sys
from typing import Any

SCRIPTS_ROOT = Path(__file__).resolve().parent
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_tooling.json_io import require_json_object
from objc3c_tooling.paths import ROOT, repo_rel


CONTRACT_ID = "objc3c.optimization.semantic.pipeline.v1"
PROOF_MODEL_CONTRACT_ID = "objc3c.optimization.semantic.proof_model.v1"
PROOF_CASES_CONTRACT_ID = "objc3c.optimization.semantic.proof_cases.v1"
PIPELINE_PATH = (
    ROOT / "tests" / "tooling" / "fixtures" / "semantic_optimization_pipeline" / "pipeline.json"
)
REPORT_PATH = ROOT / "tmp" / "reports" / "semantic-optimization-pipeline.json"
PROOF_MODEL_REPORT_PATH = ROOT / "tmp" / "reports" / "optimization-proof-model.json"
REQUIRED_ISSUES = {8175, 8191, 8192, 8193, 8194, 8205, 8224, 8226, 8227}
REQUIRED_SUPPORT_CLAIM = "objc3c.behavior.semantic_optimization_pipeline"
REQUIRED_METHOD_INLINING_SUPPORT_CLAIM = (
    "objc3c.behavior.optimization.method-inlining-safe-subset"
)
REQUIRED_CACHE_AWARE_SUPPORT_CLAIM = (
    "objc3c.behavior.runtime.cache-aware-dispatch"
)
REQUIRED_PASS_ORDER = [
    "semantic-precondition-gate",
    "nil-receiver-folding",
    "direct-dispatch-exact-call",
    "arc-retained-result-cleanup",
    "runtime-dispatch-preservation",
    "devirtualization",
    "method-inlining",
    "cache-aware-dispatch",
    "ir-cleanup-verifier",
]
REQUIRED_GATES = {
    "deterministic-pass-order",
    "typed-contracts",
    "explicit-invalidation",
    "post-pass-verification",
    "fail-closed-diagnostics",
    "benchmark-governance",
}
FALSE_UNSUPPORTED_POLICY_FIELDS = {
    "unsafe_flags_allowed",
    "reserved_pass_success_claims_allowed",
    "whole_program_optimization_allowed",
    "compatibility_shims_allowed",
}
FAIL_CLOSED_MISSING_PROOF_ACTIONS = {"SKIP_FAIL_CLOSED", "REJECT_FAIL_CLOSED"}
REQUIRED_CAPABILITY_ROWS = {
    "objc3c.behavior.semantic_optimization_pipeline",
    "objc3c.behavior.semantic_optimization.exact_target_devirtualization",
    "objc3c.behavior.optimization.method-inlining-safe-subset",
    "objc3c.behavior.runtime.cache-aware-dispatch",
    "objc3c.internal.semantic_optimization_pass_registry",
    "runtime.optimization.cache-aware-dispatch",
}
REQUIRED_EVIDENCE_IDS = {
    "objc3c.evidence.semantic_optimization_pipeline.fixture",
    "objc3c.evidence.semantic_optimization_pipeline.validator",
    "objc3c.evidence.semantic_optimization_pipeline.native_surface",
    "objc3c.evidence.semantic_optimization_pipeline.direct_dispatch_ir",
    "objc3c.evidence.semantic_optimization_pipeline.performance_governance",
    "objc3c.evidence.semantic_optimization_pipeline.proof_model",
    "objc3c.evidence.semantic_optimization_pipeline.proof_cases",
    "objc3c.evidence.semantic_optimization_pipeline.exact_target_devirtualization",
    "objc3c.evidence.semantic_optimization_pipeline.method_inlining",
    "objc3c.evidence.semantic_optimization_pipeline.cache_aware_dispatch_ir",
    "objc3c.evidence.semantic_optimization_pipeline.runtime_debug_safety_contract",
    "objc3c.evidence.semantic_optimization_pipeline.runtime_debug_safety_schema",
    "objc3c.evidence.semantic_optimization_pipeline.speedup_overclaim_negative_cases",
}
RESERVED_SKIP_CONTRACT_ID = "objc3c.optimization.semantic.pipeline.reserved.skip.v1"
REQUIRED_RESERVED_SKIP_DIAGNOSTIC_CODE = "O3OPT8175"
PERFORMANCE_GOVERNANCE_CONTRACT_ID = (
    "objc3c.optimization.semantic.pipeline.performance.governance.v1"
)
RUNTIME_EQUIVALENCE_CONTRACT_ID = (
    "objc3c.optimization.semantic.pipeline.runtime_equivalence.v1"
)
PERFORMANCE_BUDGET_MODEL_PATH = (
    "tests/tooling/fixtures/performance_governance/budget_model.json"
)
OPTIMIZATION_RUNTIME_DEBUG_SAFETY_CONTRACT_ID = (
    "objc3c.optimization.runtime_debug_safety.v1"
)
OPTIMIZATION_RUNTIME_DEBUG_SAFETY_UMBRELLA_CONTRACT_ID = (
    "objc3c.optimization.runtime_debug_safety.umbrella_readiness.v1"
)
OPTIMIZATION_RUNTIME_DEBUG_SAFETY_UMBRELLA_STATUS = (
    "bounded-source-owned-ready"
)
OPTIMIZATION_RUNTIME_DEBUG_SAFETY_REQUIRED_ISSUES = {8205, 8224, 8226, 8227}
OPTIMIZATION_RUNTIME_DEBUG_SAFETY_SCHEMA_PATH = (
    "schemas/objc3c-optimization-runtime-debug-safety-v1.schema.json"
)
OPTIMIZATION_RUNTIME_DEBUG_SAFETY_CONTRACT_PATH = (
    "tests/tooling/fixtures/performance_governance/optimization_runtime_debug_safety_contract.json"
)
REQUIRED_PERFORMANCE_PUBLIC_ACTIONS = {
    "benchmark-compiler-throughput",
    "benchmark-runtime-performance",
    "test-execution-replay",
    "test-lowering-runtime-stress",
    "validate-semantic-optimization-pipeline",
    "validate-performance-governance",
}
REQUIRED_RUNTIME_EQUIVALENCE_PUBLIC_ACTIONS = {
    "test-execution-replay",
    "test-lowering-runtime-stress",
    "validate-semantic-optimization-pipeline",
}
REQUIRED_PERFORMANCE_CHECKED_IN_PATHS = {
    "tests/tooling/fixtures/compiler_throughput/workload_manifest.json",
    "tests/tooling/fixtures/runtime_performance/workload_manifest.json",
    PERFORMANCE_BUDGET_MODEL_PATH,
    OPTIMIZATION_RUNTIME_DEBUG_SAFETY_SCHEMA_PATH,
    OPTIMIZATION_RUNTIME_DEBUG_SAFETY_CONTRACT_PATH,
    "tests/tooling/fixtures/semantic_optimization_pipeline/proof_cases.json",
}
REQUIRED_RUNTIME_EQUIVALENCE_CHECKED_IN_PATHS = {
    "tests/native/ir/optimization/semantic_pipeline_direct_dispatch.before.ll",
    "tests/native/ir/optimization/semantic_pipeline_direct_dispatch.after.ll",
    "tests/native/ir/runtime_calls/non_nil_receiver_runtime_call_contract.objc3",
    "tests/tooling/fixtures/stress/lowering_runtime_stress_manifest.json",
}
FORBIDDEN_PERFORMANCE_SOURCE_ROOTS = ("tmp/", "checked_outputs/")
REQUIRED_PROOF_MODEL_PUBLIC_ACTIONS = {
    "validate-semantic-optimization-pipeline",
    "validate-codegen-optimization-policy",
    "validate-optimization-proof-model",
}
REQUIRED_PROOF_CANDIDATE_INPUT_FIELDS = {
    "source_graph_node_ids",
    "type_graph_ids",
    "semantic_type_summaries",
    "runtime_metadata_ids",
    "abi_identity",
    "package_import_identity",
    "source_map_ids",
    "line_table_status",
    "ownership_summaries",
    "side_effect_summaries",
    "class_category_protocol_generation_assumptions",
    "benchmark_workload_digest",
}
REQUIRED_DEVIRTUALIZATION_CANDIDATE_INPUT_FIELDS = {
    "static_receiver_type_proof",
    "sealed_final_dispatch_evidence",
    "exact_target_method_identity",
    "mutation_generation_snapshot",
    "runtime_cache_version_dependency",
    "devirtualized_target_symbol",
}
REQUIRED_METHOD_INLINING_CANDIDATE_INPUT_FIELDS = {
    "exact_target_method_identity",
    "callee_body_identity",
    "callee_body_ir_digest",
    "inline_candidate_kind",
    "scalar_signature_proof",
    "ownership_effects_summary",
    "source_map_inline_frame_id",
    "diagnostic_location_id",
    "inlining_depth",
    "recursion_state",
    "callee_generation_snapshot",
    "inlined_target_symbol",
    "source_identity_preservation",
    "debug_identity_preservation",
    "runtime_identity_preservation",
    "runtime_invalidation_replay",
    "ir_digest_before",
    "ir_digest_after",
}
REQUIRED_PROOF_RESULT_FIELDS = {
    "decision",
    "reason",
    "missing_proofs",
    "failed_proofs",
    "ir_digest_before",
    "ir_digest_after",
    "source_map_impact",
    "runtime_metadata_impact",
    "invalidated_proof_state",
    "debug_safety_verdict",
    "ownership_safety_verdict",
    "runtime_abi_safety_verdict",
    "package_import_abi_identity_verdict",
    "exact_target_eligibility_verdict",
    "mutation_invalidation_verdict",
    "runtime_cache_version_verdict",
    "devirtualized_target_symbol",
    "runtime_cache_version_dependency",
    "inlined_target_symbol",
    "success_claim",
}
REQUIRED_PROOF_VERDICT_FIELDS = {
    "semantic_equivalence_verdict",
    "runtime_abi_safety_verdict",
    "source_map_debug_impact_verdict",
    "ownership_arc_safety_verdict",
    "package_import_abi_identity_verdict",
}
REQUIRED_PROOF_IDS = {
    "source_graph_node_identity",
    "type_graph_identity",
    "semantic_type_summary",
    "runtime_metadata_identity",
    "runtime_abi_safety",
    "package_import_abi_identity",
    "source_map_debug_identity",
    "line_table_debug_status",
    "ownership_arc_safety",
    "side_effect_summary",
    "generation_assumption_validity",
    "semantic_equivalence",
    "invalidation_completeness",
}
REQUIRED_DEVIRTUALIZATION_PROOF_IDS = {
    "sealed_final_dispatch_evidence",
    "static_receiver_type_proof",
    "exact_method_target_identity",
    "class_category_method_mutation_invalidation",
    "runtime_cache_version_dependency",
}
REQUIRED_METHOD_INLINING_PROOF_IDS = {
    "callee_body_identity",
    "scalar_inline_subset",
    "ownership_arc_effects_replay",
    "source_map_inline_frame_preservation",
    "diagnostic_location_preservation",
    "inlining_depth_recursion_limit",
    "callee_generation_invalidation",
    "source_identity_preservation",
    "debug_identity_preservation",
    "runtime_identity_preservation",
    "runtime_invalidation_replay",
}
SAFE_METHOD_INLINING_CANDIDATE_KINDS = {
    "pure-scalar-free-function",
    "final-scalar-class-method",
    "final-scalar-instance-method",
}
METHOD_INLINING_REQUIRED_SIDE_EFFECT_SUMMARIES = {
    "pure",
    "reads:none",
    "writes:none",
    "calls:none",
    "runtime_helpers:none",
    "dispatch:none",
    "allocation:none",
    "ownership_transfer:none",
    "error_behavior:none",
}
METHOD_INLINING_MAX_DEPTH = 3
METHOD_INLINING_REQUIRED_INVALIDATED_PROOFS = {
    "callee_body_identity",
    "callee_generation",
    "local_value",
    "ownership_transfer",
    "source_map_inline_frame",
    "diagnostic_location",
    "runtime_metadata_identity",
    "package_import_abi_identity",
    "source_identity_preservation",
    "debug_identity_preservation",
    "runtime_generation_dependency",
    "invalidation_replay",
}
METHOD_INLINING_SAFE_GUARD_STRATEGIES = {
    "generation-guard-fail-closed",
    "runtime-dispatch-replay-fail-closed",
}
METHOD_INLINING_REQUIRED_RUNTIME_GENERATION_PREFIXES = (
    "class-generation=",
    "category-generation=",
    "protocol-generation=",
    "callee-generation=",
)
METHOD_INLINING_REQUIRED_REPLAY_FIELDS = {
    "mutation_event",
    "optimized_site_id",
    "expected_behavior",
    "observed_behavior",
}
REQUIRED_ALL_PROOF_IDS = (
    REQUIRED_PROOF_IDS
    | REQUIRED_DEVIRTUALIZATION_PROOF_IDS
    | REQUIRED_METHOD_INLINING_PROOF_IDS
)
REQUIRED_DEVIRTUALIZATION_VERDICT_FIELDS = {
    "exact_target_eligibility_verdict",
    "mutation_invalidation_verdict",
    "runtime_cache_version_verdict",
}
SAFE_DEVIRTUALIZATION_VERDICTS = {
    "exact_target_eligibility_verdict": {"ELIGIBLE"},
    "mutation_invalidation_verdict": {"COMPLETE"},
    "runtime_cache_version_verdict": {"PINNED"},
}
DEVIRTUALIZATION_VERDICT_PROOF_IDS = {
    "exact_target_eligibility_verdict": "exact_method_target_identity",
    "mutation_invalidation_verdict": "class_category_method_mutation_invalidation",
    "runtime_cache_version_verdict": "runtime_cache_version_dependency",
}
DEVIRTUALIZATION_REQUIRED_INVALIDATED_PROOFS = {
    "receiver_static_type",
    "selector_resolution",
    "callee_body_identity",
    "class_generation",
    "category_generation",
    "method_generation",
    "runtime_cache_version",
}
REQUIRED_VERIFIER_PASSES = {
    "source-map-preservation-verifier",
    "ownership-preservation-verifier",
    "dispatch-semantic-preservation-verifier",
    "package-import-abi-safety-verifier",
    "runtime-metadata-consistency-verifier",
    "unsupported-skip-claimlessness-verifier",
    "invalidation-completeness-verifier",
    "exact-target-devirtualization-verifier",
    "runtime-cache-version-dependency-verifier",
    "method-inlining-safe-subset-verifier",
    "identity-preservation-evidence-verifier",
    "runtime-invalidation-replay-verifier",
}
REQUIRED_PROOF_CASE_IDS = {
    "direct-dispatch-full-proof-record",
    "direct-dispatch-missing-source-graph-proof",
    "nil-receiver-missing-source-map-proof",
    "direct-dispatch-runtime-abi-drift",
    "arc-retained-result-ownership-unsafe",
    "direct-dispatch-stale-package-identity",
    "devirtualization-exact-target-full-proof-record",
    "devirtualization-missing-sealed-final-evidence",
    "devirtualization-stale-mutation-generation",
    "devirtualization-runtime-cache-version-drift",
    "method-inlining-safe-scalar-function-full-proof-record",
    "method-inlining-final-method-full-proof-record",
    "method-inlining-missing-callee-body-identity",
    "method-inlining-unsafe-ownership-effects",
    "method-inlining-side-effecting-callee",
    "method-inlining-source-map-drift",
    "method-inlining-package-abi-drift",
    "method-inlining-recursion-depth-limit",
    "method-inlining-stale-callee-generation",
    "method-inlining-missing-source-identity-preservation",
    "method-inlining-missing-debug-identity-preservation",
    "method-inlining-missing-runtime-invalidation-replay",
}
REQUIRED_OPTIMIZATION_SAFETY_BOUNDARY_IDS = {
    "exact-target-devirtualization-runtime-safety",
    "method-inlining-runtime-debug-safety",
    "cache-aware-dispatch-runtime-debug-safety",
}
REQUIRED_OPTIMIZATION_SAFETY_NEGATIVE_CASE_IDS = {
    "speedup-overclaim-without-budget-record",
    "runtime-miss-speedup-overclaim",
    "generated-report-source-truth-overclaim",
    "optimization-public-workflow-evidence-missing",
    "method-inlining-missing-debug-source-map-preservation",
    "method-inlining-missing-inline-proof",
    "method-inlining-missing-runtime-invalidation-replay",
    "method-inlining-side-effecting-callee",
    "method-inlining-side-effecting-method-as-pure",
    "method-inlining-source-map-step-through-drift",
    "method-inlining-stale-dispatch-invalidation",
    "devirtualization-runtime-cache-version-drift",
}
REQUIRED_OPTIMIZATION_SAFETY_UMBRELLA_CONTRACT_PATHS = {
    OPTIMIZATION_RUNTIME_DEBUG_SAFETY_SCHEMA_PATH,
    OPTIMIZATION_RUNTIME_DEBUG_SAFETY_CONTRACT_PATH,
    "tests/tooling/fixtures/semantic_optimization_pipeline/pipeline.json",
    "tests/tooling/fixtures/semantic_optimization_pipeline/proof_cases.json",
    "tests/tooling/fixtures/semantic_optimization_pipeline/method_inlining_replay_contract.json",
    "tests/tooling/fixtures/optimization_pipeline/candidates.json",
    "tests/tooling/fixtures/performance_governance/budget_model.json",
    "tests/tooling/fixtures/performance_governance/source_surface.json",
}
REQUIRED_OPTIMIZATION_SAFETY_UMBRELLA_NEGATIVE_FIXTURE_PATHS = {
    "tests/tooling/fixtures/semantic_optimization_pipeline/negative_method_inlining_missing_inline_proof.json",
    "tests/tooling/fixtures/semantic_optimization_pipeline/negative_method_inlining_stale_dispatch_invalidation.json",
    "tests/tooling/fixtures/semantic_optimization_pipeline/negative_method_inlining_side_effecting_method_as_pure.json",
    "tests/tooling/fixtures/semantic_optimization_pipeline/negative_method_inlining_source_map_step_drift.json",
}
REQUIRED_OPTIMIZATION_SAFETY_UMBRELLA_PUBLIC_ACTIONS = (
    REQUIRED_PERFORMANCE_PUBLIC_ACTIONS
    | REQUIRED_RUNTIME_EQUIVALENCE_PUBLIC_ACTIONS
    | {
        "validate-performance-governance-runtime-contract-linkage",
    }
)
REQUIRED_OPTIMIZATION_SAFETY_UMBRELLA_CAPABILITY_ROWS = {
    "compiler.optimization.semantic-preserving-pipeline",
    "compiler.optimization.devirtualization",
    "compiler.optimization.method-inlining",
    "runtime.optimization.cache-aware-dispatch",
    "compiler.optimization.runtime-debug-safety",
}
REQUIRED_OPTIMIZATION_SAFETY_UMBRELLA_SUCCESS_TOKENS = {
    "semantic",
    "runtime",
    "source-map",
    "inline-frame",
    "stepping",
    "side-effect",
    "invalidation",
    "budget",
}
REQUIRED_OPTIMIZATION_SAFETY_UMBRELLA_FORBIDDEN_TOKENS = {
    "generated-only source map",
    "stale native debug line table",
    "stale dispatch",
    "side-effecting method",
    "missing public workflow evidence",
    "runtime miss",
}
SAFE_PROOF_VERDICTS = {
    "semantic_equivalence_verdict": {
        "PRESERVED",
        "VERIFIER_ONLY",
        "SKIPPED_UNSUPPORTED_NO_CLAIM",
    },
    "runtime_abi_safety_verdict": {
        "SAFE",
        "NO_RUNTIME_ABI_IMPACT",
        "VERIFIER_ONLY",
        "SKIPPED_UNSUPPORTED_NO_CLAIM",
    },
    "source_map_debug_impact_verdict": {
        "PRESERVED",
        "NO_DEBUG_IMPACT",
        "VERIFIER_ONLY",
        "SKIPPED_UNSUPPORTED_NO_CLAIM",
    },
    "ownership_arc_safety_verdict": {
        "SAFE",
        "NO_OWNERSHIP_IMPACT",
        "VERIFIER_ONLY",
        "SKIPPED_UNSUPPORTED_NO_CLAIM",
    },
    "package_import_abi_identity_verdict": {
        "IDENTICAL",
        "NO_PACKAGE_ABI_IMPACT",
        "VERIFIER_ONLY",
        "SKIPPED_UNSUPPORTED_NO_CLAIM",
    },
}
VERDICT_PROOF_IDS = {
    "semantic_equivalence_verdict": "semantic_equivalence",
    "runtime_abi_safety_verdict": "runtime_abi_safety",
    "source_map_debug_impact_verdict": "source_map_debug_identity",
    "ownership_arc_safety_verdict": "ownership_arc_safety",
    "package_import_abi_identity_verdict": "package_import_abi_identity",
}


@dataclass(frozen=True)
class SemanticOptimizationPipelineValidationResult:
    payload: dict[str, Any]
    failures: list[str]

    @property
    def passed(self) -> bool:
        return not self.failures


def _as_list(value: object) -> list[object]:
    return value if isinstance(value, list) else []


def _as_dict(value: object) -> dict[str, Any]:
    return value if isinstance(value, dict) else {}


def _require_path_with_tokens(path_text: str, tokens: list[object], failures: list[str]) -> None:
    path = ROOT / path_text
    if not path.is_file():
        failures.append(f"source anchor missing: {path_text}")
        return
    text = path.read_text(encoding="utf-8")
    for token in tokens:
        if not isinstance(token, str) or token not in text:
            failures.append(f"source anchor token missing from {path_text}: {token}")


def _require_fixture(path_text: str, failures: list[str]) -> None:
    if not (ROOT / path_text).is_file():
        failures.append(f"optimization fixture missing: {path_text}")


def _policy_path_label(path: Path) -> str:
    try:
        return repo_rel(path)
    except ValueError:
        return path.resolve().as_posix()


def _pass_text(row: dict[str, Any]) -> str:
    return " ".join(str(value) for value in row.values()).lower()


def _file_sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _walk_string_values(value: object) -> list[str]:
    if isinstance(value, str):
        return [value]
    if isinstance(value, dict):
        values: list[str] = []
        for item in value.values():
            values.extend(_walk_string_values(item))
        return values
    if isinstance(value, list):
        values = []
        for item in value:
            values.extend(_walk_string_values(item))
        return values
    return []


def _unique_ordered(values: list[str]) -> list[str]:
    seen: set[str] = set()
    ordered: list[str] = []
    for value in values:
        if value in seen:
            continue
        seen.add(value)
        ordered.append(value)
    return ordered


def _default_missing_proof_action(pass_id: str) -> str:
    pipeline = require_json_object(PIPELINE_PATH)
    for row in _as_list(pipeline.get("semantic_preservation_contracts")):
        if isinstance(row, dict) and row.get("pass_id") == pass_id:
            return str(row.get("missing_proof_action", "REJECT_FAIL_CLOSED"))
    return "REJECT_FAIL_CLOSED"


def _default_pass_contract(pass_id: str) -> dict[str, Any]:
    pipeline = require_json_object(PIPELINE_PATH)
    for row in _as_list(_as_dict(pipeline.get("proof_model")).get("pass_proof_contracts")):
        if isinstance(row, dict) and row.get("pass_id") == pass_id:
            return row
    return {}


def _default_pass_row(pass_id: str) -> dict[str, Any]:
    pipeline = require_json_object(PIPELINE_PATH)
    for row in _as_list(pipeline.get("pass_registry")):
        if isinstance(row, dict) and row.get("pass_id") == pass_id:
            return row
    return {}


def _proof_records_by_id(case: dict[str, Any]) -> dict[str, dict[str, Any]]:
    records: dict[str, dict[str, Any]] = {}
    for row in _as_list(case.get("proof_records")):
        if isinstance(row, dict) and isinstance(row.get("proof_id"), str):
            records[str(row["proof_id"])] = row
    return records


def _proof_decision_for_action(action: str) -> str:
    if action == "SKIP_FAIL_CLOSED":
        return "SKIPPED_FAIL_CLOSED"
    return "REJECTED_FAIL_CLOSED"


def _proof_result_diagnostic(
    diagnostic: str,
    missing_proofs: list[str],
    failed_proofs: list[str],
) -> str:
    if not missing_proofs and not failed_proofs:
        return diagnostic
    details: list[str] = []
    if missing_proofs:
        details.append("missing proofs: " + ", ".join(missing_proofs))
    if failed_proofs:
        details.append("failed proofs: " + ", ".join(failed_proofs))
    return diagnostic + "; " + "; ".join(details)


def _required_proof_ids_for_pass(pass_id: str) -> set[str]:
    if pass_id == "devirtualization":
        return REQUIRED_PROOF_IDS | REQUIRED_DEVIRTUALIZATION_PROOF_IDS
    if pass_id == "method-inlining":
        return REQUIRED_PROOF_IDS | REQUIRED_METHOD_INLINING_PROOF_IDS
    return REQUIRED_PROOF_IDS


def _method_inlining_candidate_failed_proofs(
    candidate: dict[str, Any],
    invalidation: dict[str, Any],
) -> list[str]:
    failed: list[str] = []
    exact_identity = str(candidate.get("exact_target_method_identity", ""))
    callee_identity = str(candidate.get("callee_body_identity", ""))
    inlined_symbol = str(candidate.get("inlined_target_symbol", ""))

    if (
        not exact_identity.startswith("method:")
        or not callee_identity.startswith("body:")
        or not inlined_symbol.startswith("objc3_inlineable_")
    ):
        failed.append("callee_body_identity")
    else:
        method_name = exact_identity.removeprefix("method:").split(":", 1)[0]
        symbol_name = method_name.replace(".", "_")
        if method_name not in callee_identity or symbol_name not in inlined_symbol:
            failed.append("callee_body_identity")

    if not str(candidate.get("callee_body_ir_digest", "")).startswith("sha256:"):
        failed.append("callee_body_identity")
    if not str(candidate.get("ir_digest_before", "")).startswith("sha256:"):
        failed.append("semantic_equivalence")
    if not str(candidate.get("ir_digest_after", "")).startswith("sha256:"):
        failed.append("semantic_equivalence")

    if str(candidate.get("inline_candidate_kind", "")) not in (
        SAFE_METHOD_INLINING_CANDIDATE_KINDS
    ):
        failed.append("scalar_inline_subset")
    if "no-consumed-values" not in str(candidate.get("ownership_effects_summary", "")):
        failed.append("ownership_arc_effects_replay")

    side_effects = {str(item) for item in _as_list(candidate.get("side_effect_summaries"))}
    if not METHOD_INLINING_REQUIRED_SIDE_EFFECT_SUMMARIES.issubset(side_effects):
        failed.append("side_effect_summary")

    if str(candidate.get("line_table_status", "")) != "PRESERVED":
        failed.append("line_table_debug_status")
    if not str(candidate.get("source_map_inline_frame_id", "")).startswith(
        "sm-inline-frame:"
    ):
        failed.append("source_map_inline_frame_preservation")
    if not str(candidate.get("diagnostic_location_id", "")).startswith("diag:inline:"):
        failed.append("diagnostic_location_preservation")

    try:
        depth = int(candidate.get("inlining_depth"))
    except (TypeError, ValueError):
        failed.append("inlining_depth_recursion_limit")
    else:
        if depth < 0 or depth > METHOD_INLINING_MAX_DEPTH:
            failed.append("inlining_depth_recursion_limit")
    if str(candidate.get("recursion_state", "")) != "ABSENT":
        failed.append("inlining_depth_recursion_limit")

    generation_snapshot = str(candidate.get("callee_generation_snapshot", ""))
    generation_assumptions = {
        str(item)
        for item in _as_list(candidate.get("class_category_protocol_generation_assumptions"))
    }
    if generation_snapshot not in generation_assumptions:
        failed.append("callee_generation_invalidation")

    invalidated = {str(proof) for proof in _as_list(invalidation.get("invalidated_proofs"))}
    if not METHOD_INLINING_REQUIRED_INVALIDATED_PROOFS.issubset(invalidated):
        failed.append("callee_generation_invalidation")

    return failed


def _string_set(value: object) -> set[str]:
    return {str(item) for item in _as_list(value)}


def _method_inlining_preservation_evidence_failures(
    candidate: dict[str, Any],
) -> tuple[list[str], list[str]]:
    missing: list[str] = []
    failed: list[str] = []

    candidate_source_ids = _string_set(candidate.get("source_graph_node_ids"))
    candidate_source_maps = _string_set(candidate.get("source_map_ids"))
    candidate_runtime_ids = _string_set(candidate.get("runtime_metadata_ids"))

    source_identity = _as_dict(candidate.get("source_identity_preservation"))
    if not source_identity:
        missing.append("source_identity_preservation")
    else:
        preserved_source_ids = _string_set(source_identity.get("source_graph_node_ids"))
        if (
            not preserved_source_ids
            or not preserved_source_ids.issubset(candidate_source_ids)
            or str(source_identity.get("callsite_id", "")) not in candidate_source_ids
            or str(source_identity.get("callee_body_identity", ""))
            != str(candidate.get("callee_body_identity", ""))
        ):
            failed.append("source_identity_preservation")

    debug_identity = _as_dict(candidate.get("debug_identity_preservation"))
    if not debug_identity:
        missing.append("debug_identity_preservation")
    else:
        preserved_source_maps = _string_set(debug_identity.get("source_map_ids"))
        if (
            not preserved_source_maps
            or not preserved_source_maps.issubset(candidate_source_maps)
            or str(debug_identity.get("inline_frame_id", ""))
            != str(candidate.get("source_map_inline_frame_id", ""))
            or str(debug_identity.get("diagnostic_location_id", ""))
            != str(candidate.get("diagnostic_location_id", ""))
            or str(debug_identity.get("line_table_status", ""))
            != str(candidate.get("line_table_status", ""))
        ):
            failed.append("debug_identity_preservation")

    runtime_identity = _as_dict(candidate.get("runtime_identity_preservation"))
    runtime_generation_dependencies: set[str] = set()
    if not runtime_identity:
        missing.append("runtime_identity_preservation")
    else:
        preserved_runtime_ids = _string_set(runtime_identity.get("runtime_metadata_ids"))
        runtime_generation_dependencies = _string_set(
            runtime_identity.get("runtime_generation_dependencies")
        )
        if (
            not preserved_runtime_ids
            or not preserved_runtime_ids.issubset(candidate_runtime_ids)
            or str(runtime_identity.get("abi_identity", ""))
            != str(candidate.get("abi_identity", ""))
            or str(runtime_identity.get("package_import_identity", ""))
            != str(candidate.get("package_import_identity", ""))
            or not runtime_generation_dependencies
            or not all(
                any(
                    dependency.startswith(prefix)
                    for dependency in runtime_generation_dependencies
                )
                for prefix in METHOD_INLINING_REQUIRED_RUNTIME_GENERATION_PREFIXES
            )
            or str(candidate.get("callee_generation_snapshot", ""))
            not in runtime_generation_dependencies
            or str(runtime_identity.get("guard_strategy", ""))
            not in METHOD_INLINING_SAFE_GUARD_STRATEGIES
        ):
            failed.append("runtime_identity_preservation")

    replay_rows = _as_list(candidate.get("runtime_invalidation_replay"))
    if not replay_rows:
        missing.append("runtime_invalidation_replay")
    else:
        for row in replay_rows:
            replay = _as_dict(row)
            if (
                set(replay).intersection(METHOD_INLINING_REQUIRED_REPLAY_FIELDS)
                != METHOD_INLINING_REQUIRED_REPLAY_FIELDS
            ):
                failed.append("runtime_invalidation_replay")
                continue
            if (
                any(
                    not str(replay.get(field, ""))
                    for field in METHOD_INLINING_REQUIRED_REPLAY_FIELDS
                )
                or str(replay.get("optimized_site_id", "")) not in candidate_source_ids
                or str(replay.get("expected_behavior", ""))
                != str(replay.get("observed_behavior", ""))
                or "fallback"
                in str(replay.get("expected_behavior", "")).lower()
                or "fallback"
                in str(replay.get("observed_behavior", "")).lower()
                or "fail-closed"
                not in str(replay.get("expected_behavior", "")).lower()
            ):
                failed.append("runtime_invalidation_replay")
                continue

    return _unique_ordered(missing), _unique_ordered(failed)


def evaluate_optimization_proof_case(
    case: dict[str, Any],
    *,
    pass_contract: dict[str, Any] | None = None,
    pass_row: dict[str, Any] | None = None,
    preservation_contract: dict[str, Any] | None = None,
) -> dict[str, Any]:
    """Evaluate a single optimization proof case without applying a transform."""

    pass_id = str(case.get("pass_id", ""))
    contract = pass_contract or _default_pass_contract(pass_id)
    pass_record = pass_row or _default_pass_row(pass_id)
    preservation = preservation_contract or {
        "missing_proof_action": _default_missing_proof_action(pass_id)
    }
    required_proofs = [str(proof) for proof in _as_list(contract.get("required_proof_ids"))]
    records = _proof_records_by_id(case)
    missing_proofs: list[str] = []
    failed_proofs: list[str] = []

    for proof_id in required_proofs:
        record = records.get(proof_id)
        if record is None or record.get("status") == "MISSING":
            missing_proofs.append(proof_id)
        elif record.get("status") != "PRESENT":
            failed_proofs.append(proof_id)

    verdicts = _as_dict(case.get("verdicts"))
    for verdict_field, proof_id in VERDICT_PROOF_IDS.items():
        verdict = str(verdicts.get(verdict_field, "MISSING"))
        if verdict not in SAFE_PROOF_VERDICTS[verdict_field]:
            failed_proofs.append(proof_id)
    if pass_id == "devirtualization":
        for verdict_field, proof_id in DEVIRTUALIZATION_VERDICT_PROOF_IDS.items():
            verdict = str(verdicts.get(verdict_field, "MISSING"))
            if verdict not in SAFE_DEVIRTUALIZATION_VERDICTS[verdict_field]:
                failed_proofs.append(proof_id)

    invalidation = _as_dict(case.get("invalidation"))
    if pass_record.get("rewrites_ir") is True:
        if invalidation.get("declares_invalidated_proof_state") is not True:
            failed_proofs.append("invalidation_completeness")
        if not _as_list(invalidation.get("invalidated_proofs")):
            failed_proofs.append("invalidation_completeness")
    if pass_id == "devirtualization":
        invalidated = {str(proof) for proof in _as_list(invalidation.get("invalidated_proofs"))}
        if not DEVIRTUALIZATION_REQUIRED_INVALIDATED_PROOFS.issubset(invalidated):
            failed_proofs.append("class_category_method_mutation_invalidation")
        candidate_fields = _as_dict(case.get("candidate"))
        for field in REQUIRED_DEVIRTUALIZATION_CANDIDATE_INPUT_FIELDS:
            value = candidate_fields.get(field)
            if isinstance(value, list):
                if not value:
                    missing_proofs.append(field)
            elif not str(value or ""):
                missing_proofs.append(field)
    if pass_id == "method-inlining":
        candidate_fields = _as_dict(case.get("candidate"))
        for field in REQUIRED_METHOD_INLINING_CANDIDATE_INPUT_FIELDS:
            value = candidate_fields.get(field)
            if isinstance(value, list):
                if not value:
                    missing_proofs.append(field)
            elif value is None or str(value) == "":
                missing_proofs.append(field)
        failed_proofs.extend(
            _method_inlining_candidate_failed_proofs(candidate_fields, invalidation)
        )
        preservation_missing, preservation_failed = (
            _method_inlining_preservation_evidence_failures(candidate_fields)
        )
        missing_proofs.extend(preservation_missing)
        failed_proofs.extend(preservation_failed)

    missing_proofs = _unique_ordered(missing_proofs)
    failed_proofs = _unique_ordered(failed_proofs)
    diagnostics = _as_list(pass_record.get("fail_closed_diagnostics"))
    diagnostic = str(
        contract.get("fail_closed_diagnostic")
        or (diagnostics[0] if diagnostics else "")
        or "semantic optimization proof model failed closed"
    )

    if pass_record.get("mode") == "reserved":
        decision = "SKIPPED_FAIL_CLOSED"
        success_claim = False
        reason = "reserved optimization pass skipped without success claim"
        missing_proofs = required_proofs
        failed_proofs = []
        diagnostic = _proof_result_diagnostic(diagnostic, missing_proofs, [])
    elif missing_proofs or failed_proofs:
        decision = _proof_decision_for_action(
            str(preservation.get("missing_proof_action", "REJECT_FAIL_CLOSED"))
        )
        success_claim = False
        reason = "optimization proof model failed closed"
        diagnostic = _proof_result_diagnostic(diagnostic, missing_proofs, failed_proofs)
    elif pass_record.get("mode") == "verifier-only":
        decision = "VERIFIED"
        success_claim = False
        reason = "verifier-only proof record accepted"
        diagnostic = ""
    else:
        decision = "APPLIED"
        success_claim = True
        reason = "all required optimization proofs and verdicts are present"
        diagnostic = ""

    candidate = _as_dict(case.get("candidate"))
    return {
        "case_id": str(case.get("case_id", "")),
        "pass_id": pass_id,
        "decision": decision,
        "reason": reason,
        "missing_proofs": missing_proofs,
        "failed_proofs": failed_proofs,
        "ir_digest_before": str(candidate.get("ir_digest_before", "")),
        "ir_digest_after": str(candidate.get("ir_digest_after", "")),
        "source_map_impact": str(verdicts.get("source_map_debug_impact_verdict", "")),
        "runtime_metadata_impact": str(
            _as_dict(case.get("runtime_metadata_impact")).get("verdict", "UNCHANGED")
        ),
        "invalidated_proof_state": [
            str(proof) for proof in _as_list(invalidation.get("invalidated_proofs"))
        ],
        "debug_safety_verdict": str(verdicts.get("source_map_debug_impact_verdict", "")),
        "ownership_safety_verdict": str(verdicts.get("ownership_arc_safety_verdict", "")),
        "runtime_abi_safety_verdict": str(verdicts.get("runtime_abi_safety_verdict", "")),
        "package_import_abi_identity_verdict": str(
            verdicts.get("package_import_abi_identity_verdict", "")
        ),
        "exact_target_eligibility_verdict": str(
            verdicts.get("exact_target_eligibility_verdict", "")
        ),
        "mutation_invalidation_verdict": str(
            verdicts.get("mutation_invalidation_verdict", "")
        ),
        "runtime_cache_version_verdict": str(
            verdicts.get("runtime_cache_version_verdict", "")
        ),
        "devirtualized_target_symbol": str(candidate.get("devirtualized_target_symbol", "")),
        "runtime_cache_version_dependency": str(
            candidate.get("runtime_cache_version_dependency", "")
        ),
        "inlined_target_symbol": str(candidate.get("inlined_target_symbol", "")),
        "success_claim": success_claim,
        "diagnostic": diagnostic,
    }


def _manifest_workload_ids(manifest: dict[str, Any]) -> set[str]:
    ids: set[str] = set()
    for key in ("workloads", "workload_families"):
        for row in _as_list(manifest.get(key)):
            if isinstance(row, dict) and isinstance(row.get("workload_id"), str):
                ids.add(str(row["workload_id"]))
    return ids


def _budget_metric_ids_by_family(budget_model: dict[str, Any]) -> dict[str, set[str]]:
    families: dict[str, set[str]] = {}
    for family in _as_list(budget_model.get("budget_families")):
        if not isinstance(family, dict):
            continue
        budget_id = str(family.get("budget_id", ""))
        metric_ids = {
            str(metric.get("metric_id"))
            for metric in _as_list(family.get("metric_definitions"))
            if isinstance(metric, dict) and metric.get("metric_id")
        }
        if budget_id:
            families[budget_id] = metric_ids
    return families


def _validate_runtime_equivalence_validation(
    runtime_equivalence: dict[str, Any],
    *,
    parent_public_actions: set[str],
    failures: list[str],
) -> dict[str, int]:
    if runtime_equivalence.get("contract_id") != RUNTIME_EQUIVALENCE_CONTRACT_ID:
        failures.append("semantic optimization runtime equivalence contract_id drifted")
    if runtime_equivalence.get("issue_ref") != "#8175":
        failures.append("semantic optimization runtime equivalence must bind issue #8175")

    policy_text = str(runtime_equivalence.get("source_truth_policy", "")).lower()
    if "generated reports" not in policy_text or "not source truth" not in policy_text:
        failures.append(
            "semantic optimization runtime equivalence must reject generated reports as source truth"
        )

    actions = {
        str(action)
        for action in _as_list(runtime_equivalence.get("required_public_actions"))
    }
    if not REQUIRED_RUNTIME_EQUIVALENCE_PUBLIC_ACTIONS.issubset(actions):
        failures.append("semantic optimization runtime equivalence actions incomplete")
    missing_parent_actions = REQUIRED_RUNTIME_EQUIVALENCE_PUBLIC_ACTIONS.difference(
        parent_public_actions
    )
    if missing_parent_actions:
        failures.append(
            "semantic optimization performance governance does not publish runtime "
            f"equivalence actions: {', '.join(sorted(missing_parent_actions))}"
        )

    checked_paths = {
        str(path)
        for path in _as_list(runtime_equivalence.get("checked_in_paths"))
    }
    if not REQUIRED_RUNTIME_EQUIVALENCE_CHECKED_IN_PATHS.issubset(checked_paths):
        failures.append("semantic optimization runtime equivalence checked paths incomplete")
    for checked_path in checked_paths:
        normalized = checked_path.replace("\\", "/")
        if normalized.startswith(FORBIDDEN_PERFORMANCE_SOURCE_ROOTS):
            failures.append(
                f"semantic optimization runtime equivalence checked path is generated output: {checked_path}"
            )
        elif not (ROOT / checked_path).is_file():
            failures.append(
                f"semantic optimization runtime equivalence checked path missing: {checked_path}"
            )

    case_count = 0
    for row in _as_list(runtime_equivalence.get("equivalence_cases")):
        if not isinstance(row, dict):
            failures.append("semantic optimization runtime equivalence case is not an object")
            continue
        case_count += 1
        case_id = str(row.get("case_id", ""))
        public_action = str(row.get("public_action", ""))
        if public_action not in REQUIRED_RUNTIME_EQUIVALENCE_PUBLIC_ACTIONS:
            failures.append(
                f"semantic optimization runtime equivalence case action is not public: {case_id}"
            )
        if public_action not in actions:
            failures.append(
                f"semantic optimization runtime equivalence case action is not declared: {case_id}"
            )
        if not str(row.get("semantic_equivalence_claim", "")):
            failures.append(
                f"semantic optimization runtime equivalence case missing claim: {case_id}"
            )
        source_paths = [str(path) for path in _as_list(row.get("source_paths"))]
        if not source_paths:
            failures.append(
                f"semantic optimization runtime equivalence case missing sources: {case_id}"
            )
        for source_path in source_paths:
            if source_path not in checked_paths:
                failures.append(
                    f"semantic optimization runtime equivalence case source is not checked: {case_id}"
                )
            elif not (ROOT / source_path).is_file():
                failures.append(
                    f"semantic optimization runtime equivalence source missing: {source_path}"
                )

    if case_count < 2:
        failures.append("semantic optimization runtime equivalence needs optimized and preserved cases")
    return {
        "runtime_equivalence_case_count": case_count,
        "runtime_equivalence_checked_path_count": len(checked_paths),
    }


def _proof_case_index(path_text: str, failures: list[str]) -> dict[str, dict[str, Any]]:
    path = ROOT / path_text
    if not path.is_file():
        failures.append(f"optimization proof case fixture missing: {path_text}")
        return {}
    payload = require_json_object(path)
    cases: dict[str, dict[str, Any]] = {}
    for row in _as_list(payload.get("cases")):
        if isinstance(row, dict) and isinstance(row.get("case_id"), str):
            cases[str(row["case_id"])] = row
    return cases


def _validate_method_inlining_negative_fixture(
    path_text: str,
    failures: list[str],
) -> None:
    path = ROOT / path_text
    if not path.is_file():
        failures.append(f"optimization method-inlining negative fixture missing: {path_text}")
        return
    payload = require_json_object(path)
    case_id = str(payload.get("case_id", path_text))
    contract_id = str(payload.get("contract_id", ""))
    if not contract_id.startswith("objc3c.optimization.method_inlining.negative."):
        failures.append(
            f"optimization method-inlining negative fixture contract_id drifted: {case_id}"
        )
    if payload.get("pass_id") != "method-inlining":
        failures.append(
            f"optimization method-inlining negative fixture pass drifted: {case_id}"
        )
    issue_refs: set[int] = set()
    for issue in _as_list(payload.get("issue_refs")):
        try:
            issue_refs.add(int(issue))
        except (TypeError, ValueError):
            failures.append(
                f"optimization method-inlining negative fixture invalid issue ref: {case_id}"
            )
    if not issue_refs.intersection({8224, 8226}):
        failures.append(
            f"optimization method-inlining negative fixture missing issue binding: {case_id}"
        )

    candidate = _as_dict(payload.get("candidate_overrides"))
    if not candidate:
        failures.append(
            f"optimization method-inlining negative fixture missing candidate overrides: {case_id}"
        )

    expected = _as_dict(payload.get("expected_result"))
    if expected.get("decision") != "REJECTED_FAIL_CLOSED":
        failures.append(
            f"optimization method-inlining negative fixture must reject fail-closed: {case_id}"
        )
    if expected.get("success_claim") is not False:
        failures.append(
            f"optimization method-inlining negative fixture permits success claim: {case_id}"
        )

    diagnostics = [str(item) for item in _as_list(expected.get("diagnostic_contains"))]
    if not diagnostics:
        failures.append(
            f"optimization method-inlining negative fixture missing diagnostics: {case_id}"
        )
    for diagnostic in diagnostics:
        if diagnostic not in candidate:
            failures.append(
                "optimization method-inlining negative fixture diagnostic is not tied "
                f"to a candidate gate for {case_id}: {diagnostic}"
            )
            continue
        diagnostic_value = candidate.get(diagnostic)
        if diagnostic == "method_inline_generated_only_source_map":
            if diagnostic_value is not True:
                failures.append(
                    "optimization method-inlining negative generated-only diagnostic "
                    f"is not triggered for {case_id}"
                )
        elif diagnostic_value not in (False, "", None):
            failures.append(
                "optimization method-inlining negative fixture diagnostic gate is not "
                f"failing for {case_id}: {diagnostic}"
            )


def _validate_optimization_runtime_debug_safety_contract(
    performance_governance: dict[str, Any],
    *,
    budget_metrics: dict[str, set[str]],
    failures: list[str],
) -> dict[str, int | str]:
    contract_path = str(
        performance_governance.get("optimization_runtime_debug_safety_contract", "")
    )
    if contract_path != OPTIMIZATION_RUNTIME_DEBUG_SAFETY_CONTRACT_PATH:
        failures.append("optimization runtime/debug safety contract path drifted")
    if not (ROOT / contract_path).is_file():
        failures.append(f"optimization runtime/debug safety contract missing: {contract_path}")
        return {
            "optimization_safety_contract": contract_path,
            "optimization_safety_benchmark_contract_count": 0,
            "optimization_safety_boundary_count": 0,
            "optimization_safety_negative_case_count": 0,
            "optimization_safety_evidence_path_count": 0,
            "optimization_safety_umbrella_status": "",
            "optimization_safety_umbrella_source_contract_count": 0,
            "optimization_safety_umbrella_negative_fixture_count": 0,
            "optimization_safety_umbrella_public_action_count": 0,
            "optimization_safety_umbrella_capability_row_count": 0,
        }

    contract = require_json_object(ROOT / contract_path)
    if contract.get("contract_id") != OPTIMIZATION_RUNTIME_DEBUG_SAFETY_CONTRACT_ID:
        failures.append("optimization runtime/debug safety contract_id drifted")
    issues = {int(issue) for issue in _as_list(contract.get("issue_refs"))}
    missing_issues = sorted(
        OPTIMIZATION_RUNTIME_DEBUG_SAFETY_REQUIRED_ISSUES.difference(issues)
    )
    if missing_issues:
        failures.append(
            "optimization runtime/debug safety contract missing issue bindings: "
            + ", ".join(f"#{issue}" for issue in missing_issues)
        )
    schema_path = str(contract.get("schema_path", ""))
    if schema_path != OPTIMIZATION_RUNTIME_DEBUG_SAFETY_SCHEMA_PATH:
        failures.append("optimization runtime/debug safety schema path drifted")
    if not (ROOT / schema_path).is_file():
        failures.append(f"optimization runtime/debug safety schema missing: {schema_path}")

    policy_text = str(contract.get("source_truth_policy", "")).lower()
    for token in ("generated reports", "not source truth", "debug", "runtime"):
        if token not in policy_text:
            failures.append(f"optimization runtime/debug safety policy missing token: {token}")

    proof_cases = _proof_case_index(
        "tests/tooling/fixtures/semantic_optimization_pipeline/proof_cases.json",
        failures,
    )

    benchmark_count = 0
    required_claim_inputs = {
        "checked-in-budget-metric",
        "semantic-optimization-proof-case",
        "debug-source-map-preservation",
        "runtime-invalidation-replay",
    }
    for row in _as_list(contract.get("benchmark_contract_records")):
        if not isinstance(row, dict):
            failures.append("optimization runtime/debug safety benchmark record is not an object")
            continue
        benchmark_count += 1
        record_id = str(row.get("record_id", ""))
        budget_family = str(row.get("budget_family", ""))
        metric_id = str(row.get("metric_id", ""))
        workload_id = str(row.get("workload_id", ""))
        manifest_path = str(row.get("manifest_path", ""))
        source_path = str(row.get("source_path", ""))
        proof_fixture = str(row.get("proof_fixture", ""))
        if metric_id not in budget_metrics.get(budget_family, set()):
            failures.append(f"optimization safety benchmark metric missing: {record_id}")
        if not (ROOT / source_path).is_file():
            failures.append(f"optimization safety source missing: {source_path}")
        if not (ROOT / proof_fixture).is_file():
            failures.append(f"optimization safety proof fixture missing: {proof_fixture}")
        manifest_file = ROOT / manifest_path
        if not manifest_file.is_file():
            failures.append(f"optimization safety workload manifest missing: {manifest_path}")
        else:
            manifest = require_json_object(manifest_file)
            if workload_id not in _manifest_workload_ids(manifest):
                failures.append(f"optimization safety workload not present in manifest: {workload_id}")
        claim_requires = {str(item) for item in _as_list(row.get("claim_requires"))}
        if not required_claim_inputs.issubset(claim_requires):
            failures.append(f"optimization safety benchmark claim inputs incomplete: {record_id}")
        if not _as_list(row.get("forbidden_speedup_claims")):
            failures.append(f"optimization safety benchmark record missing forbidden claims: {record_id}")

    boundary_count = 0
    boundary_ids: set[str] = set()
    for row in _as_list(contract.get("runtime_debug_safety_boundaries")):
        if not isinstance(row, dict):
            failures.append("optimization runtime/debug safety boundary is not an object")
            continue
        boundary_count += 1
        boundary_id = str(row.get("boundary_id", ""))
        boundary_ids.add(boundary_id)
        pass_id = str(row.get("pass_id", ""))
        if pass_id not in REQUIRED_PASS_ORDER:
            failures.append(f"optimization safety boundary references unknown pass: {boundary_id}")
        if row.get("fail_closed_if_missing") is not True:
            failures.append(f"optimization safety boundary must fail closed: {boundary_id}")
        required_proofs = {str(proof) for proof in _as_list(row.get("required_proof_ids"))}
        if not required_proofs:
            failures.append(f"optimization safety boundary missing proof ids: {boundary_id}")
        unknown_proofs = sorted(required_proofs.difference(REQUIRED_ALL_PROOF_IDS))
        if unknown_proofs:
            failures.append(
                f"optimization safety boundary references unknown proofs for {boundary_id}: "
                + ", ".join(unknown_proofs)
            )
        boundary_text = _pass_text(row)
        for token in ("invalidation", "debug", "source", "side-effect"):
            if token not in boundary_text:
                failures.append(f"optimization safety boundary missing token {token}: {boundary_id}")
        if pass_id == "method-inlining" and not {
            "debug_identity_preservation",
            "runtime_invalidation_replay",
            "side_effect_summary",
        }.issubset(required_proofs):
            failures.append("method-inlining safety boundary missing debug/runtime/side-effect proofs")
        if pass_id == "devirtualization" and not {
            "runtime_cache_version_dependency",
            "class_category_method_mutation_invalidation",
        }.issubset(required_proofs):
            failures.append("devirtualization safety boundary missing runtime invalidation proofs")

    missing_boundaries = sorted(REQUIRED_OPTIMIZATION_SAFETY_BOUNDARY_IDS.difference(boundary_ids))
    if missing_boundaries:
        failures.append(
            "optimization runtime/debug safety boundaries missing: "
            + ", ".join(missing_boundaries)
        )

    negative_count = 0
    negative_ids: set[str] = set()
    for row in _as_list(contract.get("fail_closed_negative_cases")):
        if not isinstance(row, dict):
            failures.append("optimization runtime/debug safety negative case is not an object")
            continue
        negative_count += 1
        case_id = str(row.get("case_id", ""))
        negative_ids.add(case_id)
        source_fixture = str(row.get("source_fixture", ""))
        if not (ROOT / source_fixture).is_file():
            failures.append(f"optimization safety negative case fixture missing: {case_id}")
        if row.get("success_claim") is not False:
            failures.append(f"optimization safety negative case permits success claim: {case_id}")
        expected_diagnostic = str(row.get("expected_diagnostic", ""))
        if not expected_diagnostic:
            failures.append(f"optimization safety negative case missing diagnostic: {case_id}")
        proof_case_id = str(row.get("proof_case_id", ""))
        if proof_case_id:
            proof_case = proof_cases.get(proof_case_id)
            if proof_case is None:
                failures.append(f"optimization safety negative case proof fixture missing case: {case_id}")
            else:
                result = evaluate_optimization_proof_case(proof_case)
                if result.get("decision") != row.get("expected_decision"):
                    failures.append(f"optimization safety negative case decision drifted: {case_id}")
                if result.get("success_claim") is not False:
                    failures.append(f"optimization safety negative proof case permits success: {case_id}")
                if expected_diagnostic not in str(result.get("diagnostic", "")):
                    failures.append(f"optimization safety negative diagnostic drifted: {case_id}")

    missing_negative_cases = sorted(
        REQUIRED_OPTIMIZATION_SAFETY_NEGATIVE_CASE_IDS.difference(negative_ids)
    )
    if missing_negative_cases:
        failures.append(
            "optimization runtime/debug safety negative cases missing: "
            + ", ".join(missing_negative_cases)
        )

    alignment = _as_dict(contract.get("capability_evidence_alignment"))
    capability_rows = {str(row) for row in _as_list(alignment.get("capability_rows"))}
    if not {
        "compiler.optimization.semantic-preserving-pipeline",
        "compiler.optimization.devirtualization",
        "compiler.optimization.method-inlining",
        "runtime.optimization.cache-aware-dispatch",
        "compiler.optimization.runtime-debug-safety",
    }.issubset(capability_rows):
        failures.append("optimization runtime/debug safety capability rows incomplete")
    evidence_paths = [str(path) for path in _as_list(alignment.get("evidence_paths"))]
    for evidence_path in evidence_paths:
        if evidence_path.replace("\\", "/").startswith(FORBIDDEN_PERFORMANCE_SOURCE_ROOTS):
            failures.append(f"optimization safety evidence path is generated output: {evidence_path}")
        elif not (ROOT / evidence_path).exists():
            failures.append(f"optimization safety evidence path missing: {evidence_path}")
    support_claim_policy = str(alignment.get("support_claim_policy", "")).lower()
    if "benchmark timing alone" not in support_claim_policy:
        failures.append("optimization runtime/debug safety support claim policy must reject timing-only claims")
    if (
        "runtime miss" not in support_claim_policy
        or "not an optimization success path" not in support_claim_policy
    ):
        failures.append(
            "optimization runtime/debug safety support claim policy must "
            "reject runtime miss success paths"
        )

    umbrella = _as_dict(contract.get("optimization_safety_umbrella_readiness"))
    if (
        umbrella.get("contract_id")
        != OPTIMIZATION_RUNTIME_DEBUG_SAFETY_UMBRELLA_CONTRACT_ID
    ):
        failures.append("optimization safety umbrella readiness contract_id drifted")
    if umbrella.get("status") != OPTIMIZATION_RUNTIME_DEBUG_SAFETY_UMBRELLA_STATUS:
        failures.append("optimization safety umbrella readiness status drifted")
    umbrella_issues = {int(issue) for issue in _as_list(umbrella.get("issue_refs"))}
    missing_umbrella_issues = sorted(
        OPTIMIZATION_RUNTIME_DEBUG_SAFETY_REQUIRED_ISSUES.difference(umbrella_issues)
    )
    if missing_umbrella_issues:
        failures.append(
            "optimization safety umbrella readiness missing issue bindings: "
            + ", ".join(f"#{issue}" for issue in missing_umbrella_issues)
        )

    source_contract_paths: set[str] = set()
    for row in _as_list(umbrella.get("source_owned_contracts")):
        if not isinstance(row, dict):
            failures.append("optimization safety umbrella source contract row is not an object")
            continue
        source_path = str(row.get("path", ""))
        if source_path:
            source_contract_paths.add(source_path)
        guard_text = _pass_text(row)
        if not (
            "fail closed" in guard_text
            or "fail-closed" in guard_text
            or "fails closed" in guard_text
        ):
            failures.append(
                f"optimization safety umbrella source contract is not fail-closed: {source_path}"
            )

    missing_contract_paths = sorted(
        REQUIRED_OPTIMIZATION_SAFETY_UMBRELLA_CONTRACT_PATHS.difference(
            source_contract_paths
        )
    )
    if missing_contract_paths:
        failures.append(
            "optimization safety umbrella source contracts missing: "
            + ", ".join(missing_contract_paths)
        )
    for source_path in source_contract_paths:
        if source_path.replace("\\", "/").startswith(FORBIDDEN_PERFORMANCE_SOURCE_ROOTS):
            failures.append(
                f"optimization safety umbrella source contract is generated output: {source_path}"
            )
        elif not (ROOT / source_path).exists():
            failures.append(
                f"optimization safety umbrella source contract missing: {source_path}"
            )

    negative_fixture_paths = {
        str(path) for path in _as_list(umbrella.get("negative_fixture_contracts"))
    }
    missing_negative_fixture_paths = sorted(
        REQUIRED_OPTIMIZATION_SAFETY_UMBRELLA_NEGATIVE_FIXTURE_PATHS.difference(
            negative_fixture_paths
        )
    )
    if missing_negative_fixture_paths:
        failures.append(
            "optimization safety umbrella negative fixture contracts missing: "
            + ", ".join(missing_negative_fixture_paths)
        )
    for negative_fixture_path in negative_fixture_paths:
        if negative_fixture_path.replace("\\", "/").startswith(
            FORBIDDEN_PERFORMANCE_SOURCE_ROOTS
        ):
            failures.append(
                "optimization safety umbrella negative fixture is generated output: "
                + negative_fixture_path
            )
        elif not (ROOT / negative_fixture_path).is_file():
            failures.append(
                f"optimization safety umbrella negative fixture missing: {negative_fixture_path}"
            )
        else:
            _validate_method_inlining_negative_fixture(
                negative_fixture_path,
                failures,
            )

    umbrella_actions = {
        str(action) for action in _as_list(umbrella.get("required_public_workflow_actions"))
    }
    missing_umbrella_actions = sorted(
        REQUIRED_OPTIMIZATION_SAFETY_UMBRELLA_PUBLIC_ACTIONS.difference(
            umbrella_actions
        )
    )
    if missing_umbrella_actions:
        failures.append(
            "optimization safety umbrella public workflow actions missing: "
            + ", ".join(missing_umbrella_actions)
        )

    success_policy = _as_dict(umbrella.get("optimizer_success_path_fail_closed_policy"))
    if success_policy.get("rejects_if_any_missing") is not True:
        failures.append("optimization safety umbrella success policy must reject missing proof")
    success_requires_text = " ".join(
        str(item).lower() for item in _as_list(success_policy.get("success_requires"))
    )
    for token in sorted(REQUIRED_OPTIMIZATION_SAFETY_UMBRELLA_SUCCESS_TOKENS):
        if token not in success_requires_text:
            failures.append(
                f"optimization safety umbrella success policy missing token: {token}"
            )
    forbidden_success_text = " ".join(
        str(item).lower() for item in _as_list(success_policy.get("forbidden_success_paths"))
    )
    for token in sorted(REQUIRED_OPTIMIZATION_SAFETY_UMBRELLA_FORBIDDEN_TOKENS):
        if token not in forbidden_success_text:
            failures.append(
                f"optimization safety umbrella forbidden success path missing: {token}"
            )

    umbrella_rows: set[str] = set()
    for row in _as_list(umbrella.get("capability_row_recommendations")):
        if not isinstance(row, dict):
            failures.append("optimization safety umbrella capability row is not an object")
            continue
        capability_id = str(row.get("capability_id", ""))
        umbrella_rows.add(capability_id)
        support_level = str(row.get("support_level", ""))
        if support_level == "full":
            failures.append(
                f"optimization safety umbrella overclaims full support: {capability_id}"
            )
        if support_level not in {"bounded-supported", "reserved"}:
            failures.append(
                f"optimization safety umbrella support level drifted: {capability_id}"
            )
        row_evidence_paths = [
            str(path) for path in _as_list(row.get("evidence_paths"))
        ]
        if support_level == "bounded-supported" and not row_evidence_paths:
            failures.append(
                f"optimization safety umbrella row missing evidence paths: {capability_id}"
            )
        for row_evidence_path in row_evidence_paths:
            if row_evidence_path.replace("\\", "/").startswith(
                FORBIDDEN_PERFORMANCE_SOURCE_ROOTS
            ):
                failures.append(
                    "optimization safety umbrella capability evidence is generated output: "
                    + row_evidence_path
                )
            elif not (ROOT / row_evidence_path).exists():
                failures.append(
                    "optimization safety umbrella capability evidence missing: "
                    + row_evidence_path
                )

    missing_umbrella_rows = sorted(
        REQUIRED_OPTIMIZATION_SAFETY_UMBRELLA_CAPABILITY_ROWS.difference(
            umbrella_rows
        )
    )
    if missing_umbrella_rows:
        failures.append(
            "optimization safety umbrella capability rows missing: "
            + ", ".join(missing_umbrella_rows)
        )

    reserved_full_claims = {
        str(claim) for claim in _as_list(umbrella.get("reserved_full_claims"))
    }
    if "runtime.object-model.full-realization" not in reserved_full_claims:
        failures.append(
            "optimization safety umbrella must reserve runtime.object-model.full-realization"
        )

    return {
        "optimization_safety_contract": contract_path,
        "optimization_safety_benchmark_contract_count": benchmark_count,
        "optimization_safety_boundary_count": boundary_count,
        "optimization_safety_negative_case_count": negative_count,
        "optimization_safety_evidence_path_count": len(evidence_paths),
        "optimization_safety_umbrella_status": str(umbrella.get("status", "")),
        "optimization_safety_umbrella_source_contract_count": len(source_contract_paths),
        "optimization_safety_umbrella_negative_fixture_count": len(negative_fixture_paths),
        "optimization_safety_umbrella_public_action_count": len(umbrella_actions),
        "optimization_safety_umbrella_capability_row_count": len(umbrella_rows),
    }


def _validate_performance_governance(
    performance_governance: dict[str, Any],
    failures: list[str],
) -> dict[str, int | str]:
    if performance_governance.get("contract_id") != PERFORMANCE_GOVERNANCE_CONTRACT_ID:
        failures.append("semantic optimization performance governance contract_id drifted")

    policy_text = str(performance_governance.get("source_truth_policy", "")).lower()
    if "generated reports" not in policy_text or "not source truth" not in policy_text:
        failures.append(
            "semantic optimization performance governance must reject generated reports as source truth"
        )

    forbidden_roots = tuple(
        str(root).replace("\\", "/")
        for root in _as_list(performance_governance.get("forbidden_input_roots"))
        if isinstance(root, str)
    )
    if forbidden_roots != FORBIDDEN_PERFORMANCE_SOURCE_ROOTS:
        failures.append("semantic optimization forbidden performance input roots drifted")
    for value in _walk_string_values(performance_governance):
        normalized = value.replace("\\", "/")
        if normalized in FORBIDDEN_PERFORMANCE_SOURCE_ROOTS:
            continue
        if any(normalized.startswith(root) for root in FORBIDDEN_PERFORMANCE_SOURCE_ROOTS):
            failures.append(
                f"semantic optimization performance governance uses generated-report input: {value}"
            )

    public_actions = {
        str(action) for action in _as_list(performance_governance.get("required_public_actions"))
    }
    if not REQUIRED_PERFORMANCE_PUBLIC_ACTIONS.issubset(public_actions):
        failures.append("semantic optimization performance governance public actions incomplete")

    checked_paths = {
        str(path) for path in _as_list(performance_governance.get("required_checked_in_paths"))
    }
    if not REQUIRED_PERFORMANCE_CHECKED_IN_PATHS.issubset(checked_paths):
        failures.append("semantic optimization performance checked-in paths incomplete")
    for checked_path in checked_paths:
        if checked_path.replace("\\", "/").startswith(FORBIDDEN_PERFORMANCE_SOURCE_ROOTS):
            failures.append(
                f"semantic optimization performance checked path is generated output: {checked_path}"
            )
        elif not (ROOT / checked_path).is_file():
            failures.append(f"semantic optimization performance checked path missing: {checked_path}")

    budget_model_path = str(performance_governance.get("budget_model", ""))
    if budget_model_path != PERFORMANCE_BUDGET_MODEL_PATH:
        failures.append("semantic optimization performance budget model path drifted")
    budget_metrics = (
        _budget_metric_ids_by_family(require_json_object(ROOT / budget_model_path))
        if (ROOT / budget_model_path).is_file()
        else {}
    )
    optimization_safety_counts = _validate_optimization_runtime_debug_safety_contract(
        performance_governance,
        budget_metrics=budget_metrics,
        failures=failures,
    )

    workload_count = 0
    digest_count = 0
    for row in _as_list(performance_governance.get("workload_evidence")):
        if not isinstance(row, dict):
            failures.append("semantic optimization performance workload row is not an object")
            continue
        workload_count += 1
        workload_id = str(row.get("workload_id", ""))
        manifest_path = str(row.get("manifest_path", ""))
        source_path = str(row.get("source_path", ""))
        budget_family = str(row.get("budget_family", ""))
        metric_id = str(row.get("metric_id", ""))
        source_digest = str(row.get("source_sha256", "")).lower()

        if not source_digest:
            failures.append(f"semantic optimization workload missing source digest: {workload_id}")
        elif len(source_digest) != 64:
            failures.append(f"semantic optimization workload digest is not sha256: {workload_id}")

        manifest_file = ROOT / manifest_path
        if not manifest_file.is_file():
            failures.append(f"semantic optimization workload manifest missing: {manifest_path}")
        else:
            manifest = require_json_object(manifest_file)
            if workload_id not in _manifest_workload_ids(manifest):
                failures.append(
                    f"semantic optimization workload not present in manifest: {workload_id}"
                )

        source_file = ROOT / source_path
        if not source_file.is_file():
            failures.append(f"semantic optimization workload source missing: {source_path}")
        elif source_digest and _file_sha256(source_file) != source_digest:
            failures.append(f"semantic optimization workload digest drifted: {workload_id}")
        elif source_digest:
            digest_count += 1

        if metric_id not in budget_metrics.get(budget_family, set()):
            failures.append(
                f"semantic optimization workload budget metric missing: {workload_id}"
            )

    trace_count = 0
    for row in _as_list(performance_governance.get("semantic_trace_evidence")):
        if not isinstance(row, dict):
            failures.append("semantic optimization trace row is not an object")
            continue
        trace_count += 1
        trace_id = str(row.get("trace_id", ""))
        source_path = str(row.get("source_path", ""))
        source_digest = str(row.get("source_sha256", "")).lower()
        if not source_digest:
            failures.append(f"semantic optimization trace missing source digest: {trace_id}")
        elif len(source_digest) != 64:
            failures.append(f"semantic optimization trace digest is not sha256: {trace_id}")

        source_file = ROOT / source_path
        if not source_file.is_file():
            failures.append(f"semantic optimization trace source missing: {source_path}")
        elif source_digest and _file_sha256(source_file) != source_digest:
            failures.append(f"semantic optimization trace digest drifted: {trace_id}")
        elif source_digest:
            digest_count += 1

    runtime_equivalence_counts = _validate_runtime_equivalence_validation(
        _as_dict(performance_governance.get("runtime_equivalence_validation")),
        parent_public_actions=public_actions,
        failures=failures,
    )
    return {
        "performance_workload_count": workload_count,
        "performance_trace_count": trace_count,
        "performance_digest_count": digest_count,
        **runtime_equivalence_counts,
        **optimization_safety_counts,
    }


def _validate_pass_registry(
    passes: list[object],
    failures: list[str],
) -> dict[str, dict[str, Any]]:
    pass_by_id: dict[str, dict[str, Any]] = {}
    previous_ordinal = -1
    actual_order: list[str] = []

    for row in passes:
        if not isinstance(row, dict):
            failures.append("pass registry row is not an object")
            continue
        pass_id = str(row.get("pass_id", ""))
        if not pass_id:
            failures.append("pass registry row is missing pass_id")
            continue
        if pass_id in pass_by_id:
            failures.append(f"duplicate optimization pass id: {pass_id}")
            continue
        pass_by_id[pass_id] = row
        actual_order.append(pass_id)

        ordinal = row.get("ordinal")
        if not isinstance(ordinal, int):
            failures.append(f"optimization pass ordinal is not an integer: {pass_id}")
        elif ordinal <= previous_ordinal:
            failures.append("optimization pass ordinals must be strictly increasing")
        else:
            previous_ordinal = ordinal

        if row.get("semantic_preserving") is not True:
            failures.append(f"optimization pass is not semantic-preserving: {pass_id}")
        if str(row.get("mode", "")) not in {"enabled", "reserved", "verifier-only"}:
            failures.append(f"optimization pass has invalid mode: {pass_id}")
        for required_field in (
            "stage",
            "input_contract",
            "output_contract",
            "invalidation",
        ):
            if not str(row.get(required_field, "")):
                failures.append(f"optimization pass missing {required_field}: {pass_id}")
        for required_list in ("preconditions", "post_verify", "fail_closed_diagnostics"):
            if not _as_list(row.get(required_list)):
                failures.append(f"optimization pass missing {required_list}: {pass_id}")
        for fixture in _as_list(row.get("fixtures")):
            if isinstance(fixture, str):
                _require_fixture(fixture, failures)
            else:
                failures.append(f"optimization pass fixture is not a string: {pass_id}")

        if row.get("mode") == "reserved":
            if row.get("rewrites_ir") is not False:
                failures.append(f"reserved pass must not rewrite IR: {pass_id}")
            if "reserved" not in _pass_text(row) and "until" not in _pass_text(row):
                failures.append(f"reserved pass missing explicit reservation text: {pass_id}")

    missing = [pass_id for pass_id in REQUIRED_PASS_ORDER if pass_id not in pass_by_id]
    if missing:
        failures.append(f"semantic optimization pass registry missing passes: {', '.join(missing)}")
    if actual_order != REQUIRED_PASS_ORDER:
        failures.append(
            "semantic optimization pass order drifted: "
            + ", ".join(actual_order)
        )

    direct_pass = pass_by_id.get("direct-dispatch-exact-call", {})
    if direct_pass.get("invalidates_global_proof_state") is not True:
        failures.append("direct dispatch pass must invalidate global proof state")
    if "exact" not in _pass_text(direct_pass) or "ownership" not in _pass_text(direct_pass):
        failures.append("direct dispatch pass must require exact symbol/signature ownership")

    cache_pass = pass_by_id.get("cache-aware-dispatch", {})
    if "runtime-owned" not in _pass_text(cache_pass):
        failures.append("cache-aware dispatch pass must remain runtime-owned")
    if cache_pass.get("mode") != "enabled":
        failures.append("cache-aware dispatch pass must be enabled when the runtime ABI is present")
    if cache_pass.get("rewrites_ir") is not True:
        failures.append("cache-aware dispatch pass must rewrite IR")
    if cache_pass.get("invalidates_global_proof_state") is not True:
        failures.append("cache-aware dispatch pass must invalidate global proof state")
    if (
        "tests/native/ir/optimization/semantic_pipeline_cache_aware_dispatch.ll"
        not in _as_list(cache_pass.get("fixtures"))
    ):
        failures.append("cache-aware dispatch enabled pass must cite cache-aware IR fixture")
    cache_text = _pass_text(cache_pass)
    for token in ("helper", "strict", "source-map", "semantic replay"):
        if token not in cache_text:
            failures.append(f"cache-aware dispatch pass missing proof token: {token}")

    devirt_pass = pass_by_id.get("devirtualization", {})
    devirt_text = _pass_text(devirt_pass)
    if devirt_pass.get("mode") != "enabled":
        failures.append("devirtualization pass must be enabled for exact-target optimization")
    if devirt_pass.get("rewrites_ir") is not True:
        failures.append("devirtualization pass must rewrite IR")
    if devirt_pass.get("invalidates_global_proof_state") is not True:
        failures.append("devirtualization pass must invalidate global proof state")
    for token in ("sealed", "final", "static receiver", "mutation", "runtime cache", "source-map", "abi"):
        if token not in devirt_text:
            failures.append(f"devirtualization pass missing exact-target proof token: {token}")

    inline_pass = pass_by_id.get("method-inlining", {})
    inline_text = _pass_text(inline_pass)
    if inline_pass.get("mode") != "enabled":
        failures.append("method inlining pass must be enabled for the safe scalar subset")
    if inline_pass.get("rewrites_ir") is not True:
        failures.append("method inlining pass must rewrite IR")
    if inline_pass.get("invalidates_global_proof_state") is not True:
        failures.append("method inlining pass must invalidate global proof state")
    inline_fixtures = {str(fixture) for fixture in _as_list(inline_pass.get("fixtures"))}
    if not {
        "tests/native/ir/optimization/semantic_pipeline_method_inlining.before.ll",
        "tests/native/ir/optimization/semantic_pipeline_method_inlining.after.ll",
        "tests/tooling/fixtures/semantic_optimization_pipeline/proof_cases.json",
    }.issubset(inline_fixtures):
        failures.append("method inlining enabled pass must cite before/after IR and proof-case fixtures")
    for token in (
        "exact callee",
        "callee body identity",
        "ownership",
        "side-effect",
        "source-map",
        "diagnostic",
        "abi",
        "depth",
        "recursion",
        "generation",
        "invalidation",
    ):
        if token not in inline_text:
            failures.append(f"method inlining pass missing proof token: {token}")

    return pass_by_id


def _validate_pass_order_contract(
    order_contract: dict[str, Any],
    pass_by_id: dict[str, dict[str, Any]],
    failures: list[str],
) -> list[str]:
    authority = str(order_contract.get("order_authority", ""))
    ordered_pass_ids = [
        str(pass_id) for pass_id in _as_list(order_contract.get("ordered_pass_ids"))
    ]
    if authority != "native/objc3c/src/ir/objc3_ir_semantic_optimization_policy.cpp":
        failures.append("semantic optimization pass order authority drifted")
    if order_contract.get("drift_policy") != "REJECT_FAIL_CLOSED":
        failures.append(
            "semantic optimization pass order drift policy must reject fail-closed"
        )
    if not str(order_contract.get("ordering_rule", "")):
        failures.append("semantic optimization pass order contract is missing ordering_rule")
    if ordered_pass_ids != REQUIRED_PASS_ORDER:
        failures.append(
            "semantic optimization explicit pass order contract drifted: "
            + ", ".join(ordered_pass_ids)
        )
    if ordered_pass_ids != list(pass_by_id):
        failures.append("semantic optimization pass registry and order contract disagree")

    authority_path = ROOT / authority
    if authority_path.is_file():
        authority_text = authority_path.read_text(encoding="utf-8")
        for pass_id in REQUIRED_PASS_ORDER:
            if pass_id not in authority_text:
                failures.append(
                    f"native IR optimization order surface missing pass: {pass_id}"
                )
        for token in ("REJECT_FAIL_CLOSED", "SKIP_FAIL_CLOSED"):
            if token not in authority_text:
                failures.append(
                    f"native IR optimization policy missing fail-closed token: {token}"
                )
    else:
        failures.append(f"semantic optimization pass order authority missing: {authority}")
    return ordered_pass_ids


def _validate_semantic_preservation_contracts(
    contracts: list[object],
    pass_by_id: dict[str, dict[str, Any]],
    failures: list[str],
) -> dict[str, dict[str, Any]]:
    contracts_by_id: dict[str, dict[str, Any]] = {}
    actual_order: list[str] = []
    for row in contracts:
        if not isinstance(row, dict):
            failures.append("semantic preservation contract row is not an object")
            continue
        pass_id = str(row.get("pass_id", ""))
        if not pass_id:
            failures.append("semantic preservation contract missing pass_id")
            continue
        if pass_id in contracts_by_id:
            failures.append(f"duplicate semantic preservation contract: {pass_id}")
            continue
        contracts_by_id[pass_id] = row
        actual_order.append(pass_id)

        if pass_id not in pass_by_id:
            failures.append(
                f"semantic preservation contract has no pass registry row: {pass_id}"
            )
            continue
        pass_row = pass_by_id[pass_id]
        required_proofs = [str(proof) for proof in _as_list(row.get("required_proofs"))]
        if not required_proofs:
            failures.append(
                f"semantic preservation contract missing required_proofs: {pass_id}"
            )
        if row.get("missing_proof_action") not in FAIL_CLOSED_MISSING_PROOF_ACTIONS:
            failures.append(
                f"semantic preservation contract has unsafe missing proof action: {pass_id}"
            )
        if row.get("success_claim_on_skip") is not False:
            failures.append(f"semantic preservation contract allows skip success claim: {pass_id}")
        if not str(row.get("semantic_equivalence", "")):
            failures.append(f"semantic preservation contract missing equivalence text: {pass_id}")
        pass_contract_text = {
            str(proof)
            for proof in (
                _as_list(pass_row.get("preconditions"))
                + _as_list(pass_row.get("post_verify"))
                + [pass_row.get("invalidation")]
            )
        }
        if not set(required_proofs).issubset(pass_contract_text):
            failures.append(
                f"semantic preservation contract proofs drift from pass registry contract: {pass_id}"
            )
        if (
            pass_row.get("mode") == "reserved"
            and row.get("missing_proof_action") != "SKIP_FAIL_CLOSED"
        ):
            failures.append(f"reserved optimization pass must skip fail-closed: {pass_id}")
        if pass_row.get("mode") == "enabled" and pass_row.get("rewrites_ir") is True:
            if row.get("missing_proof_action") not in FAIL_CLOSED_MISSING_PROOF_ACTIONS:
                failures.append(f"mutating optimization pass lacks fail-closed action: {pass_id}")
        if pass_id == "direct-dispatch-exact-call":
            proof_text = " ".join(
                required_proofs + [str(row.get("semantic_equivalence", ""))]
            ).lower()
            if "invalidation" not in proof_text and "invalidates" not in proof_text:
                failures.append(
                    "direct dispatch preservation contract must require proof invalidation"
                )
        if pass_id == "devirtualization":
            proof_text = " ".join(
                required_proofs + [str(row.get("semantic_equivalence", ""))]
            ).lower()
            for token in ("sealed", "static receiver", "mutation", "runtime cache", "ownership", "source-map", "abi"):
                if token not in proof_text:
                    failures.append(
                        f"devirtualization preservation contract missing exact-target proof token: {token}"
                    )
        if pass_id == "method-inlining":
            proof_text = " ".join(
                required_proofs + [str(row.get("semantic_equivalence", ""))]
            ).lower()
            for token in (
                "callee body identity",
                "ownership",
                "side-effect",
                "source-map",
                "diagnostic",
                "abi",
                "depth",
                "recursion",
                "generation",
                "invalidation",
            ):
                if token not in proof_text:
                    failures.append(
                        f"method inlining preservation contract missing proof token: {token}"
                    )

    missing = [pass_id for pass_id in REQUIRED_PASS_ORDER if pass_id not in contracts_by_id]
    if missing:
        failures.append(f"semantic preservation contracts missing passes: {', '.join(missing)}")
    if actual_order != REQUIRED_PASS_ORDER:
        failures.append(
            "semantic preservation contract order drifted: "
            + ", ".join(actual_order)
        )
    return contracts_by_id


def _validate_direct_dispatch_fixture(failures: list[str]) -> None:
    before = ROOT / "tests/native/ir/optimization/semantic_pipeline_direct_dispatch.before.ll"
    after = ROOT / "tests/native/ir/optimization/semantic_pipeline_direct_dispatch.after.ll"
    if before.is_file():
        text = before.read_text(encoding="utf-8")
        if "objc3_runtime_dispatch_i32" not in text:
            failures.append("direct dispatch before fixture must include runtime dispatch call")
    else:
        failures.append("direct dispatch before fixture missing")
    if after.is_file():
        text = after.read_text(encoding="utf-8")
        for token in (
            "objc3_direct_Sample_isReady",
            "semantic-optimization.invalidate-global-proof-state",
        ):
            if token not in text:
                failures.append(f"direct dispatch after fixture missing token: {token}")
    else:
        failures.append("direct dispatch after fixture missing")


def _validate_method_inlining_ir_fixture(failures: list[str]) -> None:
    before_path = ROOT / "tests/native/ir/optimization/semantic_pipeline_method_inlining.before.ll"
    after_path = ROOT / "tests/native/ir/optimization/semantic_pipeline_method_inlining.after.ll"
    if not before_path.is_file():
        failures.append("method inlining before fixture missing")
        return
    if not after_path.is_file():
        failures.append("method inlining after fixture missing")
        return

    before = before_path.read_text(encoding="utf-8")
    after = after_path.read_text(encoding="utf-8")
    for token in (
        "call i32 @objc3_inlineable_InlineMath_addOne",
        "callee body identity: body:InlineMath.addOne:v1",
        "exact callee identity: method:InlineMath.addOne:i32->i32",
        "original call source span: source-span:method-inline:original-callsite",
    ):
        if token not in before:
            failures.append(f"method inlining before fixture missing token: {token}")
    for token in (
        "add nsw i32 %value, 1",
        "inlined callee body identity: body:InlineMath.addOne:v1",
        "exact callee identity preserved: method:InlineMath.addOne:i32->i32",
        "source-map inline frame preserved",
        "diagnostic location preserved",
        "semantic-optimization.invalidate-global-proof-state",
    ):
        if token not in after:
            failures.append(f"method inlining after fixture missing token: {token}")
    if "call i32 @objc3_inlineable_InlineMath_addOne" in after:
        failures.append("method inlining after fixture must remove the original call")
    if "fallback" in (before + after).lower():
        failures.append("method inlining IR fixtures must use fail-closed invalidation language, not fallback")


def _validate_reserved_skip_fixtures(
    pass_by_id: dict[str, dict[str, Any]],
    preservation_contracts: dict[str, dict[str, Any]],
    failures: list[str],
) -> int:
    fixture_count = 0
    for pass_id, pass_row in pass_by_id.items():
        if pass_row.get("mode") != "reserved":
            continue

        fixture_paths = [
            fixture
            for fixture in _as_list(pass_row.get("fixtures"))
            if isinstance(fixture, str)
        ]
        if len(fixture_paths) != 1:
            failures.append(f"reserved optimization pass must have exactly one skip fixture: {pass_id}")
            continue

        fixture_path = fixture_paths[0]
        path = ROOT / fixture_path
        if not path.is_file():
            failures.append(f"reserved optimization skip fixture missing for {pass_id}: {fixture_path}")
            continue

        fixture_count += 1
        payload = require_json_object(path)
        if payload.get("contract_id") != RESERVED_SKIP_CONTRACT_ID:
            failures.append(f"reserved optimization skip fixture contract drifted: {fixture_path}")
        if payload.get("pass_id") != pass_id:
            failures.append(
                f"reserved optimization skip fixture pass_id mismatch for {pass_id}: {fixture_path}"
            )
        if payload.get("status") != "SKIPPED_FAIL_CLOSED":
            failures.append(f"reserved optimization skip fixture must be SKIPPED_FAIL_CLOSED: {pass_id}")
        if payload.get("success_claim") is not False:
            failures.append(f"reserved optimization skip fixture must not emit success claim: {pass_id}")
        if payload.get("diagnostic_code") != REQUIRED_RESERVED_SKIP_DIAGNOSTIC_CODE:
            failures.append(f"reserved optimization skip fixture diagnostic code drifted: {pass_id}")

        diagnostics = {str(value) for value in _as_list(pass_row.get("fail_closed_diagnostics"))}
        if str(payload.get("diagnostic", "")) not in diagnostics:
            failures.append(
                f"reserved optimization skip fixture diagnostic is not registered on pass: {pass_id}"
            )

        expected_proofs = [
            str(proof)
            for proof in _as_list(
                preservation_contracts.get(pass_id, {}).get("required_proofs")
            )
        ]
        actual_proofs = [str(proof) for proof in _as_list(payload.get("required_missing_proofs"))]
        if not actual_proofs:
            failures.append(f"reserved optimization skip fixture must list missing proofs: {pass_id}")
        if actual_proofs != expected_proofs:
            failures.append(
                f"reserved optimization skip fixture missing proofs drift from preservation contract: {pass_id}"
            )

    return fixture_count


def _validate_proof_case_fixture(
    proof_case_path: str,
    *,
    proof_model: dict[str, Any],
    pass_by_id: dict[str, dict[str, Any]],
    preservation_contracts: dict[str, dict[str, Any]],
    proof_contracts_by_pass: dict[str, dict[str, Any]],
    failures: list[str],
) -> dict[str, Any]:
    path = ROOT / proof_case_path
    if not path.is_file():
        failures.append(f"optimization proof case fixture missing: {proof_case_path}")
        return {
            "proof_case_count": 0,
            "proof_case_decisions": {},
            "proof_case_path": proof_case_path,
        }

    payload = require_json_object(path)
    if payload.get("contract_id") != PROOF_CASES_CONTRACT_ID:
        failures.append("optimization proof case fixture contract_id drifted")
    if payload.get("proof_model_contract_id") != proof_model.get("contract_id"):
        failures.append("optimization proof case fixture is not bound to proof model")

    seen_case_ids: set[str] = set()
    decisions: dict[str, str] = {}
    for row in _as_list(payload.get("cases")):
        if not isinstance(row, dict):
            failures.append("optimization proof case row is not an object")
            continue
        case_id = str(row.get("case_id", ""))
        pass_id = str(row.get("pass_id", ""))
        if not case_id:
            failures.append("optimization proof case missing case_id")
            continue
        if case_id in seen_case_ids:
            failures.append(f"duplicate optimization proof case id: {case_id}")
            continue
        seen_case_ids.add(case_id)
        if pass_id not in pass_by_id:
            failures.append(f"optimization proof case pass is not registered: {case_id}")
            continue

        candidate = _as_dict(row.get("candidate"))
        missing_candidate_fields = sorted(
            REQUIRED_PROOF_CANDIDATE_INPUT_FIELDS.difference(candidate)
        )
        if missing_candidate_fields:
            failures.append(
                f"optimization proof case candidate fields missing for {case_id}: "
                + ", ".join(missing_candidate_fields)
            )
        for field in (
            "source_graph_node_ids",
            "type_graph_ids",
            "runtime_metadata_ids",
            "source_map_ids",
        ):
            if not _as_list(candidate.get(field)):
                failures.append(
                    f"optimization proof case candidate lacks {field}: {case_id}"
                )
        if not str(candidate.get("abi_identity", "")):
            failures.append(f"optimization proof case candidate lacks abi_identity: {case_id}")
        if not str(candidate.get("package_import_identity", "")):
            failures.append(
                f"optimization proof case candidate lacks package_import_identity: {case_id}"
            )
        if pass_id == "devirtualization":
            missing_devirt_fields = sorted(
                REQUIRED_DEVIRTUALIZATION_CANDIDATE_INPUT_FIELDS.difference(candidate)
            )
            if missing_devirt_fields:
                failures.append(
                    f"devirtualization proof case candidate fields missing for {case_id}: "
                    + ", ".join(missing_devirt_fields)
                )
            for field in REQUIRED_DEVIRTUALIZATION_CANDIDATE_INPUT_FIELDS:
                value = candidate.get(field)
                if isinstance(value, list):
                    empty = not value
                else:
                    empty = not str(value or "")
                if empty:
                    failures.append(
                        f"devirtualization proof case candidate lacks {field}: {case_id}"
                    )
        if pass_id == "method-inlining":
            missing_inline_fields = sorted(
                REQUIRED_METHOD_INLINING_CANDIDATE_INPUT_FIELDS.difference(candidate)
            )
            if missing_inline_fields:
                failures.append(
                    f"method inlining proof case candidate fields missing for {case_id}: "
                    + ", ".join(missing_inline_fields)
                )
            for field in REQUIRED_METHOD_INLINING_CANDIDATE_INPUT_FIELDS:
                value = candidate.get(field)
                if isinstance(value, list):
                    empty = not value
                else:
                    empty = value is None or str(value) == ""
                if empty:
                    failures.append(
                        f"method inlining proof case candidate lacks {field}: {case_id}"
                    )

        result = evaluate_optimization_proof_case(
            row,
            pass_contract=proof_contracts_by_pass.get(pass_id, {}),
            pass_row=pass_by_id.get(pass_id, {}),
            preservation_contract=preservation_contracts.get(pass_id, {}),
        )
        decisions[case_id] = str(result["decision"])
        expected = _as_dict(row.get("expected_result"))
        for field in (
            "decision",
            "success_claim",
            "missing_proofs",
            "failed_proofs",
        ):
            if result.get(field) != expected.get(field):
                failures.append(
                    f"optimization proof case expected {field} drifted for {case_id}"
                )
        diagnostic_contains = str(expected.get("diagnostic_contains", ""))
        if diagnostic_contains and diagnostic_contains not in str(result.get("diagnostic", "")):
            failures.append(
                f"optimization proof case diagnostic missing expected text for {case_id}"
            )

    missing_cases = sorted(REQUIRED_PROOF_CASE_IDS.difference(seen_case_ids))
    if missing_cases:
        failures.append(
            "optimization proof case fixture missing cases: " + ", ".join(missing_cases)
        )

    return {
        "proof_case_count": len(seen_case_ids),
        "proof_case_decisions": decisions,
        "proof_case_path": proof_case_path,
    }


def _validate_proof_model(
    proof_model: dict[str, Any],
    *,
    pass_by_id: dict[str, dict[str, Any]],
    preservation_contracts: dict[str, dict[str, Any]],
    failures: list[str],
) -> dict[str, Any]:
    if proof_model.get("contract_id") != PROOF_MODEL_CONTRACT_ID:
        failures.append("optimization proof model contract_id drifted")
    if proof_model.get("issue_ref") != "#8191":
        failures.append("optimization proof model must bind issue #8191")

    public_actions = {
        str(action) for action in _as_list(proof_model.get("required_public_actions"))
    }
    if not REQUIRED_PROOF_MODEL_PUBLIC_ACTIONS.issubset(public_actions):
        failures.append("optimization proof model public actions incomplete")

    proof_ids = {
        str(row.get("proof_id"))
        for row in _as_list(proof_model.get("proof_definitions"))
        if isinstance(row, dict) and row.get("proof_id")
    }
    if not REQUIRED_ALL_PROOF_IDS.issubset(proof_ids):
        failures.append("optimization proof model required proof ids incomplete")

    candidate_fields = {
        str(field) for field in _as_list(proof_model.get("candidate_input_fields"))
    }
    if not (
        REQUIRED_PROOF_CANDIDATE_INPUT_FIELDS
        | REQUIRED_DEVIRTUALIZATION_CANDIDATE_INPUT_FIELDS
        | REQUIRED_METHOD_INLINING_CANDIDATE_INPUT_FIELDS
    ).issubset(candidate_fields):
        failures.append("optimization proof model candidate input fields incomplete")

    result_fields = {
        str(field) for field in _as_list(proof_model.get("result_payload_fields"))
    }
    if not REQUIRED_PROOF_RESULT_FIELDS.issubset(result_fields):
        failures.append("optimization proof model result payload fields incomplete")

    verdict_fields = {
        str(field) for field in _as_list(proof_model.get("required_verdict_fields"))
    }
    if not (
        REQUIRED_PROOF_VERDICT_FIELDS | REQUIRED_DEVIRTUALIZATION_VERDICT_FIELDS
    ).issubset(verdict_fields):
        failures.append("optimization proof model verdict fields incomplete")

    verifier_passes = {
        str(row.get("verifier_id"))
        for row in _as_list(proof_model.get("verifier_passes"))
        if isinstance(row, dict) and row.get("verifier_id")
    }
    if not REQUIRED_VERIFIER_PASSES.issubset(verifier_passes):
        failures.append("optimization proof model verifier passes incomplete")

    proof_contracts_by_pass: dict[str, dict[str, Any]] = {}
    actual_order: list[str] = []
    for row in _as_list(proof_model.get("pass_proof_contracts")):
        if not isinstance(row, dict):
            failures.append("optimization proof pass contract row is not an object")
            continue
        pass_id = str(row.get("pass_id", ""))
        if not pass_id:
            failures.append("optimization proof pass contract missing pass_id")
            continue
        if pass_id in proof_contracts_by_pass:
            failures.append(f"duplicate optimization proof pass contract: {pass_id}")
            continue
        proof_contracts_by_pass[pass_id] = row
        actual_order.append(pass_id)

        pass_row = pass_by_id.get(pass_id)
        if pass_row is None:
            failures.append(f"optimization proof pass contract has no pass registry row: {pass_id}")
            continue
        if row.get("ordinal") != pass_row.get("ordinal"):
            failures.append(f"optimization proof pass ordinal drifted: {pass_id}")
        for field in REQUIRED_PROOF_VERDICT_FIELDS:
            if row.get(field) != "required":
                failures.append(
                    f"optimization proof pass contract does not require {field}: {pass_id}"
                )
        if pass_id == "devirtualization":
            for field in REQUIRED_DEVIRTUALIZATION_VERDICT_FIELDS:
                if row.get(field) != "required":
                    failures.append(
                        f"devirtualization proof pass contract does not require {field}: {pass_id}"
                    )
        if row.get("success_claim_on_skip") is not False:
            failures.append(f"optimization proof pass allows skip success claim: {pass_id}")
        if row.get("unsupported_skip_behavior") != "SKIP_FAIL_CLOSED_NO_SUCCESS_CLAIM":
            failures.append(f"optimization proof pass skip behavior drifted: {pass_id}")
        if not str(row.get("invalidation_contract", "")):
            failures.append(f"optimization proof pass missing invalidation contract: {pass_id}")
        if not str(row.get("fail_closed_diagnostic", "")):
            failures.append(f"optimization proof pass missing diagnostic: {pass_id}")
        elif row.get("fail_closed_diagnostic") not in _as_list(
            pass_row.get("fail_closed_diagnostics")
        ):
            failures.append(
                f"optimization proof pass diagnostic is not registered: {pass_id}"
            )

        required_proofs = [str(proof) for proof in _as_list(row.get("required_proof_ids"))]
        if not required_proofs:
            failures.append(f"optimization proof pass missing required proofs: {pass_id}")
        missing_required = sorted(_required_proof_ids_for_pass(pass_id).difference(required_proofs))
        if missing_required:
            failures.append(
                f"optimization proof pass required proofs incomplete for {pass_id}: "
                + ", ".join(missing_required)
            )
        unknown_proofs = sorted(set(required_proofs).difference(proof_ids))
        if unknown_proofs:
            failures.append(
                f"optimization proof pass references unknown proofs for {pass_id}: "
                + ", ".join(unknown_proofs)
            )

    missing_passes = [pass_id for pass_id in REQUIRED_PASS_ORDER if pass_id not in proof_contracts_by_pass]
    if missing_passes:
        failures.append(
            "optimization proof model pass contracts missing passes: "
            + ", ".join(missing_passes)
        )
    if actual_order != REQUIRED_PASS_ORDER:
        failures.append(
            "optimization proof model pass contract order drifted: "
            + ", ".join(actual_order)
        )

    proof_case_counts = _validate_proof_case_fixture(
        str(proof_model.get("proof_case_fixture_path", "")),
        proof_model=proof_model,
        pass_by_id=pass_by_id,
        preservation_contracts=preservation_contracts,
        proof_contracts_by_pass=proof_contracts_by_pass,
        failures=failures,
    )
    return {
        "proof_model_contract": proof_model.get("contract_id", ""),
        "proof_model_public_actions": sorted(public_actions),
        "proof_definition_count": len(proof_ids),
        "proof_pass_contract_count": len(proof_contracts_by_pass),
        "proof_candidate_field_count": len(candidate_fields),
        "proof_result_field_count": len(result_fields),
        "proof_verifier_pass_count": len(verifier_passes),
        **proof_case_counts,
    }


def validate_pipeline(
    pipeline_path: Path = PIPELINE_PATH,
) -> SemanticOptimizationPipelineValidationResult:
    pipeline = require_json_object(pipeline_path)
    failures: list[str] = []

    if pipeline.get("contract_id") != CONTRACT_ID:
        failures.append("semantic optimization pipeline contract_id drifted")

    issue_mapping = _as_dict(pipeline.get("issue_mapping"))
    issues = set(int(issue) for issue in _as_list(issue_mapping.get("primary_issues")))
    if not REQUIRED_ISSUES.issubset(issues):
        failures.append(
            "semantic optimization pipeline must map to issues #8175, #8191, #8192, #8193, #8194, #8205, #8224, #8226, and #8227"
        )
    support_claims = {str(claim) for claim in _as_list(issue_mapping.get("support_claims"))}
    if REQUIRED_SUPPORT_CLAIM not in support_claims:
        failures.append("semantic optimization pipeline support claim is missing")
    if REQUIRED_METHOD_INLINING_SUPPORT_CLAIM not in support_claims:
        failures.append("method inlining safe-subset support claim is missing")
    if REQUIRED_CACHE_AWARE_SUPPORT_CLAIM not in support_claims:
        failures.append("cache-aware dispatch support claim is missing")
    capability_rows = {
        str(row) for row in _as_list(issue_mapping.get("capability_rows_required"))
    }
    if not REQUIRED_CAPABILITY_ROWS.issubset(capability_rows):
        failures.append("semantic optimization pipeline required capability rows are incomplete")
    evidence_ids = {
        str(row) for row in _as_list(issue_mapping.get("evidence_ids_required"))
    }
    if not REQUIRED_EVIDENCE_IDS.issubset(evidence_ids):
        failures.append("semantic optimization pipeline required evidence ids are incomplete")

    source_truth = _as_dict(pipeline.get("source_truth"))
    if source_truth.get("workflow_action") != "validate-semantic-optimization-pipeline":
        failures.append("semantic optimization pipeline workflow action drifted")
    schema_path = str(source_truth.get("schema_path", ""))
    if schema_path != "schemas/objc3c-semantic-optimization-pipeline-v1.schema.json":
        failures.append("semantic optimization pipeline schema path drifted")
    if not (ROOT / schema_path).is_file():
        failures.append(f"semantic optimization pipeline schema missing: {schema_path}")
    for anchor in _as_list(source_truth.get("source_anchors")):
        if isinstance(anchor, dict):
            _require_path_with_tokens(
                str(anchor.get("path", "")),
                _as_list(anchor.get("tokens")),
                failures,
            )
        else:
            failures.append("semantic optimization source anchor is not an object")

    pass_by_id = _validate_pass_registry(_as_list(pipeline.get("pass_registry")), failures)
    explicit_pass_order = _validate_pass_order_contract(
        _as_dict(pipeline.get("pass_order_contract")),
        pass_by_id,
        failures,
    )
    preservation_contracts = _validate_semantic_preservation_contracts(
        _as_list(pipeline.get("semantic_preservation_contracts")),
        pass_by_id,
        failures,
    )
    proof_model_counts = _validate_proof_model(
        _as_dict(pipeline.get("proof_model")),
        pass_by_id=pass_by_id,
        preservation_contracts=preservation_contracts,
        failures=failures,
    )

    gates = _as_list(pipeline.get("verification_gates"))
    gate_ids = {
        str(gate.get("gate_id"))
        for gate in gates
        if isinstance(gate, dict) and gate.get("required") is True
    }
    missing_gates = sorted(REQUIRED_GATES.difference(gate_ids))
    if missing_gates:
        failures.append(f"semantic optimization verification gates missing: {', '.join(missing_gates)}")

    unsupported_policy = _as_dict(pipeline.get("unsupported_policy"))
    for field in FALSE_UNSUPPORTED_POLICY_FIELDS:
        if unsupported_policy.get(field) is not False:
            failures.append(f"unsupported policy must keep {field}=false")

    performance_governance = _as_dict(pipeline.get("performance_governance"))
    performance_governance_counts = _validate_performance_governance(
        performance_governance,
        failures,
    )
    runtime_equivalence = _as_dict(
        performance_governance.get("runtime_equivalence_validation")
    )
    _validate_direct_dispatch_fixture(failures)
    _validate_method_inlining_ir_fixture(failures)
    reserved_skip_fixture_count = _validate_reserved_skip_fixtures(
        pass_by_id,
        preservation_contracts,
        failures,
    )

    payload: dict[str, Any] = {
        "contract_id": "objc3c.optimization.semantic.pipeline.validation.v1",
        "status": "PASS" if not failures else "FAIL",
        "policy_path": _policy_path_label(pipeline_path),
        "schema_path": schema_path,
        "workflow_action": source_truth.get("workflow_action", ""),
        "issue_mapping": {
            "issues": sorted(issues),
            "support_claims": sorted(support_claims),
        },
        "pass_order": [pass_id for pass_id in REQUIRED_PASS_ORDER if pass_id in pass_by_id],
        "explicit_pass_order": explicit_pass_order,
        "pass_count": len(pass_by_id),
        "semantic_preservation_contract_count": len(preservation_contracts),
        "proof_model_contract": proof_model_counts["proof_model_contract"],
        "proof_definition_count": proof_model_counts["proof_definition_count"],
        "proof_pass_contract_count": proof_model_counts["proof_pass_contract_count"],
        "proof_candidate_field_count": proof_model_counts["proof_candidate_field_count"],
        "proof_result_field_count": proof_model_counts["proof_result_field_count"],
        "proof_verifier_pass_count": proof_model_counts["proof_verifier_pass_count"],
        "proof_case_count": proof_model_counts["proof_case_count"],
        "proof_case_path": proof_model_counts["proof_case_path"],
        "proof_case_decisions": proof_model_counts["proof_case_decisions"],
        "proof_model_public_actions": proof_model_counts["proof_model_public_actions"],
        "enabled_pass_count": sum(1 for row in pass_by_id.values() if row.get("mode") == "enabled"),
        "reserved_pass_count": sum(1 for row in pass_by_id.values() if row.get("mode") == "reserved"),
        "reserved_skip_fixture_count": reserved_skip_fixture_count,
        "reserved_skip_passes": sorted(
            pass_id for pass_id, row in pass_by_id.items() if row.get("mode") == "reserved"
        ),
        "verifier_only_pass_count": sum(
            1 for row in pass_by_id.values() if row.get("mode") == "verifier-only"
        ),
        "verification_gates": sorted(gate_ids),
        "capability_rows_required": sorted(capability_rows),
        "evidence_ids_required": sorted(evidence_ids),
        "source_anchor_count": len(_as_list(source_truth.get("source_anchors"))),
        "performance_governance_contract": performance_governance.get("contract_id", ""),
        "performance_public_actions": sorted(
            str(action)
            for action in _as_list(performance_governance.get("required_public_actions"))
        ),
        "runtime_equivalence_contract": runtime_equivalence.get("contract_id", ""),
        "runtime_equivalence_actions": sorted(
            str(action)
            for action in _as_list(runtime_equivalence.get("required_public_actions"))
        ),
        "performance_workload_count": performance_governance_counts[
            "performance_workload_count"
        ],
        "performance_trace_count": performance_governance_counts[
            "performance_trace_count"
        ],
        "performance_digest_count": performance_governance_counts[
            "performance_digest_count"
        ],
        "optimization_safety_contract": performance_governance_counts[
            "optimization_safety_contract"
        ],
        "optimization_safety_benchmark_contract_count": performance_governance_counts[
            "optimization_safety_benchmark_contract_count"
        ],
        "optimization_safety_boundary_count": performance_governance_counts[
            "optimization_safety_boundary_count"
        ],
        "optimization_safety_negative_case_count": performance_governance_counts[
            "optimization_safety_negative_case_count"
        ],
        "optimization_safety_evidence_path_count": performance_governance_counts[
            "optimization_safety_evidence_path_count"
        ],
        "optimization_safety_umbrella_status": performance_governance_counts[
            "optimization_safety_umbrella_status"
        ],
        "optimization_safety_umbrella_source_contract_count": performance_governance_counts[
            "optimization_safety_umbrella_source_contract_count"
        ],
        "optimization_safety_umbrella_negative_fixture_count": performance_governance_counts[
            "optimization_safety_umbrella_negative_fixture_count"
        ],
        "optimization_safety_umbrella_public_action_count": performance_governance_counts[
            "optimization_safety_umbrella_public_action_count"
        ],
        "optimization_safety_umbrella_capability_row_count": performance_governance_counts[
            "optimization_safety_umbrella_capability_row_count"
        ],
        "runtime_equivalence_case_count": performance_governance_counts[
            "runtime_equivalence_case_count"
        ],
        "runtime_equivalence_checked_path_count": performance_governance_counts[
            "runtime_equivalence_checked_path_count"
        ],
        "failures": failures,
    }
    return SemanticOptimizationPipelineValidationResult(payload=payload, failures=failures)


def validate_optimization_proof_model(
    pipeline_path: Path = PIPELINE_PATH,
) -> SemanticOptimizationPipelineValidationResult:
    return validate_pipeline(pipeline_path)


__all__ = [
    "CONTRACT_ID",
    "PIPELINE_PATH",
    "PROOF_CASES_CONTRACT_ID",
    "PROOF_MODEL_CONTRACT_ID",
    "PROOF_MODEL_REPORT_PATH",
    "PERFORMANCE_GOVERNANCE_CONTRACT_ID",
    "OPTIMIZATION_RUNTIME_DEBUG_SAFETY_CONTRACT_ID",
    "OPTIMIZATION_RUNTIME_DEBUG_SAFETY_CONTRACT_PATH",
    "RUNTIME_EQUIVALENCE_CONTRACT_ID",
    "REPORT_PATH",
    "RESERVED_SKIP_CONTRACT_ID",
    "REQUIRED_PERFORMANCE_PUBLIC_ACTIONS",
    "REQUIRED_RUNTIME_EQUIVALENCE_PUBLIC_ACTIONS",
    "REQUIRED_EVIDENCE_IDS",
    "REQUIRED_CAPABILITY_ROWS",
    "REQUIRED_PASS_ORDER",
    "REQUIRED_PROOF_CASE_IDS",
    "REQUIRED_DEVIRTUALIZATION_PROOF_IDS",
    "REQUIRED_DEVIRTUALIZATION_VERDICT_FIELDS",
    "REQUIRED_PROOF_IDS",
    "REQUIRED_PROOF_MODEL_PUBLIC_ACTIONS",
    "REQUIRED_PROOF_RESULT_FIELDS",
    "REQUIRED_PROOF_VERDICT_FIELDS",
    "REQUIRED_RESERVED_SKIP_DIAGNOSTIC_CODE",
    "SAFE_PROOF_VERDICTS",
    "SemanticOptimizationPipelineValidationResult",
    "evaluate_optimization_proof_case",
    "validate_optimization_proof_model",
    "validate_pipeline",
]
