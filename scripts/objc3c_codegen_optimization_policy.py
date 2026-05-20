"""Validator model for codegen optimization and direct-dispatch policy."""

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


CONTRACT_ID = "objc3c.codegen.optimization.direct.dispatch.policy.v1"
POLICY_PATH = ROOT / "tests" / "tooling" / "fixtures" / "codegen_optimization_direct_dispatch" / "policy.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "codegen-optimization" / "direct-dispatch-policy.json"
REQUIRED_ISSUES = {8091, 8092}
REQUIRED_DRAFT_IDS = {"OC3-N33-001", "OC3-N33-002"}
REQUIRED_MATRIX_ROWS = {
    "nil-receiver-optional-send",
    "self-direct-method",
    "known-class-direct-method",
    "super-send",
    "category-dispatch-intent",
    "protocol-dispatch-intent",
    "dynamic-opt-out",
    "unsupported-dispatch-surface",
}
REQUIRED_COVERAGE_TERMS = {
    "nil_receiver": ("nil",),
    "super": ("super",),
    "category": ("category",),
    "protocol": ("protocol",),
    "marshalling": ("marshall", "coerc"),
    "cache_invalidation": ("cache", "proof state"),
    "strict_rejection": ("fail", "reject", "fallback"),
}
FORBIDDEN_POLICY_TERMS = ("retired adapter route", "old-mode route", "fail open")


@dataclass(frozen=True)
class PolicyValidationResult:
    payload: dict[str, Any]
    failures: list[str]

    @property
    def passed(self) -> bool:
        return not self.failures


def _as_list(value: object) -> list[object]:
    return value if isinstance(value, list) else []


def _field_text(row: dict[str, Any]) -> str:
    return " ".join(str(value) for value in row.values()).lower()


def _require_path_with_tokens(path_text: str, tokens: list[object], failures: list[str]) -> None:
    path = ROOT / path_text
    if not path.is_file():
        failures.append(f"source anchor missing: {path_text}")
        return
    text = path.read_text(encoding="utf-8")
    for token in tokens:
        if not isinstance(token, str) or token not in text:
            failures.append(f"source anchor token missing from {path_text}: {token}")


def validate_policy(policy_path: Path = POLICY_PATH) -> PolicyValidationResult:
    policy = require_json_object(policy_path)
    failures: list[str] = []

    if policy.get("contract_id") != CONTRACT_ID:
        failures.append("policy contract_id drifted")

    issue_mapping = policy.get("issue_mapping", {})
    issues = set(int(issue) for issue in _as_list(issue_mapping.get("primary_issues")))
    drafts = set(str(draft) for draft in _as_list(issue_mapping.get("draft_ids")))
    if not REQUIRED_ISSUES.issubset(issues):
        failures.append("policy must map to issues #8091 and #8092")
    if not REQUIRED_DRAFT_IDS.issubset(drafts):
        failures.append("policy must map to OC3-N33-001 and OC3-N33-002")

    source_truth = policy.get("source_truth", {})
    if source_truth.get("workflow_action") != "validate-codegen-optimization-policy":
        failures.append("policy workflow action is not the public validation action")
    for anchor in _as_list(source_truth.get("source_anchors")):
        if isinstance(anchor, dict):
            _require_path_with_tokens(
                str(anchor.get("path", "")),
                _as_list(anchor.get("tokens")),
                failures,
            )

    passes = _as_list(policy.get("optimizer_policy", {}).get("passes"))
    if len(passes) < 3:
        failures.append("optimizer policy must cover at least three semantic-preserving pass classes")
    for pass_row in passes:
        if not isinstance(pass_row, dict):
            failures.append("optimizer policy pass row is not an object")
            continue
        if pass_row.get("semantic_preserving") is not True:
            failures.append(f"optimizer pass is not semantic preserving: {pass_row.get('pass_id')}")
        for required_field in ("allowed_moves", "required_proofs", "rejection_boundaries"):
            if not _as_list(pass_row.get(required_field)):
                failures.append(f"optimizer pass missing {required_field}: {pass_row.get('pass_id')}")

    matrix_rows = _as_list(policy.get("direct_dispatch_matrix"))
    matrix_by_id = {
        str(row.get("row_id")): row
        for row in matrix_rows
        if isinstance(row, dict) and row.get("row_id")
    }
    missing_rows = sorted(REQUIRED_MATRIX_ROWS.difference(matrix_by_id))
    if missing_rows:
        failures.append(f"direct dispatch matrix missing rows: {', '.join(missing_rows)}")

    matrix_text = "\n".join(_field_text(row) for row in matrix_by_id.values())
    for coverage_id, terms in REQUIRED_COVERAGE_TERMS.items():
        if not any(term in matrix_text for term in terms):
            failures.append(f"direct dispatch matrix missing coverage term: {coverage_id}")

    for row_id, row in matrix_by_id.items():
        route = str(row.get("dispatch_route", ""))
        admitted = row.get("admitted")
        if route in {"strict-rejection", "fail-closed-before-ir-call"} and admitted is not False:
            failures.append(f"rejecting matrix row must be admitted=false: {row_id}")
        if route == "exact-direct-call" and "invalidate" not in str(row.get("cache_invalidation", "")).lower():
            failures.append(f"direct-call row must invalidate proof state: {row_id}")
        fixtures = [str(fixture) for fixture in _as_list(row.get("fixtures"))]
        if not fixtures:
            failures.append(f"matrix row must cite durable fixtures: {row_id}")
        for fixture in fixtures:
            if not (ROOT / fixture).is_file():
                failures.append(f"matrix fixture missing for {row_id}: {fixture}")

    rejection_boundary = policy.get("strict_rejection_boundaries", {})
    if rejection_boundary.get("retired_adapter_routes_allowed") is not False:
        failures.append("retired adapter routes must remain disallowed")
    if rejection_boundary.get("fail_open_allowed") is not False:
        failures.append("fail-open behavior must remain disallowed")
    if _as_list(rejection_boundary.get("retired_routes")):
        failures.append("retired routes must remain empty")

    policy_text = str(policy).lower()
    for forbidden in FORBIDDEN_POLICY_TERMS:
        if forbidden in policy_text and forbidden not in str(rejection_boundary.get("rejection_rules", [])).lower():
            failures.append(f"forbidden policy term appears outside rejection rules: {forbidden}")

    payload: dict[str, Any] = {
        "contract_id": "objc3c.codegen.optimization.direct.dispatch.policy.validation.v1",
        "status": "PASS" if not failures else "FAIL",
        "policy_path": repo_rel(policy_path),
        "schema_path": source_truth.get("schema_path", ""),
        "workflow_action": source_truth.get("workflow_action", ""),
        "issue_mapping": {
            "issues": sorted(issues),
            "draft_ids": sorted(drafts),
        },
        "optimizer_pass_count": len(passes),
        "direct_dispatch_matrix_rows": sorted(matrix_by_id),
        "coverage_terms": sorted(REQUIRED_COVERAGE_TERMS),
        "source_anchor_count": len(_as_list(source_truth.get("source_anchors"))),
        "failures": failures,
    }
    return PolicyValidationResult(payload=payload, failures=failures)


__all__ = [
    "CONTRACT_ID",
    "POLICY_PATH",
    "REPORT_PATH",
    "REQUIRED_MATRIX_ROWS",
    "PolicyValidationResult",
    "validate_policy",
]
