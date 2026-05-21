"""Validator model for the Objective-C 3.0 semantic optimization pipeline."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import sys
from typing import Any

SCRIPTS_ROOT = Path(__file__).resolve().parent
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_tooling.json_io import require_json_object
from objc3c_tooling.paths import ROOT, repo_rel


CONTRACT_ID = "objc3c.optimization.semantic.pipeline.v1"
PIPELINE_PATH = (
    ROOT / "tests" / "tooling" / "fixtures" / "semantic_optimization_pipeline" / "pipeline.json"
)
REPORT_PATH = ROOT / "tmp" / "reports" / "semantic-optimization-pipeline.json"
REQUIRED_ISSUES = {8175}
REQUIRED_SUPPORT_CLAIM = "objc3c.behavior.semantic_optimization_pipeline"
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
    "objc3c.internal.semantic_optimization_pass_registry",
    "objc3c.reserved.semantic_optimization.devirtualization",
    "objc3c.reserved.semantic_optimization.method_inlining",
    "objc3c.reserved.semantic_optimization.cache_aware_dispatch",
}
REQUIRED_EVIDENCE_IDS = {
    "objc3c.evidence.semantic_optimization_pipeline.fixture",
    "objc3c.evidence.semantic_optimization_pipeline.validator",
    "objc3c.evidence.semantic_optimization_pipeline.native_surface",
    "objc3c.evidence.semantic_optimization_pipeline.direct_dispatch_ir",
    "objc3c.evidence.semantic_optimization_pipeline.reserved_negative",
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


def _validate_reserved_skip_fixture(failures: list[str]) -> None:
    path = ROOT / "tests/tooling/fixtures/semantic_optimization_pipeline/reserved_devirtualization_skip.json"
    if not path.is_file():
        failures.append("reserved devirtualization skip fixture missing")
        return
    payload = require_json_object(path)
    if payload.get("status") != "SKIPPED_FAIL_CLOSED":
        failures.append("reserved devirtualization skip fixture must be SKIPPED_FAIL_CLOSED")
    if payload.get("success_claim") is not False:
        failures.append("reserved devirtualization skip fixture must not emit success claim")
    if not _as_list(payload.get("required_missing_proofs")):
        failures.append("reserved devirtualization skip fixture must list missing proofs")


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
        failures.append("semantic optimization pipeline must map to issue #8175")
    support_claims = {str(claim) for claim in _as_list(issue_mapping.get("support_claims"))}
    if REQUIRED_SUPPORT_CLAIM not in support_claims:
        failures.append("semantic optimization pipeline support claim is missing")
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

    _validate_direct_dispatch_fixture(failures)
    _validate_reserved_skip_fixture(failures)

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
        "enabled_pass_count": sum(1 for row in pass_by_id.values() if row.get("mode") == "enabled"),
        "reserved_pass_count": sum(1 for row in pass_by_id.values() if row.get("mode") == "reserved"),
        "verifier_only_pass_count": sum(
            1 for row in pass_by_id.values() if row.get("mode") == "verifier-only"
        ),
        "verification_gates": sorted(gate_ids),
        "capability_rows_required": sorted(capability_rows),
        "evidence_ids_required": sorted(evidence_ids),
        "source_anchor_count": len(_as_list(source_truth.get("source_anchors"))),
        "failures": failures,
    }
    return SemanticOptimizationPipelineValidationResult(payload=payload, failures=failures)


__all__ = [
    "CONTRACT_ID",
    "PIPELINE_PATH",
    "REPORT_PATH",
    "REQUIRED_EVIDENCE_IDS",
    "REQUIRED_CAPABILITY_ROWS",
    "REQUIRED_PASS_ORDER",
    "SemanticOptimizationPipelineValidationResult",
    "validate_pipeline",
]
