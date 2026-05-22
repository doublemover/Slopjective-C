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
from scripts.objc3c_debug_maps.model import (
    REQUIRED_SOURCE_MAP_RECORD_KINDS,
    load_bundle,
    validate_bundle_path,
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
ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT = (
    "tests/tooling/fixtures/advanced_runtime_closure/"
    "combined_runtime_identity_contract.json"
)
ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT_ID = (
    "objc3c.advanced-runtime.closure.combined-runtime-identity.v1"
)
ADVANCED_CLOSURE_CANONICAL_SOURCE_DEBUG_MAP_BUNDLE = (
    "tests/tooling/fixtures/advanced_runtime_closure/"
    "combined_runtime_source_debug_map.json"
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
REQUIRED_RUNTIME_SOURCE_DEBUG_LINKS = {
    "runtime-source-debug.ownership-block": frozenset({"ownership", "blocks"}),
    "runtime-source-debug.async-cancellation": frozenset({"async", "cancellation"}),
    "runtime-source-debug.actor-mailbox": frozenset({"actor", "async"}),
    "runtime-source-debug.error-bridge": frozenset({"error", "async"}),
    "runtime-source-debug.property-behavior": frozenset({"property"}),
    "runtime-source-debug.macro-provenance": frozenset({"macro"}),
    "runtime-source-debug.package-replay": frozenset({"package_replay"}),
}
REQUIRED_RUNTIME_SOURCE_DEBUG_EXEMPTIONS = {
    "runtime-state.combined-language-semantics"
}
REQUIRED_UNSUPPORTED_RESERVED_CLAIMS = {
    "swift-abi",
    "distributed-actors",
    "broad-scheduler-guarantees",
    "arbitrary-macro-host-execution",
}
REQUIRED_INTERACTION_FEATURE_SETS = {
    "ownership-block": frozenset({"ownership", "blocks"}),
    "block-async-error": frozenset({"blocks", "async", "error"}),
    "async-actor-cancellation": frozenset({"async", "actor", "cancellation"}),
    "actor-property-macro": frozenset({"actor", "property", "macro"}),
    "macro-package-replay": frozenset({"macro", "package_replay"}),
    "error-package-replay": frozenset({"error", "package_replay"}),
}
REQUIRED_LITERAL_NEGATIVE_CASE_IDS = {
    "actor_mailbox_unsupported_payload",
    "property_behavior_conflict",
    "scheduler_guarantee_overclaim",
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


def _index_json_records(
    records: list[dict[str, Any]],
    *,
    key: str,
    failures: list[str],
    label: str,
) -> dict[str, dict[str, Any]]:
    indexed: dict[str, dict[str, Any]] = {}
    for index, record in enumerate(records):
        record_label = f"{label}[{index}]"
        record_id = str(record.get(key, ""))
        if not record_id:
            failures.append(f"{record_label}: {key} is required")
            continue
        if record_id in indexed:
            failures.append(f"{record_label}: duplicate {key} {record_id}")
            continue
        indexed[record_id] = record
    return indexed


def _validate_unsupported_combination_policy(
    contract: dict[str, Any],
    failures: list[str],
    *,
    label: str,
) -> None:
    policy = contract.get("unsupported_combination_policy")
    if not isinstance(policy, dict):
        failures.append(f"{label}: unsupported_combination_policy is required")
        return
    if policy.get("diagnostic") != "advanced-runtime.unsupported-combination":
        failures.append(f"{label}: unsupported combination diagnostic drifted")
    statuses = {str(status) for status in _as_list(policy.get("unsupported_statuses"))}
    if statuses != ALLOWED_NEGATIVE_STATUSES:
        failures.append(
            f"{label}: unsupported statuses must stay {sorted(ALLOWED_NEGATIVE_STATUSES)}"
        )
    reserved_claims = {
        str(claim) for claim in _as_list(policy.get("reserved_claims"))
    }
    missing_claims = REQUIRED_UNSUPPORTED_RESERVED_CLAIMS - reserved_claims
    extra_claims = reserved_claims - REQUIRED_UNSUPPORTED_RESERVED_CLAIMS
    if missing_claims:
        failures.append(
            f"{label}: unsupported policy missing reserved claims "
            f"{sorted(missing_claims)}"
        )
    if extra_claims:
        failures.append(
            f"{label}: unsupported policy has unexpected reserved claims "
            f"{sorted(extra_claims)}"
        )
    if "Objective-C 3 runtime surfaces only" not in str(
        policy.get("source_debug_policy", "")
    ):
        failures.append(f"{label}: source/debug policy must stay Objective-C 3 scoped")


def _validate_runtime_source_debug_links(
    contract_links: list[dict[str, Any]],
    bundle_links: list[dict[str, Any]],
    runtime_records_by_id: dict[str, dict[str, Any]],
    source_graph_by_id: dict[str, dict[str, Any]],
    debug_map_records_by_id: dict[str, dict[str, Any]],
    abi_records_by_id: dict[str, dict[str, Any]],
    source_maps: dict[str, Any],
    debug_maps: dict[str, Any],
    line_rows: dict[str, Any],
    failures: list[str],
    *,
    label: str,
) -> dict[str, Any]:
    link_label = f"{label}.runtime_state_source_debug_links"
    contract_by_id = _index_json_records(
        contract_links,
        key="link_id",
        failures=failures,
        label=link_label,
    )
    bundle_by_id = _index_json_records(
        bundle_links,
        key="link_id",
        failures=failures,
        label=f"{label}.canonical_source_debug_map.runtime_state_links",
    )

    expected_ids = set(REQUIRED_RUNTIME_SOURCE_DEBUG_LINKS)
    missing_contract_links = expected_ids - set(contract_by_id)
    extra_contract_links = set(contract_by_id) - expected_ids
    if missing_contract_links:
        failures.append(
            f"{link_label}: missing required links {sorted(missing_contract_links)}"
        )
    if extra_contract_links:
        failures.append(
            f"{link_label}: unexpected links {sorted(extra_contract_links)}"
        )
    missing_bundle_links = expected_ids - set(bundle_by_id)
    extra_bundle_links = set(bundle_by_id) - expected_ids
    if missing_bundle_links:
        failures.append(
            f"{label}.canonical_source_debug_map.runtime_state_links: "
            f"missing required links {sorted(missing_bundle_links)}"
        )
    if extra_bundle_links:
        failures.append(
            f"{label}.canonical_source_debug_map.runtime_state_links: "
            f"unexpected links {sorted(extra_bundle_links)}"
        )

    covered_runtime_state_ids: set[str] = set()
    matched_ids: set[str] = set()
    for link_id in sorted(expected_ids & set(contract_by_id)):
        record = contract_by_id[link_id]
        record_label = f"{link_label}.{link_id}"
        required_features = REQUIRED_RUNTIME_SOURCE_DEBUG_LINKS[link_id]
        features = _validate_feature_list(
            record.get("features"),
            failures,
            label=record_label,
            minimum=len(required_features),
        )
        if features != required_features:
            failures.append(f"{record_label}: features drifted")

        bundle_record = bundle_by_id.get(link_id)
        if bundle_record is None:
            continue
        matched_ids.add(link_id)
        bundle_features = {
            str(feature) for feature in _as_list(bundle_record.get("features"))
        }
        if bundle_features != features:
            failures.append(f"{record_label}: bundle features drifted")

        source_graph_record_id = str(record.get("source_graph_record_id", ""))
        source_graph_record = source_graph_by_id.get(source_graph_record_id)
        debug_map_record_id = str(record.get("debug_map_record_id", ""))
        debug_record = debug_map_records_by_id.get(debug_map_record_id)
        source_graph_node_id = str(record.get("source_graph_node_id", ""))
        source_map_entry_id = str(record.get("source_map_entry_id", ""))
        debug_map_entry_id = str(record.get("debug_map_entry_id", ""))
        native_line_table_row_ids = tuple(
            str(row_id) for row_id in _as_list(record.get("native_line_table_row_ids"))
        )
        runtime_anchor_ids = tuple(
            str(anchor_id) for anchor_id in _as_list(record.get("runtime_anchor_ids"))
        )
        runtime_state_record_ids = tuple(
            str(runtime_id)
            for runtime_id in _as_list(record.get("runtime_state_record_ids"))
        )
        abi_record_ids = tuple(
            str(abi_id) for abi_id in _as_list(record.get("abi_record_ids"))
        )

        for key in (
            "runtime_state_record_ids",
            "source_graph_record_id",
            "debug_map_record_id",
            "abi_record_ids",
            "source_graph_node_id",
            "source_map_entry_id",
            "debug_map_entry_id",
            "native_line_table_row_ids",
            "runtime_anchor_ids",
        ):
            contract_value = record.get(key)
            bundle_value = bundle_record.get(key)
            if contract_value != bundle_value:
                failures.append(f"{record_label}: bundle {key} drifted")

        if source_graph_record is None:
            failures.append(
                f"{record_label}: missing source graph record {source_graph_record_id}"
            )
        elif source_graph_record.get("source_graph_node_id") != source_graph_node_id:
            failures.append(f"{record_label}: source graph node drifted")
        else:
            source_features = {
                str(feature) for feature in _as_list(source_graph_record.get("features"))
            }
            if not source_features <= features:
                failures.append(f"{record_label}: source graph features drifted")

        if debug_record is None:
            failures.append(f"{record_label}: missing debug map record {debug_map_record_id}")
        else:
            if debug_record.get("source_graph_record_id") != source_graph_record_id:
                failures.append(f"{record_label}: debug record source graph drifted")
            if debug_record.get("source_graph_node_id") != source_graph_node_id:
                failures.append(f"{record_label}: debug record source node drifted")
            if debug_record.get("source_map_entry_id") != source_map_entry_id:
                failures.append(f"{record_label}: debug record source-map entry drifted")
            if debug_record.get("debug_map_entry_id") != debug_map_entry_id:
                failures.append(f"{record_label}: debug record debug-map entry drifted")
            if tuple(_as_list(debug_record.get("runtime_anchor_ids"))) != runtime_anchor_ids:
                failures.append(f"{record_label}: debug record runtime anchors drifted")

        source_map = source_maps.get(source_map_entry_id)
        if source_map is None:
            failures.append(f"{record_label}: canonical source-map entry missing")
        else:
            if source_map.source_graph_node_id != source_graph_node_id:
                failures.append(f"{record_label}: source-map source node drifted")
            missing_source_map_anchors = set(runtime_anchor_ids) - set(
                source_map.runtime_anchor_ids
            )
            if missing_source_map_anchors:
                failures.append(
                    f"{record_label}: source-map runtime anchors missing "
                    f"{sorted(missing_source_map_anchors)}"
                )

        debug_map = debug_maps.get(debug_map_entry_id)
        if debug_map is None:
            failures.append(f"{record_label}: canonical debug-map entry missing")
        else:
            if debug_map.source_map_entry_id != source_map_entry_id:
                failures.append(f"{record_label}: debug-map source-map entry drifted")
            if debug_map.source_graph_node_id != source_graph_node_id:
                failures.append(f"{record_label}: debug-map source node drifted")
            if tuple(debug_map.runtime_anchor_ids) != runtime_anchor_ids:
                failures.append(f"{record_label}: debug-map runtime anchors drifted")

        if not native_line_table_row_ids:
            failures.append(f"{record_label}: native line-table row ids are required")
        for row_id in native_line_table_row_ids:
            line_row = line_rows.get(row_id)
            if line_row is None:
                failures.append(f"{record_label}: missing native line-table row {row_id}")
                continue
            if line_row.source_map_entry_id != source_map_entry_id:
                failures.append(f"{record_label}: line-table source-map entry drifted")
            if line_row.source_graph_node_id != source_graph_node_id:
                failures.append(f"{record_label}: line-table source node drifted")

        if not runtime_state_record_ids:
            failures.append(f"{record_label}: runtime_state_record_ids are required")
        for runtime_id in runtime_state_record_ids:
            runtime_record = runtime_records_by_id.get(runtime_id)
            if runtime_record is None:
                failures.append(f"{record_label}: missing runtime state record {runtime_id}")
                continue
            covered_runtime_state_ids.add(runtime_id)
            runtime_features = {
                str(feature) for feature in _as_list(runtime_record.get("features"))
            }
            if not runtime_features & features:
                failures.append(
                    f"{record_label}: runtime state record {runtime_id} "
                    "does not intersect link features"
                )

        if not abi_record_ids:
            failures.append(f"{record_label}: abi_record_ids are required")
        for abi_id in abi_record_ids:
            abi_record = abi_records_by_id.get(abi_id)
            if abi_record is None:
                failures.append(f"{record_label}: missing ABI record {abi_id}")
                continue
            abi_features = {
                str(feature) for feature in _as_list(abi_record.get("features"))
            }
            if not abi_features & features:
                failures.append(
                    f"{record_label}: ABI record {abi_id} does not intersect link features"
                )

    return {
        "link_count": len(matched_ids),
        "covered_runtime_state_record_ids": sorted(covered_runtime_state_ids),
    }


def _validate_canonical_source_debug_map_bundle(
    contract: dict[str, Any],
    runtime_records_by_id: dict[str, dict[str, Any]],
    source_graph_by_id: dict[str, dict[str, Any]],
    debug_map_records_by_id: dict[str, dict[str, Any]],
    abi_records_by_id: dict[str, dict[str, Any]],
    runtime_source_debug_links: list[dict[str, Any]],
    failures: list[str],
    *,
    label: str,
) -> dict[str, Any]:
    bundle_path = str(contract.get("canonical_compiler_emitted_source_debug_map_bundle", ""))
    if bundle_path != ADVANCED_CLOSURE_CANONICAL_SOURCE_DEBUG_MAP_BUNDLE:
        failures.append(f"{label}: canonical source/debug-map bundle path drifted")
    if contract.get("canonical_source_debug_map_public_command") != (
        "npm run objc3c -- validate-advanced-runtime-closure"
    ):
        failures.append(f"{label}: canonical source/debug-map public command drifted")

    if not bundle_path:
        return {
            "path": bundle_path,
            "source_map_record_count": 0,
            "debug_map_record_count": 0,
            "native_line_table_record_count": 0,
            "runtime_source_debug_link_count": 0,
            "covered_runtime_state_record_ids": [],
        }
    if _is_forbidden_path(bundle_path):
        failures.append(f"{label}: canonical source/debug-map bundle is not checked source: {bundle_path}")
        return {
            "path": bundle_path,
            "source_map_record_count": 0,
            "debug_map_record_count": 0,
            "native_line_table_record_count": 0,
            "runtime_source_debug_link_count": 0,
            "covered_runtime_state_record_ids": [],
        }

    resolved_bundle_path = ROOT / bundle_path
    validation = validate_bundle_path(resolved_bundle_path)
    if not validation.ok:
        failures.extend(
            f"{label}.canonical_source_debug_map: {diagnostic.code}: {diagnostic.message}"
            for diagnostic in validation.diagnostics
        )
        return {
            "path": bundle_path,
            "source_map_record_count": 0,
            "debug_map_record_count": 0,
            "native_line_table_record_count": 0,
            "runtime_source_debug_link_count": 0,
            "covered_runtime_state_record_ids": [],
        }

    bundle = load_bundle(resolved_bundle_path)
    source_maps = {entry.entry_id: entry for entry in bundle.source_maps}
    debug_maps = {entry.entry_id: entry for entry in bundle.debug_maps}
    line_rows = {entry.row_id: entry for entry in bundle.native_line_tables}
    line_table_source_map_ids = {
        row.source_map_entry_id for row in bundle.native_line_tables
    }
    bundle_runtime_source_debug_links = [
        record
        for record in _as_list(bundle.payload.get("runtime_state_links"))
        if isinstance(record, dict)
    ]

    matched_source_map_ids: set[str] = set()
    matched_debug_map_ids: set[str] = set()
    matched_line_table_source_map_ids: set[str] = set()
    for index, record in enumerate(debug_map_records_by_id.values()):
        record_label = f"{label}.canonical_source_debug_map.debug_map_records[{index}]"
        source_graph_record_id = str(record.get("source_graph_record_id", ""))
        source_graph_record = source_graph_by_id.get(source_graph_record_id)
        source_map_entry_id = str(record.get("source_map_entry_id", ""))
        debug_map_entry_id = str(record.get("debug_map_entry_id", ""))
        source_graph_node_id = str(record.get("source_graph_node_id", ""))
        runtime_anchor_ids = tuple(str(item) for item in _as_list(record.get("runtime_anchor_ids")))
        language_anchor_ids = {
            str(item) for item in _as_list(record.get("language_anchor_ids"))
        }

        source_map = source_maps.get(source_map_entry_id)
        if source_map is None:
            failures.append(
                f"{record_label}: canonical source-map entry missing: {source_map_entry_id}"
            )
        else:
            matched_source_map_ids.add(source_map.entry_id)
            if source_map.source_file != ADVANCED_CLOSURE_POSITIVE_FIXTURE:
                failures.append(f"{record_label}: source-map fixture path drifted")
            if source_map.source_graph_node_id != source_graph_node_id:
                failures.append(f"{record_label}: source-map source graph node drifted")
            if source_graph_record is not None and source_map.record_kind != str(
                source_graph_record.get("source_map_record_kind", "")
            ):
                failures.append(f"{record_label}: source-map record kind drifted")
            if source_map.entry_id not in line_table_source_map_ids:
                failures.append(f"{record_label}: native line-table row is missing")
            else:
                matched_line_table_source_map_ids.add(source_map.entry_id)
            missing_runtime_anchors = set(runtime_anchor_ids) - set(source_map.runtime_anchor_ids)
            if missing_runtime_anchors:
                failures.append(
                    f"{record_label}: source-map runtime anchors missing: "
                    f"{sorted(missing_runtime_anchors)}"
                )

        debug_map = debug_maps.get(debug_map_entry_id)
        if debug_map is None:
            failures.append(
                f"{record_label}: canonical debug-map entry missing: {debug_map_entry_id}"
            )
        else:
            matched_debug_map_ids.add(debug_map.entry_id)
            if debug_map.source_map_entry_id != source_map_entry_id:
                failures.append(f"{record_label}: debug-map source-map entry drifted")
            if debug_map.source_graph_node_id != source_graph_node_id:
                failures.append(f"{record_label}: debug-map source graph node drifted")
            if tuple(debug_map.runtime_anchor_ids) != runtime_anchor_ids:
                failures.append(f"{record_label}: debug-map runtime anchors drifted")
            actual_language_anchors = {
                debug_map.hover_anchor_id,
                debug_map.definition_anchor_id,
            }
            missing_language_anchors = language_anchor_ids - actual_language_anchors
            if missing_language_anchors:
                failures.append(
                    f"{record_label}: debug-map language anchors missing: "
                    f"{sorted(missing_language_anchors)}"
                )

    runtime_source_debug = _validate_runtime_source_debug_links(
        runtime_source_debug_links,
        bundle_runtime_source_debug_links,
        runtime_records_by_id,
        source_graph_by_id,
        debug_map_records_by_id,
        abi_records_by_id,
        source_maps,
        debug_maps,
        line_rows,
        failures,
        label=label,
    )

    return {
        "path": bundle_path,
        "source_map_record_count": len(matched_source_map_ids),
        "debug_map_record_count": len(matched_debug_map_ids),
        "native_line_table_record_count": len(matched_line_table_source_map_ids),
        "runtime_source_debug_link_count": runtime_source_debug.get("link_count"),
        "covered_runtime_state_record_ids": runtime_source_debug.get(
            "covered_runtime_state_record_ids"
        ),
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
    missing_literal_cases = REQUIRED_LITERAL_NEGATIVE_CASE_IDS - seen_case_ids
    if missing_literal_cases:
        failures.append(
            "advanced runtime negative matrix missing literal issue cases: "
            f"{sorted(missing_literal_cases)}"
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
    _validate_unsupported_combination_policy(contract, failures, label=label)

    _paths_exist(
        [
            str(contract.get("language_semantics_contract", "")),
            str(contract.get("debug_source_map_validator", "")),
            str(contract.get("canonical_compiler_emitted_source_debug_map_bundle", "")),
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
    runtime_source_debug_links = [
        record
        for record in _as_list(contract.get("runtime_state_source_debug_links"))
        if isinstance(record, dict)
    ]
    runtime_source_debug_exemptions = [
        record
        for record in _as_list(contract.get("runtime_state_source_debug_exemptions"))
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
    runtime_records_by_id: dict[str, dict[str, Any]] = {}
    for index, record in enumerate(runtime_records):
        record_label = f"{label}.runtime_state_records[{index}]"
        record_id = str(record.get("record_id", ""))
        if not record_id:
            failures.append(f"{record_label}: record_id is required")
        elif record_id in runtime_ids:
            failures.append(f"{record_label}: duplicate record_id {record_id}")
        runtime_ids.add(record_id)
        runtime_records_by_id[record_id] = record
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
    debug_map_records_by_id: dict[str, dict[str, Any]] = {}
    for index, record in enumerate(debug_map_records):
        record_label = f"{label}.debug_map_records[{index}]"
        record_id = str(record.get("record_id", ""))
        if not record_id:
            failures.append(f"{record_label}: record_id is required")
        elif record_id in debug_ids:
            failures.append(f"{record_label}: duplicate record_id {record_id}")
        debug_ids.add(record_id)
        debug_map_records_by_id[record_id] = record
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
    abi_records_by_id: dict[str, dict[str, Any]] = {}
    for index, record in enumerate(abi_records):
        record_label = f"{label}.abi_interaction_records[{index}]"
        record_id = str(record.get("record_id", ""))
        if not record_id:
            failures.append(f"{record_label}: record_id is required")
        elif record_id in abi_ids:
            failures.append(f"{record_label}: duplicate record_id {record_id}")
        abi_ids.add(record_id)
        abi_records_by_id[record_id] = record
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
    available_runtime_source_debug_link_ids = {
        str(record.get("link_id", ""))
        for record in runtime_source_debug_links
        if isinstance(record, dict)
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
        runtime_source_debug_link_ids = {
            str(link_id)
            for link_id in _as_list(record.get("runtime_source_debug_link_ids"))
        }
        if not runtime_source_debug_link_ids:
            failures.append(f"{record_label}: runtime source/debug link ids are required")
        for link_id in runtime_source_debug_link_ids:
            if link_id not in REQUIRED_RUNTIME_SOURCE_DEBUG_LINKS:
                failures.append(
                    f"{record_label}: missing runtime source/debug link {link_id}"
                )
            if link_id not in available_runtime_source_debug_link_ids:
                failures.append(
                    f"{record_label}: runtime source/debug link not declared {link_id}"
                )

    missing_interactions = set(REQUIRED_INTERACTION_FEATURE_SETS) - seen_interactions
    if missing_interactions:
        failures.append(f"{label}: missing interaction records {sorted(missing_interactions)}")

    canonical_source_debug_map = _validate_canonical_source_debug_map_bundle(
        contract,
        runtime_records_by_id,
        source_graph_by_id,
        debug_map_records_by_id,
        abi_records_by_id,
        runtime_source_debug_links,
        failures,
        label=label,
    )
    covered_runtime_ids = {
        str(runtime_id)
        for runtime_id in _as_list(
            canonical_source_debug_map.get("covered_runtime_state_record_ids")
        )
    }
    exemption_ids: set[str] = set()
    for index, record in enumerate(runtime_source_debug_exemptions):
        record_label = f"{label}.runtime_state_source_debug_exemptions[{index}]"
        runtime_id = str(record.get("runtime_state_record_id", ""))
        if runtime_id not in runtime_ids:
            failures.append(f"{record_label}: missing runtime state record {runtime_id}")
        exemption_ids.add(runtime_id)
        if not str(record.get("reason", "")):
            failures.append(f"{record_label}: reason is required")
    missing_exemptions = REQUIRED_RUNTIME_SOURCE_DEBUG_EXEMPTIONS - exemption_ids
    extra_exemptions = exemption_ids - REQUIRED_RUNTIME_SOURCE_DEBUG_EXEMPTIONS
    if missing_exemptions:
        failures.append(
            f"{label}: missing runtime source/debug exemptions "
            f"{sorted(missing_exemptions)}"
        )
    if extra_exemptions:
        failures.append(
            f"{label}: unexpected runtime source/debug exemptions "
            f"{sorted(extra_exemptions)}"
        )
    unlinked_runtime_ids = runtime_ids - covered_runtime_ids - exemption_ids
    if unlinked_runtime_ids:
        failures.append(
            f"{label}: runtime state records lack source/debug proof "
            f"{sorted(unlinked_runtime_ids)}"
        )

    return {
        "path": ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT,
        "runtime_state_record_count": len(runtime_records),
        "source_graph_record_count": len(source_graph_records),
        "debug_map_record_count": len(debug_map_records),
        "abi_interaction_record_count": len(abi_records),
        "interaction_count": len(interaction_records),
        "interactions": sorted(seen_interactions),
        "canonical_source_debug_map": canonical_source_debug_map.get("path"),
        "canonical_source_map_record_count": canonical_source_debug_map.get(
            "source_map_record_count"
        ),
        "canonical_debug_map_record_count": canonical_source_debug_map.get(
            "debug_map_record_count"
        ),
        "canonical_native_line_table_record_count": canonical_source_debug_map.get(
            "native_line_table_record_count"
        ),
        "runtime_source_debug_link_count": canonical_source_debug_map.get(
            "runtime_source_debug_link_count"
        ),
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
        "canonical_source_debug_map_bundle": (
            ADVANCED_CLOSURE_CANONICAL_SOURCE_DEBUG_MAP_BUNDLE
        ),
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
        "canonical_source_debug_map_evidence": True,
    }
    for key, expected_value in expected_bool_fields.items():
        if row.get(key) is not expected_value:
            failures.append(
                f"#8199 language semantics row {key} must be {expected_value}"
            )
    expected_int_fields = {
        "combined_runtime_state_record_count": 8,
        "canonical_source_map_record_count": 7,
        "canonical_debug_map_record_count": 7,
        "canonical_native_line_table_record_count": 7,
        "combined_interaction_record_count": len(REQUIRED_INTERACTION_FEATURE_SETS),
    }
    for key, expected_value in expected_int_fields.items():
        if row.get(key) != expected_value:
            failures.append(
                f"#8199 language semantics row {key} must be {expected_value}"
            )
    _paths_exist(
        [
            str(row.get("combined_fixture", "")),
            str(row.get("combined_contract", "")),
            str(row.get("canonical_source_debug_map_bundle", "")),
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
        "advanced_runtime_canonical_source_debug_map": combined_identity.get(
            "canonical_source_debug_map"
        ),
        "advanced_runtime_canonical_source_map_record_count": combined_identity.get(
            "canonical_source_map_record_count"
        ),
        "advanced_runtime_canonical_debug_map_record_count": combined_identity.get(
            "canonical_debug_map_record_count"
        ),
        "advanced_runtime_canonical_native_line_table_record_count": combined_identity.get(
            "canonical_native_line_table_record_count"
        ),
        "advanced_runtime_runtime_source_debug_link_count": combined_identity.get(
            "runtime_source_debug_link_count"
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
