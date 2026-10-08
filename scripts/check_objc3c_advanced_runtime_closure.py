#!/usr/bin/env python3
"""Validate the combined advanced runtime closure support surface."""

from __future__ import annotations

import json
import os
import shutil
import subprocess
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
from scripts.objc3c_runtime_acceptance.compile_backends import (
    DIRECT_COMPILE_BACKEND,
    compile_command,
)
from scripts.objc3c_tooling.llvm_discovery import find_llvm_tool_path

LANGUAGE_SEMANTICS_CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "objc3c"
    / "language_semantics_runtime_api_contract.json"
)
LANGUAGE_SEMANTICS_HEADER_PATH = (
    ROOT
    / "native"
    / "objc3c"
    / "src"
    / "runtime"
    / "public"
    / "objc3_runtime_language_semantics.h"
)
LANGUAGE_SEMANTICS_IMPLEMENTATION_PATH = LANGUAGE_SEMANTICS_HEADER_PATH.with_suffix(
    ".cpp"
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
ADVANCED_CLOSURE_NATIVE_ARTIFACT_CONTRACT = (
    "tests/tooling/fixtures/advanced_runtime_closure/"
    "native_artifact_contract.json"
)
ADVANCED_CLOSURE_PROVIDER_FIXTURE = (
    f"{ADVANCED_CLOSURE_FIXTURE_DIR}/combined_provider.objc3"
)
ADVANCED_CLOSURE_NATIVE_ARTIFACT_CONTRACT_ID = (
    "objc3c.advanced-runtime.closure.native-artifact.v1"
)
ADVANCED_CLOSURE_EXECUTABLE_CONTRACT = (
    "tests/tooling/fixtures/advanced_runtime_closure/"
    "executable_runtime_contract_surfaces.json"
)
ADVANCED_CLOSURE_EXECUTABLE_CONTRACT_ID = (
    "objc3c.advanced-runtime.executable-contract-surfaces.v1"
)
ADVANCED_CLOSURE_INTEGRATION_CONTRACT = (
    "tests/tooling/fixtures/advanced_runtime_closure/"
    "advanced_runtime_integration_closure_contract.json"
)
ADVANCED_CLOSURE_INTEGRATION_CONTRACT_ID = (
    "objc3c.advanced-runtime.integration-runtime-closure.v1"
)
ADVANCED_CLOSURE_NATIVE_ATTEMPT_DIR = (
    "tmp/artifacts/advanced-runtime-closure/native-executable-umbrella/compile"
)
REQUIRED_CLOSURE_FEATURES = {
    "ownership",
    "blocks",
    "async",
    "actor",
    "cancellation",
    "error",
    "interop",
    "property",
    "macro",
    "package_replay",
}
REQUIRED_PROOF_AXES = {"runtime_state", "source_graph", "debug_map", "abi_surface"}
ALLOWED_NEGATIVE_CASE_ISSUE_REFS = {8199, 8214, 8215, 8216, 8217}
REQUIRED_RUNTIME_SOURCE_DEBUG_LINKS = {
    "runtime-source-debug.ownership-block": frozenset({"ownership", "blocks"}),
    "runtime-source-debug.async-cancellation": frozenset({"async", "cancellation"}),
    "runtime-source-debug.actor-mailbox": frozenset({"actor", "async"}),
    "runtime-source-debug.actor-mailbox-expanded": frozenset(
        {"actor", "async", "cancellation", "error"}
    ),
    "runtime-source-debug.error-bridge": frozenset({"error", "async"}),
    "runtime-source-debug.property-behavior": frozenset({"property"}),
    "runtime-source-debug.macro-provenance": frozenset({"macro"}),
    "runtime-source-debug.package-replay": frozenset({"package_replay"}),
    "runtime-source-debug.macro-package-replay": frozenset(
        {"macro", "package_replay"}
    ),
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
REQUIRED_INTEGRATION_RESERVED_CLAIMS = {
    "broad-scheduler-guarantees",
    "swift-cpp-abi-mirroring",
    "distributed-actor-networking",
    "arbitrary-macro-host-execution",
}
REQUIRED_INTEGRATION_SURFACE_KINDS = {
    "scheduler-task-runtime-contract",
    "foreign-abi-runtime-contract",
    "actor-mailbox-runtime-expansion-contract",
    "macro-package-replay-runtime-contract",
}
REQUIRED_INTEGRATION_NEGATIVE_CASE_IDS = {
    "scheduler_priority_fairness_overclaim",
    "distributed_actor_transport_overclaim",
    "macro_host_arbitrary_execution",
    "macro_package_replay_stale_cache",
    "foreign_abi_unsafe_mixed_image",
}
REQUIRED_INTERACTION_FEATURE_SETS = {
    "ownership-block": frozenset({"ownership", "blocks"}),
    "block-async-error": frozenset({"blocks", "async", "error"}),
    "async-actor-cancellation": frozenset({"async", "actor", "cancellation"}),
    "actor-property-macro": frozenset({"actor", "property", "macro"}),
    "macro-package-replay": frozenset({"macro", "package_replay"}),
    "error-package-replay": frozenset({"error", "package_replay"}),
    "foreign-abi-package-replay": frozenset({"interop", "package_replay"}),
    "foreign-abi-ownership": frozenset({"interop", "ownership"}),
    "foreign-abi-async-error": frozenset({"interop", "async", "error"}),
}
REQUIRED_LITERAL_NEGATIVE_CASE_IDS = {
    "actor_mailbox_cancelled_queue",
    "actor_mailbox_error_without_replay",
    "actor_mailbox_stale_identity",
    "actor_mailbox_unsupported_payload",
    "distributed_actor_missing_transport",
    "distributed_actor_transport_overclaim",
    "foreign_abi_mismatch",
    "foreign_abi_missing_bridge_ownership",
    "foreign_abi_stale_import",
    "foreign_abi_unsafe_mixed_image",
    "foreign_abi_unsupported_runtime_fallback",
    "macro_package_replay_host_mismatch",
    "macro_package_replay_missing_metadata",
    "macro_package_replay_stale_cache",
    "macro_package_replay_tampered_metadata",
    "macro_host_arbitrary_execution",
    "property_behavior_conflict",
    "scheduler_cancellation_error_cleanup_missing_evidence",
    "scheduler_guarantee_overclaim",
    "scheduler_priority_fairness_overclaim",
    "scheduler_shutdown_drain_missing_evidence",
    "scheduler_task_lifecycle_missing_replay",
}
REQUIRED_NEGATIVE_DIAGNOSTIC_COMMENT_PREFIX = (
    "// Expected advanced-runtime diagnostic: "
)
REQUIRED_ADVANCED_RUNTIME_NATIVE_SNAPSHOT_FIELDS = {
    "native_artifact_evidence",
    "native_executable_umbrella_support",
    "native_artifact_contract",
}
NATIVE_PARALLELISM_CAP_ENV = {
    "CMAKE_BUILD_PARALLEL_LEVEL": "2",
    "CL_MPCount": "2",
    "LLVM_PARALLEL_COMPILE_JOBS": "2",
    "OBJC3C_NATIVE_BUILD_PARALLELISM": "2",
}
REQUIRED_NATIVE_ARTIFACTS = (
    "module.obj",
    "module.ll",
    "module.manifest.json",
    "module.runtime-registration-manifest.json",
    "module.runtime-metadata.bin",
    "module.error_handling-error-replay.json",
    "module.exe",
)
FORBIDDEN_NATIVE_SUCCESS_ARTIFACTS: tuple[str, ...] = ()
FORBIDDEN_NATIVE_LLVM_OPERAND_MARKERS = ("|%", "%|")
NATIVE_PHASE_STATUS_CONTRACT_ID = (
    "objc3c.advanced-runtime.closure.typed-failure-reporting.v1"
)
NATIVE_PHASE_STATUS_ORDER = (
    "compile",
    "link",
    "runtime_registration",
    "runtime_metadata",
    "error_replay",
    "execution_status",
)
NATIVE_PHASE_STATUS_VALUES = {
    "compile": (
        "native_compile_succeeded",
        "native_compile_failed",
        "native_compile_launch_failed",
        "native_object_emission_missing_llc",
        "native_object_emission_filetype_obj_unavailable",
        "native_object_emission_backend_unavailable",
    ),
    "link": (
        "native_link_succeeded",
        "native_link_failed",
        "native_link_unclaimed",
    ),
    "runtime_registration": (
        "runtime_registration_artifact_present",
        "runtime_registration_artifact_missing",
    ),
    "runtime_metadata": (
        "runtime_metadata_artifact_present",
        "runtime_metadata_artifact_missing",
    ),
    "error_replay": (
        "error_replay_artifact_present",
        "error_replay_artifact_missing",
    ),
    "execution_status": (
        "native_execution_succeeded",
        "native_execution_failed",
        "native_execution_unclaimed",
    ),
}
NATIVE_ARTIFACT_PHASES = {
    "module.runtime-registration-manifest.json": "runtime_registration",
    "module.runtime-metadata.bin": "runtime_metadata",
    "module.error_handling-error-replay.json": "error_replay",
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
    "interop": (
        "objc_foreign",
        "objc_import_module(named(\"AdvancedRuntimeClosureKit\"))",
        "objc_mixed_image(named(\"AdvancedRuntimeClosureKitCore\"))",
    ),
    "property": (
        "@property (strong, behavior=Observed) id value",
        "@property (readonly, strong, behavior=Projected, getter=currentValue) id currentValue",
    ),
    "macro": (
        "objc_macro_package(named(\"std.metaprogramming.advanced-runtime\"))",
        "objc_macro_provenance(named(\"sha256:5883f2e2120170bb3de07348e4f101cd478379337b6c0ce2a0f751b29da70e4f\"))",
        "objc_macro_sandbox(named(\"deterministic\"))",
    ),
    "package_replay": (
        "objc_import_module(named(\"AdvancedRuntimeClosureKit\"))",
        "objc_mixed_image(named(\"AdvancedRuntimeClosureKitCore\"))",
        "objc_package_entry(named(\"AdvancedRuntimeClosureKit.replay\"))",
    ),
}
REQUIRED_RUNTIME_EXECUTION_ENTRYPOINT = "advancedRuntimeExecutableEntry"
REQUIRED_RUNTIME_EXECUTION_ENTRYPOINT_TOKENS = (
    "fn advancedRuntimeExecutableEntry() -> i32",
    "combinedAdvancedRuntimeClosure(11, 11, nil)",
    "asyncCancellationLane(6)",
    "schedulerAndActorRuntimeExecutionLane(4, 41)",
    "return advancedRuntimeExecutableEntry()",
)
REQUIRED_RUNTIME_EXECUTION_OBSERVATIONS = {
    "ownership_blocks_errors_macro_package",
    "async_task_group_cancellation",
    "scheduler_executor_hop",
    "actor_mailbox_enqueue_drain",
}
ALLOWED_NEGATIVE_STATUSES = {"rejected", "reserved"}
DOCS_SUPPORT_CAPABILITY_MATRIX = ROOT / "docs" / "support" / "capability_matrix.json"
DOCS_SUPPORT_EVIDENCE_MAP = ROOT / "docs" / "support" / "evidence_map.json"
REQUIRED_DOCS_SUPPORT_EVIDENCE = {
    "scripts/check_objc3c_advanced_runtime_closure.py",
    ADVANCED_CLOSURE_POSITIVE_FIXTURE,
    ADVANCED_CLOSURE_PROVIDER_FIXTURE,
    ADVANCED_CLOSURE_NEGATIVE_MATRIX,
    ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT,
    ADVANCED_CLOSURE_NATIVE_ARTIFACT_CONTRACT,
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


def _validate_promotion_boundary(
    boundary: Any,
    failures: list[str],
    *,
    label: str,
) -> None:
    if not isinstance(boundary, dict):
        failures.append(f"{label}: promotion boundary is required")
        return
    if boundary.get("contract_fixture") != ADVANCED_CLOSURE_INTEGRATION_CONTRACT:
        failures.append(f"{label}: integration contract fixture drifted")
    if int(boundary.get("umbrella_issue_ref", 0)) != 8203:
        failures.append(f"{label}: umbrella issue alignment drifted")
    if int(boundary.get("promotion_issue_ref", 0)) != 8218:
        failures.append(f"{label}: promotion issue alignment drifted")
    if boundary.get("supported_runtime_envelope") != "integrated-objective-c-3-runtime":
        failures.append(f"{label}: runtime envelope must stay Objective-C 3 bounded")
    reserved_claims = {
        str(claim) for claim in _as_list(boundary.get("reserved_claims"))
    }
    if reserved_claims != REQUIRED_INTEGRATION_RESERVED_CLAIMS:
        failures.append(f"{label}: reserved promotion claims drifted")
    required_case_ids = {
        str(case_id)
        for case_id in _as_list(boundary.get("required_negative_case_ids"))
    }
    if required_case_ids != REQUIRED_INTEGRATION_NEGATIVE_CASE_IDS:
        failures.append(f"{label}: promotion negative case ids drifted")
    _paths_exist(
        [str(boundary.get("contract_fixture", ""))],
        failures,
        label,
    )


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
    _validate_required_tokens(
        ADVANCED_CLOSURE_POSITIVE_FIXTURE,
        list(REQUIRED_RUNTIME_EXECUTION_ENTRYPOINT_TOKENS),
        failures,
        label="advanced_runtime_closure.positive_fixture.runtime_execution_entrypoint",
    )
    return {
        "path": ADVANCED_CLOSURE_POSITIVE_FIXTURE,
        "features": sorted(REQUIRED_CLOSURE_FEATURES),
        "runtime_execution_entrypoint": REQUIRED_RUNTIME_EXECUTION_ENTRYPOINT,
        "runtime_execution_observations": sorted(
            REQUIRED_RUNTIME_EXECUTION_OBSERVATIONS
        ),
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
    seen_diagnostics: set[str] = set()
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
        if int(case.get("issue_ref", 0)) not in ALLOWED_NEGATIVE_CASE_ISSUE_REFS:
            failures.append(
                f"{label}: case must be issue-scoped to #8199/#8214/#8215/#8216/#8217"
            )
        if str(case.get("status", "")) not in ALLOWED_NEGATIVE_STATUSES:
            failures.append(f"{label}: status must remain rejected or reserved")
        diagnostic = str(case.get("expected_diagnostic", ""))
        if not diagnostic.startswith("advanced-runtime."):
            failures.append(f"{label}: expected_diagnostic must be advanced-runtime scoped")
        elif diagnostic in seen_diagnostics:
            failures.append(f"{label}: duplicate expected_diagnostic {diagnostic}")
        seen_diagnostics.add(diagnostic)
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
        fixture_text = _source_text(
            fixture,
            failures,
            label=label,
            forbidden_prefixes=forbidden_prefixes,
        )
        diagnostic_anchor = f"{REQUIRED_NEGATIVE_DIAGNOSTIC_COMMENT_PREFIX}{diagnostic}"
        if diagnostic_anchor not in fixture_text:
            failures.append(f"{label}: missing deterministic diagnostic anchor")

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


def _validate_executable_runtime_contract(failures: list[str]) -> dict[str, Any]:
    contract = _load_json(ROOT / ADVANCED_CLOSURE_EXECUTABLE_CONTRACT)
    label = "advanced_runtime_closure.executable_runtime_contract"
    if contract.get("contract_id") != ADVANCED_CLOSURE_EXECUTABLE_CONTRACT_ID:
        failures.append(f"{label}: contract_id drifted")
    if contract.get("schema_path") != (
        "schemas/objc3c-advanced-runtime-executable-contract-v1.schema.json"
    ):
        failures.append(f"{label}: schema_path drifted")

    runtime_contract = contract.get("runtime_public_contract")
    if not isinstance(runtime_contract, dict):
        failures.append(f"{label}: runtime_public_contract is required")
        runtime_contract = {}
    header_path = _normalized(str(runtime_contract.get("header", "")))
    implementation_path = _normalized(str(runtime_contract.get("implementation", "")))
    _paths_exist([header_path, implementation_path], failures, label)
    _validate_required_tokens(
        header_path,
        [
            "OBJC3_RUNTIME_EXECUTABLE_CONTRACT_ABI_VERSION 3u",
            "scheduler_queue_state_required",
            "task_lifecycle_state_required",
            "cancellation_checkpoint_required",
            "task_error_cleanup_required",
            "shutdown_drain_required",
            "imported_runtime_replay_required",
            "foreign_surface_classification_required",
            "foreign_typed_dispatch_expectation_required",
            "foreign_package_runtime_identity_required",
            "foreign_bridge_ownership_required",
            "unsafe_mixed_image_rejected",
            "unsupported_runtime_fallback_rejected",
            "actor_mailbox_message_identity_required",
            "actor_mailbox_fifo_drain_required",
            "actor_mailbox_cancel_evidence_required",
            "actor_mailbox_error_evidence_required",
            "actor_mailbox_shutdown_evidence_required",
            "distributed_actor_without_transport_rejected",
            "macro_package_lock_identity_required",
            "macro_package_trust_identity_required",
            "macro_host_identity_required",
            "macro_cache_identity_required",
            "macro_input_output_identity_required",
            "macro_runtime_consumption_identity_required",
            "macro_missing_replay_metadata_rejected",
        ],
        failures,
        label=f"{label}.runtime_public_contract.header",
    )
    _validate_required_tokens(
        implementation_path,
        [
            "objc3c.advanced-runtime.executable-contract.scheduler-task.v1",
            "objc3c.advanced-runtime.executable-contract.foreign-abi.v1",
            "objc3c.advanced-runtime.executable-contract.actor-mailbox.v1",
            "objc3c.advanced-runtime.executable-contract.macro-package-replay.v1",
            "scheduler_queue_state_required",
            "foreign_surface_classification_required",
            "actor_mailbox_message_identity_required",
            "macro_package_lock_identity_required",
        ],
        failures,
        label=f"{label}.runtime_public_contract.implementation",
    )

    surfaces = [
        surface
        for surface in _as_list(contract.get("surfaces"))
        if isinstance(surface, dict)
    ]
    surfaces_by_kind = _index_json_records(
        surfaces,
        key="surface_kind",
        failures=failures,
        label=f"{label}.surfaces",
    )
    for required_kind in (
        "scheduler-task-runtime-contract",
        "foreign-abi-runtime-contract",
        "actor-mailbox-runtime-expansion-contract",
        "macro-package-replay-runtime-contract",
    ):
        if required_kind not in surfaces_by_kind:
            failures.append(f"{label}: missing surface {required_kind}")

    integration = contract.get("integration_contract")
    if not isinstance(integration, dict):
        failures.append(f"{label}: integration_contract missing")
        integration = {}
    integration_contract_path = _normalized(
        str(integration.get("contract_fixture", ""))
    )
    _paths_exist([integration_contract_path], failures, f"{label}.integration")
    if integration.get("umbrella_issue_ref") != 8203:
        failures.append(f"{label}.integration: umbrella issue alignment drifted")
    if integration.get("promotion_issue_ref") != 8218:
        failures.append(f"{label}.integration: promotion issue alignment drifted")
    if integration.get("runtime_envelope") != "integrated-objective-c-3-runtime":
        failures.append(f"{label}.integration: runtime envelope drifted")
    integration_surface_kinds = {
        str(kind) for kind in _as_list(integration.get("required_surface_kinds"))
    }
    if integration_surface_kinds != REQUIRED_INTEGRATION_SURFACE_KINDS:
        failures.append(f"{label}.integration: required surface kinds drifted")
    integration_reserved_claims = {
        str(claim) for claim in _as_list(integration.get("required_reserved_claims"))
    }
    if integration_reserved_claims != REQUIRED_INTEGRATION_RESERVED_CLAIMS:
        failures.append(f"{label}.integration: reserved claims drifted")
    integration_case_ids = {
        str(case_id)
        for case_id in _as_list(integration.get("required_negative_case_ids"))
    }
    if integration_case_ids != REQUIRED_INTEGRATION_NEGATIVE_CASE_IDS:
        failures.append(f"{label}.integration: negative case ids drifted")
    matrix_contract = _load_json(ROOT / ADVANCED_CLOSURE_NEGATIVE_MATRIX)
    matrix_case_ids = {
        str(case.get("case_id", ""))
        for case in _as_list(matrix_contract.get("cases"))
        if isinstance(case, dict)
    }
    missing_integration_cases = (
        REQUIRED_INTEGRATION_NEGATIVE_CASE_IDS - matrix_case_ids
    )
    if missing_integration_cases:
        failures.append(
            f"{label}.integration: negative matrix missing integration cases "
            f"{sorted(missing_integration_cases)}"
        )
    if integration_contract_path:
        integration_contract = _load_json(ROOT / integration_contract_path)
        if integration_contract.get("contract_id") != (
            ADVANCED_CLOSURE_INTEGRATION_CONTRACT_ID
        ):
            failures.append(f"{label}.integration: contract fixture id drifted")
        if int(integration_contract.get("umbrella_issue_ref", 0)) != 8203:
            failures.append(f"{label}.integration: contract umbrella issue drifted")
        if int(integration_contract.get("promotion_issue_ref", 0)) != 8218:
            failures.append(f"{label}.integration: contract promotion issue drifted")
        if integration_contract.get("source_truth") != (
            "checked-in source files and checked-in contract fixtures only"
        ):
            failures.append(f"{label}.integration: source truth drifted")
        envelope = integration_contract.get("runtime_envelope")
        if not isinstance(envelope, dict):
            failures.append(f"{label}.integration: runtime envelope fixture missing")
            envelope = {}
        if envelope.get("supported_boundary") != (
            "integrated Objective-C 3 executable runtime envelope only"
        ):
            failures.append(f"{label}.integration: supported boundary drifted")
        if envelope.get("compatibility_modes_allowed") is not False:
            failures.append(f"{label}.integration: compatibility modes must stay false")
        if envelope.get("local_temp_evidence_allowed") is not False:
            failures.append(f"{label}.integration: local temp evidence must stay false")
        fixture_surface_kinds = {
            str(kind) for kind in _as_list(envelope.get("required_surface_kinds"))
        }
        if fixture_surface_kinds != REQUIRED_INTEGRATION_SURFACE_KINDS:
            failures.append(f"{label}.integration: fixture surface kinds drifted")
        reserved_rows = [
            row
            for row in _as_list(integration_contract.get("reserved_fail_closed_claims"))
            if isinstance(row, dict)
        ]
        reserved_claim_ids = {str(row.get("claim_id", "")) for row in reserved_rows}
        if reserved_claim_ids != REQUIRED_INTEGRATION_RESERVED_CLAIMS:
            failures.append(f"{label}.integration: fixture reserved claims drifted")
        alignment = integration_contract.get("negative_matrix_alignment")
        if not isinstance(alignment, dict):
            failures.append(f"{label}.integration: negative matrix alignment missing")
            alignment = {}
        aligned_case_ids = {
            str(case_id)
            for case_id in _as_list(alignment.get("required_case_ids"))
        }
        if aligned_case_ids != REQUIRED_INTEGRATION_NEGATIVE_CASE_IDS:
            failures.append(f"{label}.integration: aligned negative case ids drifted")

    negative_fixtures = [
        fixture
        for fixture in _as_list(contract.get("negative_fixtures"))
        if isinstance(fixture, dict)
    ]
    negative_fixture_paths = {
        _normalized(str(fixture.get("fixture", ""))) for fixture in negative_fixtures
    }
    for fixture in negative_fixture_paths:
        _paths_exist([fixture], failures, f"{label}.negative_fixtures")

    scheduler = surfaces_by_kind.get("scheduler-task-runtime-contract", {})
    scheduler_guarantees = scheduler.get("scheduler_task_runtime_guarantees")
    if not isinstance(scheduler_guarantees, dict):
        failures.append(f"{label}.scheduler: scheduler_task_runtime_guarantees missing")
        scheduler_guarantees = {}
    scheduler_contract_path = _normalized(
        str(scheduler_guarantees.get("contract_fixture", ""))
    )
    _paths_exist([scheduler_contract_path], failures, f"{label}.scheduler")
    if scheduler_contract_path:
        scheduler_contract = _load_json(ROOT / scheduler_contract_path)
        if scheduler_contract.get("contract_id") != (
            "objc3c.advanced-runtime.scheduler-task-runtime-guarantees.v1"
        ):
            failures.append(f"{label}.scheduler: contract fixture id drifted")
        if int(scheduler_contract.get("issue_ref", 0)) != 8214:
            failures.append(f"{label}.scheduler: contract fixture issue drifted")
    expected_scheduler_import_fields = {
        "task_runtime_ready",
        "deterministic",
        "shutdown_drain_ready",
        "cancellation_error_cleanup_ready",
        "replay_key",
        "task_lifecycle_replay_key",
        "task_cancellation_replay_key",
        "shutdown_replay_key",
        "task_record_sites",
        "continuation_record_sites",
        "executor_hop_record_sites",
        "queue_lifecycle_record_sites",
        "cancellation_checkpoint_sites",
        "error_cleanup_sites",
        "shutdown_drain_sites",
        "unsupported_policy_sites",
    }
    scheduler_import_fields = {
        str(field)
        for field in _as_list(scheduler_guarantees.get("required_import_surface_fields"))
    }
    if not expected_scheduler_import_fields <= scheduler_import_fields:
        failures.append(f"{label}.scheduler: required import-surface fields missing")
    scheduler_case_ids = {
        str(case_id)
        for case_id in _as_list(scheduler_guarantees.get("required_negative_case_ids"))
    }
    expected_scheduler_cases = {
        "scheduler_priority_fairness_overclaim",
        "scheduler_task_lifecycle_missing_replay",
        "scheduler_shutdown_drain_missing_evidence",
        "scheduler_cancellation_error_cleanup_missing_evidence",
    }
    if scheduler_case_ids != expected_scheduler_cases:
        failures.append(f"{label}.scheduler: negative case ids drifted")

    foreign = surfaces_by_kind.get("foreign-abi-runtime-contract", {})
    foreign_closure = foreign.get("foreign_abi_runtime_closure")
    if not isinstance(foreign_closure, dict):
        failures.append(f"{label}.foreign_abi: foreign_abi_runtime_closure missing")
        foreign_closure = {}
    foreign_contract_path = _normalized(str(foreign_closure.get("contract_fixture", "")))
    _paths_exist([foreign_contract_path], failures, f"{label}.foreign_abi")
    if foreign_contract_path:
        foreign_contract = _load_json(ROOT / foreign_contract_path)
        if foreign_contract.get("contract_id") != (
            "objc3c.advanced-runtime.foreign-abi-runtime-closure.v1"
        ):
            failures.append(f"{label}.foreign_abi: contract fixture id drifted")
        if int(foreign_contract.get("issue_ref", 0)) != 8215:
            failures.append(f"{label}.foreign_abi: contract fixture issue drifted")
    expected_foreign_import_fields = {
        "package_identity",
        "runtime_identity",
        "runtime_closure_ready",
        "deterministic",
        "typed_dispatch_ready",
        "package_runtime_identity_ready",
        "bridge_ownership_ready",
        "replay_key",
        "classification_replay_key",
        "bridge_metadata_replay_key",
        "foreign_surface_count",
        "supported_c_abi_surface_count",
        "preserved_swift_metadata_surface_count",
        "preserved_cpp_metadata_surface_count",
        "rejected_surface_count",
        "abi_mismatch_negative_case_count",
        "missing_bridge_ownership_negative_case_count",
        "unsafe_mixed_image_negative_case_count",
        "stale_import_negative_case_count",
        "unsupported_runtime_fallback_negative_case_count",
    }
    foreign_import_fields = {
        str(field)
        for field in _as_list(foreign_closure.get("required_import_surface_fields"))
    }
    if not expected_foreign_import_fields <= foreign_import_fields:
        failures.append(f"{label}.foreign_abi: required import-surface fields missing")
    foreign_case_ids = {
        str(case_id)
        for case_id in _as_list(foreign_closure.get("required_negative_case_ids"))
    }
    expected_foreign_cases = {
        "foreign_abi_mismatch",
        "foreign_abi_missing_bridge_ownership",
        "foreign_abi_unsafe_mixed_image",
        "foreign_abi_stale_import",
        "foreign_abi_unsupported_runtime_fallback",
    }
    if foreign_case_ids != expected_foreign_cases:
        failures.append(f"{label}.foreign_abi: negative case ids drifted")

    actor = surfaces_by_kind.get("actor-mailbox-runtime-expansion-contract", {})
    actor_expansion = actor.get("actor_mailbox_runtime_expansion")
    if not isinstance(actor_expansion, dict):
        failures.append(
            f"{label}.actor_mailbox: actor_mailbox_runtime_expansion missing"
        )
        actor_expansion = {}
    actor_contract_path = _normalized(
        str(actor_expansion.get("contract_fixture", ""))
    )
    _paths_exist([actor_contract_path], failures, f"{label}.actor_mailbox")
    if actor_contract_path:
        actor_contract = _load_json(ROOT / actor_contract_path)
        if actor_contract.get("contract_id") != (
            "objc3c.advanced-runtime.actor-mailbox-runtime-expansion.v1"
        ):
            failures.append(f"{label}.actor_mailbox: contract fixture id drifted")
        if int(actor_contract.get("issue_ref", 0)) != 8216:
            failures.append(f"{label}.actor_mailbox: contract fixture issue drifted")
        if int(actor_contract.get("umbrella_issue_ref", 0)) != 8203:
            failures.append(
                f"{label}.actor_mailbox: umbrella issue alignment drifted"
            )
    actor_runtime_contract = str(actor.get("runtime_contract", ""))
    for required_runtime_symbol in (
        "objc3_runtime_actor_mailbox_enqueue_i32",
        "objc3_runtime_actor_mailbox_drain_next_i32",
        "objc3_runtime_actor_mailbox_cancel_i32",
        "objc3_runtime_actor_mailbox_record_error_i32",
        "objc3_runtime_actor_mailbox_shutdown_i32",
    ):
        if required_runtime_symbol not in actor_runtime_contract:
            failures.append(
                f"{label}.actor_mailbox: runtime contract missing "
                f"{required_runtime_symbol}"
            )
    expected_actor_import_fields = {
        "actor_mailbox_runtime_ready",
        "deterministic",
        "replay_key",
        "actor_lowering_replay_key",
        "actor_isolation_lowering_replay_key",
        "actor_mailbox_message_identity_field_count",
        "actor_mailbox_fifo_ordering_field_count",
        "actor_mailbox_drain_operation_field_count",
        "actor_mailbox_cancel_operation_field_count",
        "actor_mailbox_error_operation_field_count",
        "actor_mailbox_shutdown_operation_field_count",
        "distributed_actor_transport_evidence_sites",
    }
    actor_import_fields = {
        str(field)
        for field in _as_list(actor_expansion.get("required_import_surface_fields"))
    }
    if not expected_actor_import_fields <= actor_import_fields:
        failures.append(
            f"{label}.actor_mailbox: required import-surface fields missing"
        )
    actor_case_ids = {
        str(case_id)
        for case_id in _as_list(actor_expansion.get("required_negative_case_ids"))
    }
    expected_actor_cases = {
        "distributed_actor_missing_transport",
        "distributed_actor_transport_overclaim",
        "actor_mailbox_stale_identity",
        "actor_mailbox_cancelled_queue",
        "actor_mailbox_error_without_replay",
    }
    if actor_case_ids != expected_actor_cases:
        failures.append(f"{label}.actor_mailbox: negative case ids drifted")

    macro = surfaces_by_kind.get("macro-package-replay-runtime-contract", {})
    macro_closure = macro.get("macro_package_replay_runtime_closure")
    if not isinstance(macro_closure, dict):
        failures.append(
            f"{label}.macro_package_replay: macro_package_replay_runtime_closure missing"
        )
        macro_closure = {}
    macro_contract_path = _normalized(
        str(macro_closure.get("contract_fixture", ""))
    )
    _paths_exist([macro_contract_path], failures, f"{label}.macro_package_replay")
    if macro_contract_path:
        macro_contract = _load_json(ROOT / macro_contract_path)
        if macro_contract.get("contract_id") != (
            "objc3c.advanced-runtime.macro-package-replay-runtime-closure.v1"
        ):
            failures.append(
                f"{label}.macro_package_replay: contract fixture id drifted"
            )
        if int(macro_contract.get("issue_ref", 0)) != 8217:
            failures.append(
                f"{label}.macro_package_replay: contract fixture issue drifted"
            )
        if int(macro_contract.get("umbrella_issue_ref", 0)) != 8203:
            failures.append(
                f"{label}.macro_package_replay: umbrella issue alignment drifted"
            )
    expected_macro_import_fields = {
        "runtime_import_artifact_ready",
        "separate_compilation_ready",
        "deterministic",
        "replay_key",
        "host_executable_relative_path",
        "cache_root_relative_path",
        "macro_package_identity",
        "macro_package_lock_identity",
        "macro_package_trust_identity",
        "macro_input_content_identity",
        "macro_output_content_identity",
        "macro_host_identity",
        "cache_validation_status",
        "runtime_consumption_artifact_identity",
        "package_replay_generation",
    }
    macro_import_fields = {
        str(field)
        for field in _as_list(macro_closure.get("required_import_surface_fields"))
    }
    if not expected_macro_import_fields <= macro_import_fields:
        failures.append(
            f"{label}.macro_package_replay: required import-surface fields missing"
        )
    macro_case_ids = {
        str(case_id)
        for case_id in _as_list(macro_closure.get("required_negative_case_ids"))
    }
    expected_macro_cases = {
        "macro_host_arbitrary_execution",
        "macro_package_replay_stale_cache",
        "macro_package_replay_host_mismatch",
        "macro_package_replay_tampered_metadata",
        "macro_package_replay_missing_metadata",
    }
    if macro_case_ids != expected_macro_cases:
        failures.append(f"{label}.macro_package_replay: negative case ids drifted")

    return {
        "path": ADVANCED_CLOSURE_EXECUTABLE_CONTRACT,
        "surface_count": len(surfaces),
        "negative_fixture_count": len(negative_fixtures),
        "scheduler_contract": scheduler_contract_path,
        "foreign_abi_contract": foreign_contract_path,
        "actor_mailbox_contract": actor_contract_path,
        "macro_package_replay_contract": macro_contract_path,
        "integration_contract": integration_contract_path,
    }


def _parallelism_cap_env() -> dict[str, str]:
    env = os.environ.copy()
    env.update(NATIVE_PARALLELISM_CAP_ENV)
    return env


def _safe_clean_generated_attempt_dir(path: Path, failures: list[str], label: str) -> bool:
    expected_root = (ROOT / "tmp" / "artifacts" / "advanced-runtime-closure").resolve()
    resolved = path.resolve()
    if not str(resolved).startswith(str(expected_root)):
        failures.append(f"{label}: generated attempt dir is outside #8199 tmp root")
        return False
    if path.exists():
        shutil.rmtree(path)
    path.mkdir(parents=True, exist_ok=True)
    return True


def _diagnostic_signature(record: dict[str, Any]) -> dict[str, Any]:
    return {
        "code": str(record.get("code", "")),
        "line": int(record.get("line", 0)),
        "column": int(record.get("column", 0)),
        "message": str(record.get("message", "")),
    }


def _expected_native_diagnostic_signature(record: dict[str, Any]) -> dict[str, Any]:
    return {
        "code": str(record.get("code", "")),
        "line": int(record.get("line", 0)),
        "column": int(record.get("column", 0)),
        "message_contains": str(record.get("message_contains", "")),
    }


def _resolve_clangxx() -> str:
    configured = os.environ.get("OBJC3C_NATIVE_EXECUTION_CLANG_PATH")
    if configured:
        return configured
    candidate = find_llvm_tool_path("clang++")
    return str(candidate) if candidate else "clang++"


def _native_link_driver_args() -> list[str]:
    args = ["-std=c++20"]
    if os.name == "nt":
        args.extend(
            [
                "-fms-runtime-lib=dll",
                "-fuse-ld=lld",
                "-Xlinker",
                "/MANIFEST:EMBED",
                "-Xlinker",
                "/MANIFESTUAC:level='asInvoker' uiAccess='false'",
            ]
        )
    return args


def _write_phase_log(
    *,
    command: list[str],
    result: subprocess.CompletedProcess[str],
    log_path: Path,
) -> None:
    log_path.parent.mkdir(parents=True, exist_ok=True)
    log_path.write_text(
        "command: "
        + " ".join(command)
        + "\n"
        + f"exit_code: {result.returncode}\n"
        + "stdout:\n"
        + result.stdout
        + "\nstderr:\n"
        + result.stderr,
        encoding="utf-8",
    )


def _run_native_phase(
    command: list[str],
    *,
    cwd: Path,
    log_path: Path,
    env: dict[str, str],
) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        command,
        cwd=str(cwd),
        env=env,
        text=True,
        capture_output=True,
        check=False,
    )
    _write_phase_log(command=command, result=result, log_path=log_path)
    return result


def _runtime_launch_inputs(
    compile_dir: Path,
    failures: list[str],
    *,
    label: str,
) -> tuple[Path | None, list[str]]:
    registration_manifest_path = compile_dir / "module.runtime-registration-manifest.json"
    main_manifest_path = compile_dir / "module.manifest.json"
    if not registration_manifest_path.is_file():
        failures.append(f"{label}: runtime registration manifest missing")
        return None, []
    if not main_manifest_path.is_file():
        failures.append(f"{label}: compile manifest missing")
        return None, []

    registration_manifest = _load_json(registration_manifest_path)
    main_manifest = _load_json(main_manifest_path)
    if registration_manifest.get("launch_integration_ready") is not True:
        failures.append(f"{label}: launch_integration_ready must be true")

    archive_rel = _normalized(
        str(registration_manifest.get("runtime_support_library_archive_relative_path", ""))
    )
    runtime_library = ROOT / archive_rel if archive_rel else None
    if runtime_library is None or not runtime_library.is_file():
        failures.append(f"{label}: runtime support library missing")

    raw_driver_flags = registration_manifest.get("driver_linker_flags")
    if not isinstance(raw_driver_flags, list) or not raw_driver_flags:
        failures.append(f"{label}: driver_linker_flags must be non-empty")
        driver_flags: list[str] = []
    else:
        driver_flags = [
            str(flag)
            for flag in raw_driver_flags
            if isinstance(flag, str) and flag
        ]
        if len(driver_flags) != len(raw_driver_flags):
            failures.append(f"{label}: driver_linker_flags must be strings")

    semantic_surface = (
        main_manifest.get("frontend", {})
        .get("pipeline", {})
        .get("semantic_surface", {})
        .get("objc_runtime_translation_unit_registration_manifest", {})
    )
    if semantic_surface.get("manifest_artifact_relative_path") != (
        "module.runtime-registration-manifest.json"
    ):
        failures.append(f"{label}: compile manifest is not bound to runtime registration")
    return runtime_library, driver_flags


def _native_phase_statuses(
    *,
    compile_status: str,
    link_claimed: bool,
    run_claimed: bool,
    generated_dir: Path,
    link_succeeded: bool | None = None,
    run_succeeded: bool | None = None,
) -> dict[str, str]:
    if not link_claimed:
        link_status = "native_link_unclaimed"
    elif link_succeeded is True:
        link_status = "native_link_succeeded"
    else:
        link_status = "native_link_failed"
    execution_status = (
        "native_execution_unclaimed"
        if not run_claimed
        else "native_execution_succeeded"
        if run_succeeded is True
        else "native_execution_failed"
    )
    statuses = {
        "compile": compile_status,
        "link": link_status,
        "runtime_registration": "runtime_registration_artifact_missing",
        "runtime_metadata": "runtime_metadata_artifact_missing",
        "error_replay": "error_replay_artifact_missing",
        "execution_status": execution_status,
    }
    for artifact_name, phase in NATIVE_ARTIFACT_PHASES.items():
        if (generated_dir / artifact_name).is_file():
            statuses[phase] = str(NATIVE_PHASE_STATUS_VALUES[phase][0])
    return statuses


def _validate_native_phase_status_contract(
    contract: dict[str, Any],
    failures: list[str],
    *,
    label: str,
) -> dict[str, Any]:
    reporting = contract.get("typed_failure_reporting")
    if not isinstance(reporting, dict):
        failures.append(f"{label}: typed_failure_reporting is required")
        return {}
    if reporting.get("contract_id") != NATIVE_PHASE_STATUS_CONTRACT_ID:
        failures.append(f"{label}: typed failure reporting contract_id drifted")
    if int(reporting.get("issue_ref", 0)) != 8213:
        failures.append(f"{label}: typed failure reporting issue_ref must be #8213")
    if tuple(_as_list(reporting.get("phase_order"))) != NATIVE_PHASE_STATUS_ORDER:
        failures.append(f"{label}: typed failure reporting phase_order drifted")
    phase_statuses = reporting.get("phase_statuses")
    if not isinstance(phase_statuses, dict):
        failures.append(f"{label}: typed failure reporting phase_statuses is required")
        return reporting
    expected_phases = set(NATIVE_PHASE_STATUS_ORDER)
    actual_phases = {str(phase) for phase in phase_statuses}
    if actual_phases != expected_phases:
        failures.append(
            f"{label}: typed failure reporting phases drifted: "
            f"expected {sorted(expected_phases)}, got {sorted(actual_phases)}"
        )
    for phase, expected_values in NATIVE_PHASE_STATUS_VALUES.items():
        phase_payload = phase_statuses.get(phase)
        if not isinstance(phase_payload, dict):
            failures.append(f"{label}: typed failure reporting phase {phase} is required")
            continue
        values = tuple(str(value) for value in _as_list(phase_payload.get("values")))
        if values != expected_values:
            failures.append(
                f"{label}: typed failure reporting values drifted for {phase}"
            )
        failure_value = str(phase_payload.get("failure_value", ""))
        if failure_value not in expected_values:
            failures.append(
                f"{label}: typed failure reporting failure_value drifted for {phase}"
            )
    return reporting


def _validate_native_artifact_contract(
    failures: list[str],
) -> dict[str, Any]:
    contract_path = ROOT / ADVANCED_CLOSURE_NATIVE_ARTIFACT_CONTRACT
    contract = _load_json(contract_path)
    label = "advanced_runtime_closure.native_artifact"
    if contract.get("contract_id") != ADVANCED_CLOSURE_NATIVE_ARTIFACT_CONTRACT_ID:
        failures.append(f"{label}: contract_id drifted")
    if int(contract.get("issue_ref", 0)) != 8199:
        failures.append(f"{label}: issue_ref must be #8199")
    if int(contract.get("followup_issue_ref", 0)) != 8213:
        failures.append(f"{label}: followup_issue_ref must be #8213")
    if contract.get("status") != "native_link_run_ready":
        failures.append(f"{label}: status must stay native_link_run_ready")
    if contract.get("positive_fixture") != ADVANCED_CLOSURE_POSITIVE_FIXTURE:
        failures.append(f"{label}: positive_fixture drifted")
    if contract.get("provider_fixture") != ADVANCED_CLOSURE_PROVIDER_FIXTURE:
        failures.append(f"{label}: provider_fixture drifted")
    if contract.get("generated_attempt_dir") != ADVANCED_CLOSURE_NATIVE_ATTEMPT_DIR:
        failures.append(f"{label}: generated_attempt_dir drifted")
    if contract.get("expected_compile_backend") != DIRECT_COMPILE_BACKEND:
        failures.append(f"{label}: expected_compile_backend drifted")
    expected_exit_code = int(contract.get("expected_exit_code", -1))
    if expected_exit_code != 0:
        failures.append(f"{label}: expected_exit_code must remain 0")
    if contract.get("runtime_execution_entrypoint") != REQUIRED_RUNTIME_EXECUTION_ENTRYPOINT:
        failures.append(f"{label}: runtime_execution_entrypoint drifted")
    runtime_observations = {
        str(item) for item in _as_list(contract.get("runtime_execution_observations"))
    }
    if runtime_observations != REQUIRED_RUNTIME_EXECUTION_OBSERVATIONS:
        failures.append(f"{label}: runtime_execution_observations drifted")
    true_claim_fields = (
        "native_compile_claimed",
        "native_object_artifact_claimed",
        "native_ir_artifact_claimed",
        "native_manifest_artifact_claimed",
        "native_link_claimed",
        "native_run_claimed",
        "native_executable_umbrella_promoted",
        "umbrella_support_promoted",
    )
    for field in true_claim_fields:
        if contract.get(field) is not True:
            failures.append(f"{label}: {field} must remain true")

    expected_diagnostics = [
        _expected_native_diagnostic_signature(record)
        for record in _as_list(contract.get("expected_diagnostics"))
        if isinstance(record, dict)
    ]
    if expected_diagnostics:
        failures.append(f"{label}: expected_diagnostics must stay empty")
    absent_codes = {str(code) for code in _as_list(contract.get("absent_diagnostic_codes"))}
    if absent_codes:
        failures.append(f"{label}: absent_diagnostic_codes must stay empty")
    required_artifacts = tuple(
        str(path) for path in _as_list(contract.get("required_success_artifacts"))
    )
    if required_artifacts != REQUIRED_NATIVE_ARTIFACTS:
        failures.append(f"{label}: required_success_artifacts drifted")
    forbidden_artifacts = tuple(
        str(path) for path in _as_list(contract.get("forbidden_success_artifacts"))
    )
    if forbidden_artifacts != FORBIDDEN_NATIVE_SUCCESS_ARTIFACTS:
        failures.append(f"{label}: forbidden_success_artifacts drifted")
    forbidden_ir_markers = tuple(
        str(marker) for marker in _as_list(contract.get("forbidden_llvm_operand_markers"))
    )
    if forbidden_ir_markers != FORBIDDEN_NATIVE_LLVM_OPERAND_MARKERS:
        failures.append(f"{label}: forbidden LLVM operand marker guard drifted")
    phase_status_contract = _validate_native_phase_status_contract(
        contract,
        failures,
        label=label,
    )

    generated_dir = ROOT / ADVANCED_CLOSURE_NATIVE_ATTEMPT_DIR
    if not _safe_clean_generated_attempt_dir(generated_dir, failures, label):
        return {
            "path": ADVANCED_CLOSURE_NATIVE_ARTIFACT_CONTRACT,
            "status": "not_run",
            "compile_exit_code": None,
            "diagnostic_count": 0,
            "diagnostic_codes": [],
            "diagnostics_path": "",
            "phase_status_contract": phase_status_contract.get("contract_id"),
            "phase_statuses": _native_phase_statuses(
                compile_status="native_compile_launch_failed",
                link_claimed=contract.get("native_link_claimed") is True,
                run_claimed=contract.get("native_run_claimed") is True,
                generated_dir=generated_dir,
            ),
            "generated_attempt_dir": ADVANCED_CLOSURE_NATIVE_ATTEMPT_DIR,
            "native_artifact_ready": False,
            "native_executable_umbrella_promoted": False,
        }

    provider_dir = generated_dir / "provider"
    provider_dir.mkdir(parents=True, exist_ok=True)
    provider_command, provider_backend = compile_command(
        ROOT / ADVANCED_CLOSURE_PROVIDER_FIXTURE,
        provider_dir,
        extra_args=["--objc3-bootstrap-registration-order-ordinal", "1"],
        backend=DIRECT_COMPILE_BACKEND,
    )
    command, selected_backend = compile_command(
        ROOT / ADVANCED_CLOSURE_POSITIVE_FIXTURE,
        generated_dir,
        extra_args=[
            "--objc3-import-runtime-surface",
            str(provider_dir / "module.runtime-import-surface.json"),
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
        ],
        backend=DIRECT_COMPILE_BACKEND,
    )
    env = _parallelism_cap_env()
    try:
        provider_result = _run_native_phase(
            provider_command,
            cwd=ROOT,
            log_path=generated_dir / "provider.compile.log",
            env=env,
        )
        result = _run_native_phase(
            command,
            cwd=ROOT,
            log_path=generated_dir / "compile.log",
            env=env,
        )
    except OSError as exc:
        failures.append(f"{label}: unable to launch native compiler: {exc}")
        return {
            "path": ADVANCED_CLOSURE_NATIVE_ARTIFACT_CONTRACT,
            "status": "launch_failed",
            "compile_exit_code": None,
            "diagnostic_count": 0,
            "diagnostic_codes": [],
            "diagnostics_path": "",
            "phase_status_contract": phase_status_contract.get("contract_id"),
            "phase_statuses": _native_phase_statuses(
                compile_status="native_compile_launch_failed",
                link_claimed=contract.get("native_link_claimed") is True,
                run_claimed=contract.get("native_run_claimed") is True,
                generated_dir=generated_dir,
                link_succeeded=False,
                run_succeeded=False,
            ),
            "generated_attempt_dir": ADVANCED_CLOSURE_NATIVE_ATTEMPT_DIR,
            "native_artifact_ready": False,
            "native_executable_umbrella_promoted": False,
        }

    if provider_backend != DIRECT_COMPILE_BACKEND:
        failures.append(f"{label}: provider native attempt did not use direct-native backend")
    if selected_backend != DIRECT_COMPILE_BACKEND:
        failures.append(f"{label}: native attempt did not use direct-native backend")
    if provider_result.returncode != 0:
        failures.append(
            f"{label}: expected provider native compile exit 0, got {provider_result.returncode}"
        )
    if result.returncode != expected_exit_code:
        failures.append(
            f"{label}: expected native compile exit {expected_exit_code}, "
            f"got {result.returncode}"
        )

    provider_obj = provider_dir / "module.obj"
    provider_import_surface = provider_dir / "module.runtime-import-surface.json"
    if not provider_obj.is_file():
        failures.append(f"{label}: provider native object missing")
    if not provider_import_surface.is_file():
        failures.append(f"{label}: provider runtime import surface missing")
    runtime_library, driver_flags = _runtime_launch_inputs(
        generated_dir,
        failures,
        label=f"{label}.main",
    )
    provider_runtime_library, provider_driver_flags = _runtime_launch_inputs(
        provider_dir,
        failures,
        label=f"{label}.provider",
    )
    if (
        runtime_library is not None
        and provider_runtime_library is not None
        and runtime_library != provider_runtime_library
    ):
        failures.append(f"{label}: provider and main resolved different runtime libraries")

    exe_path = generated_dir / "module.exe"
    link_result: subprocess.CompletedProcess[str] | None = None
    run_result: subprocess.CompletedProcess[str] | None = None
    link_succeeded = False
    run_succeeded = False
    if (
        provider_result.returncode == 0
        and result.returncode == expected_exit_code
        and runtime_library is not None
        and provider_obj.is_file()
    ):
        link_command = [
            _resolve_clangxx(),
            *_native_link_driver_args(),
            str(generated_dir / "module.obj"),
            str(provider_obj),
            str(runtime_library),
            *driver_flags,
            *provider_driver_flags,
            "-o",
            str(exe_path),
            "-fno-color-diagnostics",
        ]
        link_result = _run_native_phase(
            link_command,
            cwd=ROOT,
            log_path=generated_dir / "link.log",
            env=env,
        )
        link_succeeded = link_result.returncode == 0 and exe_path.is_file()
        if not link_succeeded:
            failures.append(
                f"{label}: native executable link failed with exit {link_result.returncode}"
            )
    else:
        failures.append(f"{label}: skipping native link/run because compile artifacts are incomplete")

    if link_succeeded:
        run_result = _run_native_phase(
            [str(exe_path)],
            cwd=ROOT,
            log_path=generated_dir / "run.log",
            env=env,
        )
        run_succeeded = run_result.returncode == expected_exit_code
        if not run_succeeded:
            failures.append(
                f"{label}: native executable run expected exit {expected_exit_code}, "
                f"got {run_result.returncode}"
            )

    for artifact_name in required_artifacts:
        if not (generated_dir / artifact_name).is_file():
            failures.append(f"{label}: required native artifact missing: {artifact_name}")
    for artifact_name in forbidden_artifacts:
        if (generated_dir / artifact_name).exists():
            failures.append(f"{label}: forbidden success artifact exists: {artifact_name}")
    llvm_ir_path = generated_dir / "module.ll"
    if llvm_ir_path.is_file():
        llvm_ir = llvm_ir_path.read_text(encoding="utf-8")
        for marker in forbidden_ir_markers:
            if marker in llvm_ir:
                failures.append(
                    f"{label}: forbidden LLVM operand marker {marker!r} exists in module.ll"
                )

    diagnostics_path = generated_dir / "module.diagnostics.json"
    if not diagnostics_path.is_file():
        failures.append(f"{label}: native compile did not emit module.diagnostics.json")
        diagnostics: list[dict[str, Any]] = []
    else:
        diagnostics_payload = _load_json(diagnostics_path)
        diagnostics = [
            diagnostic
            for diagnostic in _as_list(diagnostics_payload.get("diagnostics"))
            if isinstance(diagnostic, dict)
        ]

    actual_signatures = [_diagnostic_signature(diagnostic) for diagnostic in diagnostics]
    if actual_signatures:
        failures.append(f"{label}: native compile emitted diagnostics")

    actual_codes = {str(signature["code"]) for signature in actual_signatures}
    phase_statuses = _native_phase_statuses(
        compile_status=(
            "native_compile_succeeded"
            if provider_result.returncode == 0 and result.returncode == 0
            else "native_compile_failed"
        ),
        link_claimed=contract.get("native_link_claimed") is True,
        run_claimed=contract.get("native_run_claimed") is True,
        generated_dir=generated_dir,
        link_succeeded=link_succeeded,
        run_succeeded=run_succeeded,
    )

    status = (
        "native_link_run_ready"
        if run_succeeded
        else "native_link_run_failed"
        if provider_result.returncode == 0 and result.returncode == 0
        else "compile_failed"
    )
    return {
        "path": ADVANCED_CLOSURE_NATIVE_ARTIFACT_CONTRACT,
        "status": status,
        "compile_exit_code": result.returncode,
        "provider_compile_exit_code": provider_result.returncode,
        "link_exit_code": link_result.returncode if link_result is not None else None,
        "run_exit_code": run_result.returncode if run_result is not None else None,
        "diagnostic_count": len(actual_signatures),
        "diagnostic_codes": sorted(actual_codes),
        "diagnostics_path": _repo_rel(diagnostics_path) if diagnostics_path.exists() else "",
        "phase_status_contract": phase_status_contract.get("contract_id"),
        "phase_statuses": phase_statuses,
        "generated_attempt_dir": ADVANCED_CLOSURE_NATIVE_ATTEMPT_DIR,
        "native_artifact_ready": provider_result.returncode == 0
        and result.returncode == 0,
        "native_executable_umbrella_promoted": run_succeeded,
        "native_executable_path": _repo_rel(exe_path) if exe_path.exists() else "",
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
    if contract.get("umbrella_support_promoted") is not True:
        failures.append(f"{label}: umbrella support must be promoted after #8213")
    if contract.get("native_artifact_contract") != ADVANCED_CLOSURE_NATIVE_ARTIFACT_CONTRACT:
        failures.append(f"{label}: native_artifact_contract drifted")
    if contract.get("native_artifact_ready") is not True:
        failures.append(f"{label}: native_artifact_ready must remain true")
    if contract.get("native_executable_umbrella_promoted") is not True:
        failures.append(f"{label}: native executable umbrella must be promoted after #8213")
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
    _validate_promotion_boundary(
        contract.get("promotion_boundary"),
        failures,
        label=f"{label}.promotion_boundary",
    )

    _paths_exist(
        [
            str(contract.get("language_semantics_contract", "")),
            str(contract.get("debug_source_map_validator", "")),
            str(contract.get("canonical_compiler_emitted_source_debug_map_bundle", "")),
            str(contract.get("native_artifact_contract", "")),
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
        if row.get("state") != "implemented":
            failures.append("language.advanced-runtime-closure must be implemented in capability matrix")
        support_claims = {
            str(claim) for claim in _as_list(row.get("support_claims"))
        }
        if "objc3c.behavior.language.advanced-runtime-closure" not in support_claims:
            failures.append("language.advanced-runtime-closure support claim missing")
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
    non_claiming_rows = [
        str(row.get("path"))
        for row in evidence_rows
        if row.get("support_claim")
        != "objc3c.behavior.language.advanced-runtime-closure"
    ]
    if non_claiming_rows:
        failures.append(
            "language.advanced-runtime-closure evidence-map rows must use the umbrella support claim: "
            f"{non_claiming_rows}"
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
    header = LANGUAGE_SEMANTICS_HEADER_PATH.read_text(encoding="utf-8")
    implementation = LANGUAGE_SEMANTICS_IMPLEMENTATION_PATH.read_text(encoding="utf-8")
    if contract.get("abi_version") != 5:
        failures.append("language semantics runtime API contract abi_version must be 5")
    if "OBJC3_RUNTIME_LANGUAGE_SEMANTICS_ABI_VERSION 5u" not in header:
        failures.append("language semantics runtime API header must publish ABI version 5")
    native_snapshot_fields = {
        str(field)
        for field in _as_list(contract.get("advanced_runtime_native_snapshot_fields"))
    }
    if native_snapshot_fields != REQUIRED_ADVANCED_RUNTIME_NATIVE_SNAPSHOT_FIELDS:
        failures.append(
            "language semantics runtime API contract native snapshot fields drifted"
        )
    for field in sorted(REQUIRED_ADVANCED_RUNTIME_NATIVE_SNAPSHOT_FIELDS):
        if field not in header:
            failures.append(
                f"language semantics runtime API header missing native snapshot field {field}"
            )
        if field not in implementation:
            failures.append(
                f"language semantics runtime API implementation missing native snapshot field {field}"
            )
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
        "native_artifact_contract": ADVANCED_CLOSURE_NATIVE_ARTIFACT_CONTRACT,
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
        "umbrella_closure_support": True,
        "canonical_source_debug_map_evidence": True,
        "native_artifact_evidence": True,
        "native_executable_umbrella_support": True,
    }
    for key, expected_value in expected_bool_fields.items():
        if row.get(key) is not expected_value:
            failures.append(
                f"#8199 language semantics row {key} must be {expected_value}"
            )
    expected_int_fields = {
        "combined_runtime_state_record_count": 10,
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
            str(row.get("native_artifact_contract", "")),
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
    executable_contract = _validate_executable_runtime_contract(failures)
    combined_identity = _validate_combined_identity_contract(failures)
    native_artifact = _validate_native_artifact_contract(failures)
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
        "advanced_runtime_runtime_execution_entrypoint": positive_fixture.get(
            "runtime_execution_entrypoint"
        ),
        "advanced_runtime_runtime_execution_observations": positive_fixture.get(
            "runtime_execution_observations"
        ),
        "advanced_runtime_negative_matrix": negative_matrix.get("path"),
        "advanced_runtime_negative_matrix_case_count": negative_matrix.get(
            "case_count"
        ),
        "advanced_runtime_negative_matrix_interaction_count": negative_matrix.get(
            "interaction_count"
        ),
        "advanced_runtime_executable_contract": executable_contract.get("path"),
        "advanced_runtime_executable_contract_surface_count": executable_contract.get(
            "surface_count"
        ),
        "advanced_runtime_executable_contract_negative_fixture_count": executable_contract.get(
            "negative_fixture_count"
        ),
        "advanced_runtime_scheduler_task_contract": executable_contract.get(
            "scheduler_contract"
        ),
        "advanced_runtime_foreign_abi_contract": executable_contract.get(
            "foreign_abi_contract"
        ),
        "advanced_runtime_integration_contract": executable_contract.get(
            "integration_contract"
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
        "advanced_runtime_native_artifact_contract": native_artifact.get(
            "path"
        ),
        "advanced_runtime_native_compile_attempt_status": native_artifact.get(
            "status"
        ),
        "advanced_runtime_native_compile_attempt_exit_code": native_artifact.get(
            "compile_exit_code"
        ),
        "advanced_runtime_native_compile_attempt_diagnostic_count": native_artifact.get(
            "diagnostic_count"
        ),
        "advanced_runtime_native_compile_attempt_diagnostic_codes": native_artifact.get(
            "diagnostic_codes"
        ),
        "advanced_runtime_native_compile_attempt_diagnostics_path": native_artifact.get(
            "diagnostics_path"
        ),
        "advanced_runtime_native_phase_status_contract": native_artifact.get(
            "phase_status_contract"
        ),
        "advanced_runtime_native_phase_statuses": native_artifact.get(
            "phase_statuses"
        ),
        "advanced_runtime_native_artifact_ready": native_artifact.get(
            "native_artifact_ready"
        ),
        "advanced_runtime_native_executable_umbrella_promoted": native_artifact.get(
            "native_executable_umbrella_promoted"
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
