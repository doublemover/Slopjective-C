#!/usr/bin/env python3
"""Validate the combined advanced runtime closure support surface."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
ROOT_TEXT = str(ROOT)
if ROOT_TEXT not in sys.path:
    sys.path.insert(0, ROOT_TEXT)

from check_ownership_concurrency_macro_completion_contract import (
    validate_ownership_concurrency_macro_completion_contract,
)
from scripts.objc3c_runtime_acceptance.domains.advanced_runtime_capability_split import (
    ADVANCED_RUNTIME_FEATURE_TAXONOMY,
    ADVANCED_RUNTIME_IMPLEMENTED_SUPPORT_CONTRACTS,
    build_advanced_runtime_capability_split_contract,
)

LANGUAGE_SEMANTICS_CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "objc3c"
    / "language_semantics_runtime_api_contract.json"
)
REPORT_PATH = ROOT / "tmp" / "reports" / "advanced-runtime-closure.json"
CONTRACT_ID = "objc3c.advanced-runtime.closure.validation.v1"
REQUIRED_FAMILIES = {
    "arc",
    "blocks",
    "concurrency",
    "errors",
    "interop",
    "metaprogramming",
    "property",
}
REQUIRED_PUBLIC_COMMANDS = {
    "npm run objc3c -- test-runtime-acceptance-block-arc",
    "npm run objc3c -- test-runtime-acceptance-arc-cleanup-integration",
    "npm run objc3c -- validate-error-conformance",
    "npm run objc3c -- validate-concurrency-conformance",
    "npm run objc3c -- validate-metaprogramming-conformance",
    "npm run objc3c -- validate-interop-conformance",
}
ADVANCED_CLOSURE_FIXTURE_DIR = "tests/native/runtime/advanced_closure"
ADVANCED_CLOSURE_POSITIVE_FIXTURE = (
    f"{ADVANCED_CLOSURE_FIXTURE_DIR}/combined_positive.objc3"
)
ADVANCED_CLOSURE_NEGATIVE_MATRIX = (
    f"{ADVANCED_CLOSURE_FIXTURE_DIR}/negative_matrix.contract.json"
)
ADVANCED_CLOSURE_NEGATIVE_MATRIX_CONTRACT_ID = (
    "objc3c.advanced-runtime.closure.negative-matrix.v1"
)
REQUIRED_CLOSURE_FEATURES = {
    "ownership",
    "blocks",
    "async",
    "actor",
    "cancellation",
    "error",
    "property",
    "macro",
    "package_replay",
}
REQUIRED_POSITIVE_FEATURE_TOKENS = {
    "ownership": (
        "borrowed id *",
        "objc_family_retain",
        "cf_returns_retained",
        "objc_family_release",
        "cf_consumed",
    ),
    "blocks": (
        "^[weak owner, unowned peer, retained]",
        "return closure()",
    ),
    "async": (
        "async fn asyncCancellationLane",
        "return await statusBridgeAdvancedRuntime",
    ),
    "actor": (
        "actor class AdvancedRuntimeClosureActor",
        "objc3_runtime_actor_mailbox_enqueue_i32",
        "objc3_runtime_actor_mailbox_drain_next_i32",
    ),
    "cancellation": (
        "objc3_runtime_task_is_cancelled_i32",
        "objc3_runtime_task_on_cancel_i32",
        "objc3_runtime_cancel_task_group_i32",
    ),
    "error": (
        "throws -> i32",
        "try? statusBridgeAdvancedRuntime",
        "try! throwingAdvancedRuntime",
        "catch (NSError* error)",
        "objc_status_code",
    ),
    "property": (
        "@property (strong, behavior=Observed) id value",
        "@property (readonly, strong, behavior=Projected, getter=currentValue) id currentValue",
    ),
    "macro": (
        "objc_macro_package(named(\"std.metaprogramming.advanced-runtime\"))",
        "objc_macro_provenance(named(\"sha256:advanced-runtime-closure\"))",
        "objc_macro_sandbox(named(\"deterministic\"))",
    ),
    "package_replay": (
        "objc_import_module(named(\"AdvancedRuntimeClosureKit\"))",
        "objc_mixed_image(named(\"AdvancedRuntimeClosureKitCore\"))",
        "objc_package_entry(named(\"AdvancedRuntimeClosureKit.replay\"))",
    ),
}
ALLOWED_NEGATIVE_STATUSES = {"rejected", "reserved"}
DOCS_SUPPORT_CAPABILITY_MATRIX = ROOT / "docs" / "support" / "capability_matrix.json"
DOCS_SUPPORT_EVIDENCE_MAP = ROOT / "docs" / "support" / "evidence_map.json"
REQUIRED_DOCS_SUPPORT_EVIDENCE = {
    "scripts/check_objc3c_advanced_runtime_closure.py",
    ADVANCED_CLOSURE_POSITIVE_FIXTURE,
    ADVANCED_CLOSURE_NEGATIVE_MATRIX,
}


def _repo_rel(path: Path) -> str:
    return path.resolve().relative_to(ROOT.resolve()).as_posix()


def _load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise ValueError(f"{_repo_rel(path)} must contain a JSON object")
    return payload


def _as_list(value: object) -> list[object]:
    return value if isinstance(value, list) else []


def _normalized(path: str) -> str:
    return path.replace("\\", "/")


def _is_forbidden_path(path: str, forbidden_prefixes: list[str] | None = None) -> bool:
    prefixes = forbidden_prefixes or ["tmp/", "temp/", "generated/", "build/", "dist/"]
    normalized = _normalized(path)
    return any(normalized.startswith(prefix) for prefix in prefixes)


def _source_text(
    relative_path: str,
    failures: list[str],
    *,
    label: str,
    forbidden_prefixes: list[str] | None = None,
) -> str:
    if not relative_path:
        failures.append(f"{label}: missing source path")
        return ""
    if _is_forbidden_path(relative_path, forbidden_prefixes):
        failures.append(f"{label}: generated path is not source truth: {relative_path}")
        return ""
    path = ROOT / relative_path
    if not path.is_file():
        failures.append(f"{label}: missing source-truth file: {relative_path}")
        return ""
    return path.read_text(encoding="utf-8")


def _paths_exist(paths: list[str], failures: list[str], label: str) -> None:
    for path in paths:
        if _is_forbidden_path(path):
            failures.append(f"{label}: generated path is not source truth: {path}")
            continue
        if not (ROOT / path).exists():
            failures.append(f"{label}: missing source-truth path: {path}")


def _validate_required_tokens(
    relative_path: str,
    tokens: list[str],
    failures: list[str],
    *,
    label: str,
    forbidden_prefixes: list[str] | None = None,
) -> None:
    text = _source_text(
        relative_path,
        failures,
        label=label,
        forbidden_prefixes=forbidden_prefixes,
    )
    if not tokens:
        failures.append(f"{label}: no required feature tokens declared")
    for token in tokens:
        if token not in text:
            failures.append(f"{label}: missing feature token in {relative_path}: {token}")


def _validate_combined_positive_fixture(failures: list[str]) -> dict[str, Any]:
    _source_text(
        ADVANCED_CLOSURE_POSITIVE_FIXTURE,
        failures,
        label="advanced_runtime_closure.positive_fixture",
    )
    for feature in sorted(REQUIRED_CLOSURE_FEATURES):
        _validate_required_tokens(
            ADVANCED_CLOSURE_POSITIVE_FIXTURE,
            list(REQUIRED_POSITIVE_FEATURE_TOKENS[feature]),
            failures,
            label=f"advanced_runtime_closure.positive_fixture.{feature}",
        )
    return {
        "path": ADVANCED_CLOSURE_POSITIVE_FIXTURE,
        "features": sorted(REQUIRED_CLOSURE_FEATURES),
    }


def _validate_negative_matrix(failures: list[str]) -> dict[str, Any]:
    matrix_path = ROOT / ADVANCED_CLOSURE_NEGATIVE_MATRIX
    matrix = _load_json(matrix_path)
    if matrix.get("contract_id") != ADVANCED_CLOSURE_NEGATIVE_MATRIX_CONTRACT_ID:
        failures.append("advanced runtime negative matrix contract_id drifted")
    if int(matrix.get("issue_ref", 0)) != 8199:
        failures.append("advanced runtime negative matrix must be issue-scoped to #8199")
    if matrix.get("scope") != "issue-scoped-negative-matrix":
        failures.append("advanced runtime negative matrix scope drifted")
    if matrix.get("positive_fixture") != ADVANCED_CLOSURE_POSITIVE_FIXTURE:
        failures.append("advanced runtime negative matrix must bind the combined positive fixture")

    forbidden_prefixes = [
        str(prefix) for prefix in _as_list(matrix.get("forbidden_source_truth_prefixes"))
    ]
    cases = [case for case in _as_list(matrix.get("cases")) if isinstance(case, dict)]
    seen_features: set[str] = set()
    for index, case in enumerate(cases):
        label = f"advanced_runtime_closure.negative_matrix.cases[{index}]"
        feature = str(case.get("feature", ""))
        seen_features.add(feature)
        if feature not in REQUIRED_CLOSURE_FEATURES:
            failures.append(f"{label}: unexpected feature {feature}")
        if int(case.get("issue_ref", 0)) != 8199:
            failures.append(f"{label}: case must be issue-scoped to #8199")
        if str(case.get("status", "")) not in ALLOWED_NEGATIVE_STATUSES:
            failures.append(f"{label}: status must remain rejected or reserved")
        diagnostic = str(case.get("expected_diagnostic", ""))
        if not diagnostic.startswith("advanced-runtime."):
            failures.append(f"{label}: expected_diagnostic must be advanced-runtime scoped")
        fixture = _normalized(str(case.get("fixture", "")))
        if not fixture.startswith(f"{ADVANCED_CLOSURE_FIXTURE_DIR}/"):
            failures.append(f"{label}: fixture must stay in {ADVANCED_CLOSURE_FIXTURE_DIR}")
        tokens = [str(token) for token in _as_list(case.get("required_tokens"))]
        _validate_required_tokens(
            fixture,
            tokens,
            failures,
            label=label,
            forbidden_prefixes=forbidden_prefixes,
        )

    missing_features = REQUIRED_CLOSURE_FEATURES - seen_features
    extra_features = seen_features - REQUIRED_CLOSURE_FEATURES
    if missing_features:
        failures.append(
            "advanced runtime negative matrix missing features: "
            f"{sorted(missing_features)}"
        )
    if extra_features:
        failures.append(
            "advanced runtime negative matrix has extra features: "
            f"{sorted(extra_features)}"
        )
    return {
        "path": ADVANCED_CLOSURE_NEGATIVE_MATRIX,
        "case_count": len(cases),
        "features": sorted(seen_features),
    }


def _validate_docs_support_rows(failures: list[str]) -> None:
    capability_matrix = _load_json(DOCS_SUPPORT_CAPABILITY_MATRIX)
    matrix_rows = [
        row
        for row in _as_list(capability_matrix.get("capabilities"))
        if isinstance(row, dict) and row.get("id") == "language.advanced-runtime-closure"
    ]
    if len(matrix_rows) != 1:
        failures.append("docs/support/capability_matrix.json must have exactly one advanced-runtime-closure row")
    else:
        row = matrix_rows[0]
        if row.get("state") != "reserved":
            failures.append("language.advanced-runtime-closure must remain reserved in capability matrix")
        evidence_paths = {
            str(item.get("path"))
            for item in _as_list(row.get("evidence"))
            if isinstance(item, dict)
        }
        missing = REQUIRED_DOCS_SUPPORT_EVIDENCE - evidence_paths
        if missing:
            failures.append(
                "language.advanced-runtime-closure capability matrix evidence missing: "
                f"{sorted(missing)}"
            )

    evidence_map = _load_json(DOCS_SUPPORT_EVIDENCE_MAP)
    evidence_rows = [
        row
        for row in _as_list(evidence_map.get("rows"))
        if isinstance(row, dict)
        and row.get("capability_id") == "language.advanced-runtime-closure"
    ]
    evidence_paths = {str(row.get("path")) for row in evidence_rows}
    missing = REQUIRED_DOCS_SUPPORT_EVIDENCE - evidence_paths
    if missing:
        failures.append(
            "language.advanced-runtime-closure evidence-map rows missing: "
            f"{sorted(missing)}"
        )
    claiming_rows = [
        str(row.get("path"))
        for row in evidence_rows
        if row.get("support_claim") not in (None, "")
    ]
    if claiming_rows:
        failures.append(
            "language.advanced-runtime-closure evidence-map rows must remain non-claiming: "
            f"{claiming_rows}"
        )


def _validate_advanced_runtime_split(failures: list[str]) -> dict[str, Any]:
    contract = build_advanced_runtime_capability_split_contract()
    implemented_contracts = list(ADVANCED_RUNTIME_IMPLEMENTED_SUPPORT_CONTRACTS)
    taxonomy = list(ADVANCED_RUNTIME_FEATURE_TAXONOMY)

    families = {str(item.get("family")) for item in taxonomy}
    if families != REQUIRED_FAMILIES:
        failures.append(f"feature taxonomy families drifted: {sorted(families)}")

    public_commands = {
        str(item.get("public_command")) for item in implemented_contracts
    }
    if not REQUIRED_PUBLIC_COMMANDS <= public_commands:
        failures.append(
            "advanced runtime closure is missing public command coverage: "
            f"{sorted(REQUIRED_PUBLIC_COMMANDS - public_commands)}"
        )

    for item in implemented_contracts:
        capability_id = str(item.get("capability_id", ""))
        _paths_exist(
            [str(path) for path in item.get("source_truth", ())],
            failures,
            f"{capability_id}.source_truth",
        )
        _paths_exist(
            [str(path) for path in item.get("positive_evidence", ())],
            failures,
            f"{capability_id}.positive_evidence",
        )
        _paths_exist(
            [str(path) for path in item.get("negative_evidence", ())],
            failures,
            f"{capability_id}.negative_evidence",
        )

    return contract


def _validate_language_semantics_row(failures: list[str]) -> dict[str, Any]:
    contract = _load_json(LANGUAGE_SEMANTICS_CONTRACT_PATH)
    rows = [
        row
        for row in contract.get("surface_rows", [])
        if isinstance(row, dict) and row.get("issue") == 8199
    ]
    if len(rows) != 1:
        failures.append("language semantics runtime API must publish exactly one #8199 row")
        return {}
    row = rows[0]
    expected = {
        "support_claim": "objc3c.behavior.language.advanced-runtime-closure",
        "semantic_surface": "advanced-runtime-combined-closure",
        "metadata_key": "advanced-runtime-combined-source-identity",
        "unsupported_combination_diagnostic": "advanced-runtime.unsupported-combination",
    }
    for key, value in expected.items():
        if row.get(key) != value:
            failures.append(f"#8199 language semantics row {key} drifted")
    for key in (
        "combined_runtime_evidence",
        "negative_combination_evidence",
        "source_identity_evidence",
        "umbrella_closure_support",
    ):
        if row.get(key) is not True:
            failures.append(f"#8199 language semantics row {key} must be true")
    _paths_exist(
        [
            str(row.get("combined_fixture", "")),
            str(row.get("combined_contract", "")),
            str(row.get("positive_fixture", "")),
            str(row.get("negative_fixture", "")),
        ],
        failures,
        "language_semantics_runtime_api_contract.surface_rows[8199]",
    )
    return row


def validate_advanced_runtime_closure() -> dict[str, Any]:
    failures: list[str] = []
    completion = validate_ownership_concurrency_macro_completion_contract()
    if not completion.passed:
        failures.extend(
            f"ownership-concurrency-macro completion: {failure}"
            for failure in completion.failures
        )
    split_contract = _validate_advanced_runtime_split(failures)
    language_row = _validate_language_semantics_row(failures)
    positive_fixture = _validate_combined_positive_fixture(failures)
    negative_matrix = _validate_negative_matrix(failures)
    _validate_docs_support_rows(failures)
    return {
        "contract_id": CONTRACT_ID,
        "status": "PASS" if not failures else "FAIL",
        "completion_contract_status": completion.payload.get("status"),
        "advanced_runtime_capability_count": len(
            split_contract.get("implemented_rows", [])
        ),
        "advanced_runtime_support_contract_count": len(
            split_contract.get("implemented_support_contracts", [])
        ),
        "advanced_runtime_combined_positive_fixture": positive_fixture.get("path"),
        "advanced_runtime_combined_feature_count": len(
            positive_fixture.get("features", [])
        ),
        "advanced_runtime_negative_matrix": negative_matrix.get("path"),
        "advanced_runtime_negative_matrix_case_count": negative_matrix.get(
            "case_count"
        ),
        "language_semantics_issue": language_row.get("issue"),
        "language_semantics_support_claim": language_row.get("support_claim"),
        "failures": failures,
    }


def main() -> int:
    payload = validate_advanced_runtime_closure()
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(
        json.dumps(payload, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2, sort_keys=True))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
