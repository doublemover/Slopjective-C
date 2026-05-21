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
    "objc3c.evidence.semantic_optimization_pipeline.performance_governance",
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
}
REQUIRED_RUNTIME_EQUIVALENCE_CHECKED_IN_PATHS = {
    "tests/native/ir/optimization/semantic_pipeline_direct_dispatch.before.ll",
    "tests/native/ir/optimization/semantic_pipeline_direct_dispatch.after.ll",
    "tests/native/ir/runtime_calls/non_nil_receiver_runtime_call_contract.objc3",
    "tests/tooling/fixtures/stress/lowering_runtime_stress_manifest.json",
}
FORBIDDEN_PERFORMANCE_SOURCE_ROOTS = ("tmp/", "checked_outputs/")


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


def _validate_performance_governance(
    performance_governance: dict[str, Any],
    failures: list[str],
) -> dict[str, int]:
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

    performance_governance = _as_dict(pipeline.get("performance_governance"))
    performance_governance_counts = _validate_performance_governance(
        performance_governance,
        failures,
    )
    runtime_equivalence = _as_dict(
        performance_governance.get("runtime_equivalence_validation")
    )
    _validate_direct_dispatch_fixture(failures)
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
        "runtime_equivalence_case_count": performance_governance_counts[
            "runtime_equivalence_case_count"
        ],
        "runtime_equivalence_checked_path_count": performance_governance_counts[
            "runtime_equivalence_checked_path_count"
        ],
        "failures": failures,
    }
    return SemanticOptimizationPipelineValidationResult(payload=payload, failures=failures)


__all__ = [
    "CONTRACT_ID",
    "PIPELINE_PATH",
    "PERFORMANCE_GOVERNANCE_CONTRACT_ID",
    "RUNTIME_EQUIVALENCE_CONTRACT_ID",
    "REPORT_PATH",
    "RESERVED_SKIP_CONTRACT_ID",
    "REQUIRED_PERFORMANCE_PUBLIC_ACTIONS",
    "REQUIRED_RUNTIME_EQUIVALENCE_PUBLIC_ACTIONS",
    "REQUIRED_EVIDENCE_IDS",
    "REQUIRED_CAPABILITY_ROWS",
    "REQUIRED_PASS_ORDER",
    "REQUIRED_RESERVED_SKIP_DIAGNOSTIC_CODE",
    "SemanticOptimizationPipelineValidationResult",
    "validate_pipeline",
]
