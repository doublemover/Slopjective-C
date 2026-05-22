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
from scripts.objc3c_debug_maps.model import REQUIRED_SOURCE_MAP_RECORD_KINDS

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
ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT = (
    "tests/tooling/fixtures/advanced_runtime_closure/"
    "combined_runtime_identity_contract.json"
)
ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT_ID = (
    "objc3c.advanced-runtime.closure.combined-runtime-identity.v1"
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
REQUIRED_PROOF_AXES = {"runtime_state", "source_graph", "debug_map", "abi_surface"}
REQUIRED_INTERACTION_FEATURE_SETS = {
    "ownership-block": frozenset({"ownership", "blocks"}),
    "block-async-error": frozenset({"blocks", "async", "error"}),
    "async-actor-cancellation": frozenset({"async", "actor", "cancellation"}),
    "actor-property-macro": frozenset({"actor", "property", "macro"}),
    "macro-package-replay": frozenset({"macro", "package_replay"}),
    "error-package-replay": frozenset({"error", "package_replay"}),
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
    ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT,
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


def _validate_feature_list(
    raw_features: object,
    failures: list[str],
    *,
    label: str,
    minimum: int = 1,
) -> set[str]:
    features = {str(feature) for feature in _as_list(raw_features)}
    if len(features) < minimum:
        failures.append(f"{label}: expected at least {minimum} features")
    invalid = features - REQUIRED_CLOSURE_FEATURES
    if invalid:
        failures.append(f"{label}: unexpected features {sorted(invalid)}")
    return features


def _matching_required_interactions(features: set[str]) -> set[str]:
    return {
        interaction_id
        for interaction_id, required_features in REQUIRED_INTERACTION_FEATURE_SETS.items()
        if required_features <= features
    }


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
    seen_case_ids: set[str] = set()
    seen_interactions: set[str] = set()
    for index, case in enumerate(cases):
        label = f"advanced_runtime_closure.negative_matrix.cases[{index}]"
        case_id = str(case.get("case_id", ""))
        if not case_id:
            failures.append(f"{label}: case_id is required")
        elif case_id in seen_case_ids:
            failures.append(f"{label}: duplicate case_id {case_id}")
        seen_case_ids.add(case_id)
        feature = str(case.get("feature", ""))
        seen_features.add(feature)
        if feature not in REQUIRED_CLOSURE_FEATURES:
            failures.append(f"{label}: unexpected feature {feature}")
        interaction_features = {feature} | {
            str(item) for item in _as_list(case.get("interacts_with"))
        }
        invalid_interaction_features = interaction_features - REQUIRED_CLOSURE_FEATURES
        if invalid_interaction_features:
            failures.append(
                f"{label}: unexpected interaction features "
                f"{sorted(invalid_interaction_features)}"
            )
        if case.get("interacts_with") is not None and len(interaction_features) < 2:
            failures.append(f"{label}: interaction case must name at least two features")
        seen_interactions.update(_matching_required_interactions(interaction_features))
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
    missing_interactions = set(REQUIRED_INTERACTION_FEATURE_SETS) - seen_interactions
    if missing_interactions:
        failures.append(
            "advanced runtime negative matrix missing interaction coverage: "
            f"{sorted(missing_interactions)}"
        )
    return {
        "path": ADVANCED_CLOSURE_NEGATIVE_MATRIX,
        "case_count": len(cases),
        "features": sorted(seen_features),
        "interaction_count": len(seen_interactions),
        "interactions": sorted(seen_interactions),
    }


def _validate_combined_identity_contract(failures: list[str]) -> dict[str, Any]:
    contract_path = ROOT / ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT
    contract = _load_json(contract_path)
    label = "advanced_runtime_closure.combined_identity_contract"
    if contract.get("contract_id") != ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT_ID:
        failures.append(f"{label}: contract_id drifted")
    if int(contract.get("issue_ref", 0)) != 8199:
        failures.append(f"{label}: issue_ref must be #8199")
    if contract.get("scope") != "combined-runtime-state-source-graph-debug-map-proof":
        failures.append(f"{label}: scope drifted")
    if contract.get("positive_fixture") != ADVANCED_CLOSURE_POSITIVE_FIXTURE:
        failures.append(f"{label}: positive_fixture drifted")
    if contract.get("negative_matrix") != ADVANCED_CLOSURE_NEGATIVE_MATRIX:
        failures.append(f"{label}: negative_matrix drifted")
    if contract.get("umbrella_support_promoted") is not False:
        failures.append(f"{label}: umbrella support must remain unpromoted")
    if contract.get("source_truth") != "checked-in source files and checked-in contract fixtures only":
        failures.append(f"{label}: source_truth must stay checked-in")

    forbidden_prefixes = [
        str(prefix) for prefix in _as_list(contract.get("forbidden_source_truth_prefixes"))
    ]
    proof_axes = {str(axis) for axis in _as_list(contract.get("required_proof_axes"))}
    missing_axes = REQUIRED_PROOF_AXES - proof_axes
    if missing_axes:
        failures.append(f"{label}: missing proof axes {sorted(missing_axes)}")

    _paths_exist(
        [
            str(contract.get("language_semantics_contract", "")),
            str(contract.get("debug_source_map_validator", "")),
        ],
        failures,
        label,
    )

    runtime_records = [
        record
        for record in _as_list(contract.get("runtime_state_records"))
        if isinstance(record, dict)
    ]
    source_graph_records = [
        record
        for record in _as_list(contract.get("compiler_owned_source_graph_records"))
        if isinstance(record, dict)
    ]
    debug_map_records = [
        record
        for record in _as_list(contract.get("debug_map_records"))
        if isinstance(record, dict)
    ]
    abi_records = [
        record
        for record in _as_list(contract.get("abi_interaction_records"))
        if isinstance(record, dict)
    ]
    interaction_records = [
        record
        for record in _as_list(contract.get("interaction_records"))
        if isinstance(record, dict)
    ]

    runtime_ids: set[str] = set()
    for index, record in enumerate(runtime_records):
        record_label = f"{label}.runtime_state_records[{index}]"
        record_id = str(record.get("record_id", ""))
        if not record_id:
            failures.append(f"{record_label}: record_id is required")
        elif record_id in runtime_ids:
            failures.append(f"{record_label}: duplicate record_id {record_id}")
        runtime_ids.add(record_id)
        _validate_feature_list(record.get("features"), failures, label=record_label)
        _validate_required_tokens(
            _normalized(str(record.get("source_path", ""))),
            [str(token) for token in _as_list(record.get("required_tokens"))],
            failures,
            label=record_label,
            forbidden_prefixes=forbidden_prefixes,
        )

    source_graph_ids: set[str] = set()
    source_graph_by_id: dict[str, dict[str, Any]] = {}
    for index, record in enumerate(source_graph_records):
        record_label = f"{label}.compiler_owned_source_graph_records[{index}]"
        record_id = str(record.get("record_id", ""))
        if not record_id:
            failures.append(f"{record_label}: record_id is required")
        elif record_id in source_graph_ids:
            failures.append(f"{record_label}: duplicate record_id {record_id}")
        source_graph_ids.add(record_id)
        source_graph_by_id[record_id] = record
        _validate_feature_list(record.get("features"), failures, label=record_label)
        if str(record.get("source_graph_node_id", "")).startswith("advanced-runtime.") is False:
            failures.append(f"{record_label}: source_graph_node_id must be advanced-runtime scoped")
        record_kind = str(record.get("source_map_record_kind", ""))
        if record_kind not in REQUIRED_SOURCE_MAP_RECORD_KINDS:
            failures.append(f"{record_label}: invalid source_map_record_kind {record_kind}")
        _validate_required_tokens(
            _normalized(str(record.get("source_path", ""))),
            [str(token) for token in _as_list(record.get("required_tokens"))],
            failures,
            label=record_label,
            forbidden_prefixes=forbidden_prefixes,
        )

    debug_ids: set[str] = set()
    for index, record in enumerate(debug_map_records):
        record_label = f"{label}.debug_map_records[{index}]"
        record_id = str(record.get("record_id", ""))
        if not record_id:
            failures.append(f"{record_label}: record_id is required")
        elif record_id in debug_ids:
            failures.append(f"{record_label}: duplicate record_id {record_id}")
        debug_ids.add(record_id)
        source_graph_record_id = str(record.get("source_graph_record_id", ""))
        source_graph_record = source_graph_by_id.get(source_graph_record_id)
        if source_graph_record is None:
            failures.append(
                f"{record_label}: source_graph_record_id is missing: "
                f"{source_graph_record_id}"
            )
        elif record.get("source_graph_node_id") != source_graph_record.get("source_graph_node_id"):
            failures.append(f"{record_label}: source graph node drifted")
        if not _as_list(record.get("runtime_anchor_ids")):
            failures.append(f"{record_label}: runtime_anchor_ids are required")
        if not _as_list(record.get("language_anchor_ids")):
            failures.append(f"{record_label}: language_anchor_ids are required")

    abi_ids: set[str] = set()
    for index, record in enumerate(abi_records):
        record_label = f"{label}.abi_interaction_records[{index}]"
        record_id = str(record.get("record_id", ""))
        if not record_id:
            failures.append(f"{record_label}: record_id is required")
        elif record_id in abi_ids:
            failures.append(f"{record_label}: duplicate record_id {record_id}")
        abi_ids.add(record_id)
        _validate_feature_list(record.get("features"), failures, label=record_label)
        _validate_required_tokens(
            _normalized(str(record.get("source_path", ""))),
            [str(token) for token in _as_list(record.get("required_tokens"))],
            failures,
            label=record_label,
            forbidden_prefixes=forbidden_prefixes,
        )

    matrix = _load_json(ROOT / ADVANCED_CLOSURE_NEGATIVE_MATRIX)
    negative_case_ids = {
        str(case.get("case_id", ""))
        for case in _as_list(matrix.get("cases"))
        if isinstance(case, dict)
    }
    seen_interactions: set[str] = set()
    for index, record in enumerate(interaction_records):
        record_label = f"{label}.interaction_records[{index}]"
        interaction_id = str(record.get("interaction_id", ""))
        features = _validate_feature_list(
            record.get("features"),
            failures,
            label=record_label,
            minimum=2,
        )
        if interaction_id not in REQUIRED_INTERACTION_FEATURE_SETS:
            failures.append(f"{record_label}: unexpected interaction_id {interaction_id}")
        elif REQUIRED_INTERACTION_FEATURE_SETS[interaction_id] != frozenset(features):
            failures.append(f"{record_label}: interaction features drifted")
        seen_interactions.add(interaction_id)
        _validate_required_tokens(
            ADVANCED_CLOSURE_POSITIVE_FIXTURE,
            [str(token) for token in _as_list(record.get("positive_required_tokens"))],
            failures,
            label=record_label,
            forbidden_prefixes=forbidden_prefixes,
        )
        for case_id in _as_list(record.get("negative_case_ids")):
            if str(case_id) not in negative_case_ids:
                failures.append(f"{record_label}: missing negative case {case_id}")
        for source_graph_id in _as_list(record.get("source_graph_record_ids")):
            if str(source_graph_id) not in source_graph_ids:
                failures.append(f"{record_label}: missing source graph record {source_graph_id}")
        for debug_id in _as_list(record.get("debug_map_record_ids")):
            if str(debug_id) not in debug_ids:
                failures.append(f"{record_label}: missing debug map record {debug_id}")
        for runtime_id in _as_list(record.get("runtime_state_record_ids")):
            if str(runtime_id) not in runtime_ids:
                failures.append(f"{record_label}: missing runtime state record {runtime_id}")
        for abi_id in _as_list(record.get("abi_record_ids")):
            if str(abi_id) not in abi_ids:
                failures.append(f"{record_label}: missing ABI record {abi_id}")

    missing_interactions = set(REQUIRED_INTERACTION_FEATURE_SETS) - seen_interactions
    if missing_interactions:
        failures.append(f"{label}: missing interaction records {sorted(missing_interactions)}")

    return {
        "path": ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT,
        "runtime_state_record_count": len(runtime_records),
        "source_graph_record_count": len(source_graph_records),
        "debug_map_record_count": len(debug_map_records),
        "abi_interaction_record_count": len(abi_records),
        "interaction_count": len(interaction_records),
        "interactions": sorted(seen_interactions),
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
        "runtime_anchor": "build_advanced_runtime_capability_split_contract+combined_runtime_identity_contract",
        "combined_fixture": ADVANCED_CLOSURE_POSITIVE_FIXTURE,
        "combined_contract": ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT,
        "positive_fixture": ADVANCED_CLOSURE_POSITIVE_FIXTURE,
        "negative_fixture": ADVANCED_CLOSURE_NEGATIVE_MATRIX,
        "unsupported_combination_diagnostic": "advanced-runtime.unsupported-combination",
    }
    for key, value in expected.items():
        if row.get(key) != value:
            failures.append(f"#8199 language semantics row {key} drifted")
    expected_bool_fields = {
        "combined_runtime_evidence": True,
        "negative_combination_evidence": True,
        "source_identity_evidence": True,
        "umbrella_closure_support": False,
    }
    for key, expected_value in expected_bool_fields.items():
        if row.get(key) is not expected_value:
            failures.append(
                f"#8199 language semantics row {key} must be {expected_value}"
            )
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
    combined_identity = _validate_combined_identity_contract(failures)
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
        "advanced_runtime_negative_matrix_interaction_count": negative_matrix.get(
            "interaction_count"
        ),
        "advanced_runtime_combined_identity_contract": combined_identity.get("path"),
        "advanced_runtime_combined_identity_runtime_state_record_count": combined_identity.get(
            "runtime_state_record_count"
        ),
        "advanced_runtime_combined_identity_source_graph_record_count": combined_identity.get(
            "source_graph_record_count"
        ),
        "advanced_runtime_combined_identity_debug_map_record_count": combined_identity.get(
            "debug_map_record_count"
        ),
        "advanced_runtime_combined_identity_abi_interaction_record_count": combined_identity.get(
            "abi_interaction_record_count"
        ),
        "advanced_runtime_combined_identity_interaction_count": combined_identity.get(
            "interaction_count"
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
