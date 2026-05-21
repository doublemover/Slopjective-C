#!/usr/bin/env python3
"""Validate the next runtime public capability-row split for #8154/#8155."""

from __future__ import annotations

from dataclasses import dataclass
import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
MATRIX_PATH = ROOT / "docs" / "support" / "capability_matrix.json"
EVIDENCE_MAP_PATH = ROOT / "docs" / "support" / "evidence_map.json"
MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
CATALOG_PATH = ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"
PUBLIC_COMMAND_PREFIX = "npm run objc3c -- "


class NextRuntimePublicRowsError(RuntimeError):
    """Raised when the checked capability-row split drifts."""


@dataclass(frozen=True)
class CapabilityRowExpectation:
    issue: int
    capability_id: str
    support_claim: str
    owner_phase: str
    behavior_fixture: str
    manifest_command: str
    runnable_command: str
    required_positive: tuple[str, ...]
    required_negative: tuple[str, ...]
    required_diagnostic_codes: tuple[str, ...]


OBJECT_MODEL_EXPECTATIONS: tuple[CapabilityRowExpectation, ...] = (
    CapabilityRowExpectation(
        issue=8154,
        capability_id="runtime.object-model.interface-method-table",
        support_claim="objc3c.behavior.runtime.object-model-interface-method-table",
        owner_phase="runtime",
        behavior_fixture="tests/native/runtime/object_model/interface_method_table_contract.objc3",
        manifest_command="npm run objc3c -- test-behavior-matrix",
        runnable_command="npm run objc3c -- test-runtime-acceptance-fast",
        required_positive=(
            "scripts/objc3c_runtime_acceptance/domains/object_model_category_attachment_cases.py",
            "tests/tooling/runtime/category_attachment_protocol_runtime_probe.cpp",
            "tests/tooling/runtime/protocol_category_invalid_metadata_probe.cpp",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/execution/negative/category_attachment_collision.objc3",
            "tests/tooling/fixtures/native/execution/negative/duplicate_protocol_runtime_export.objc3",
        ),
        required_diagnostic_codes=("O3S200", "O3RT004"),
    ),
    CapabilityRowExpectation(
        issue=8154,
        capability_id="runtime.object-model.class-realization",
        support_claim="objc3c.behavior.runtime.object-model-class-realization",
        owner_phase="runtime",
        behavior_fixture="tests/native/runtime/object_model/class_realization_contract.objc3",
        manifest_command="npm run objc3c -- test-behavior-matrix",
        runnable_command="npm run objc3c -- validate-object-model-conformance",
        required_positive=(
            "tests/tooling/fixtures/native/class_realization_runtime_library.objc3",
            "tests/tooling/fixtures/native/metaclass_graph_root_class_library.objc3",
            "tests/tooling/runtime/class_realization_runtime_probe.cpp",
            "scripts/objc3c_runtime_acceptance/domains/object_model_surface_class_cases.py",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/execution/negative/class_metaclass_missing_superclass.objc3",
            "tests/tooling/fixtures/native/runtime_export_enforcement_duplicate_interface.objc3",
        ),
        required_diagnostic_codes=("O3S200", "O3S220"),
    ),
    CapabilityRowExpectation(
        issue=8154,
        capability_id="runtime.object-model.category-protocol-registration",
        support_claim="objc3c.behavior.runtime.object-model-category-protocol-registration",
        owner_phase="runtime",
        behavior_fixture="tests/native/runtime/object_model/category_protocol_registration_contract.objc3",
        manifest_command="npm run objc3c -- test-behavior-matrix",
        runnable_command="npm run objc3c -- test-runtime-acceptance-fast",
        required_positive=(
            "scripts/objc3c_runtime_acceptance/domains/object_model_category_attachment_cases.py",
            "tests/tooling/fixtures/native/category_attachment_protocol_runtime_library.objc3",
            "tests/tooling/runtime/category_attachment_protocol_runtime_probe.cpp",
            "tests/tooling/runtime/protocol_category_invalid_metadata_probe.cpp",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/execution/negative/category_unknown_class_rejected.objc3",
            "tests/tooling/fixtures/native/execution/negative/protocol_requirement_duplicate_conflict_rejected.objc3",
            "tests/tooling/fixtures/native/execution/negative/duplicate_protocol_runtime_export.objc3",
        ),
        required_diagnostic_codes=("O3S218", "O3S219", "O3RT004"),
    ),
    CapabilityRowExpectation(
        issue=8154,
        capability_id="runtime.object-model.property-ivar-reflection",
        support_claim="objc3c.behavior.runtime.object-model-property-ivar-reflection",
        owner_phase="runtime",
        behavior_fixture="tests/native/runtime/object_model/property_ivar_reflection_contract.objc3",
        manifest_command="npm run objc3c -- test-behavior-matrix",
        runnable_command="npm run objc3c -- validate-storage-reflection-conformance",
        required_positive=(
            "tests/native/runtime/storage/property_accessor_storage_contract.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3",
            "tests/tooling/runtime/property_ivar_invalid_layout_probe.cpp",
        ),
        required_diagnostic_codes=("O3S206", "O3RT004"),
    ),
    CapabilityRowExpectation(
        issue=8154,
        capability_id="runtime.object-model.registration-replay",
        support_claim="objc3c.behavior.runtime.object-model-registration-replay",
        owner_phase="runtime",
        behavior_fixture="tests/native/runtime/object_model/registration_replay_contract.objc3",
        manifest_command="npm run objc3c -- test-behavior-matrix",
        runnable_command="npm run objc3c -- validate-object-model-conformance",
        required_positive=(
            "tests/tooling/fixtures/native/execution/positive/registration_reset_replay_runtime_entrypoints.objc3",
            "scripts/objc3c_runtime_acceptance/domains/registration_replay_cases/assertions.py",
            "scripts/check_objc3c_runnable_object_model_conformance.py",
            "scripts/check_objc3c_runnable_object_model_end_to_end.py",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/execution/negative/module_duplicate_declaration.objc3",
            "tests/tooling/fixtures/native/missing_replay_proof_rejected.objc3",
        ),
        required_diagnostic_codes=("O3S200", "O3RT004"),
    ),
    CapabilityRowExpectation(
        issue=8154,
        capability_id="runtime.object-model.bounded-query-snapshots",
        support_claim="objc3c.behavior.runtime.object-model-bounded-query-snapshots",
        owner_phase="runtime",
        behavior_fixture="tests/native/runtime/object_model/bounded_query_snapshot_contract.objc3",
        manifest_command="npm run objc3c -- test-behavior-matrix",
        runnable_command="npm run objc3c -- validate-object-model-conformance",
        required_positive=(
            "tests/tooling/runtime/object_model_lookup_reflection_runtime_probe.cpp",
            "scripts/objc3c_runtime_acceptance/domains/object_model_surface_query_implementation.py",
            "scripts/objc3c_runtime_acceptance/domains/object_model_surface_query_abi.py",
            "scripts/objc3c_runtime_acceptance/domains/object_model_surface_query_reflection.py",
            "scripts/objc3c_runtime_acceptance/domains/object_model_surface_query_semantics.py",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/execution/negative/category_unknown_class_rejected.objc3",
            "tests/tooling/runtime/property_ivar_invalid_layout_probe.cpp",
        ),
        required_diagnostic_codes=("O3S206", "O3RT004"),
    ),
    CapabilityRowExpectation(
        issue=8154,
        capability_id="runtime.public-api.reflection",
        support_claim="objc3c.behavior.runtime.public-reflection-api",
        owner_phase="runtime",
        behavior_fixture="tests/tooling/fixtures/objc3c/public_runtime_reflection_api_contract.json",
        manifest_command="npm run objc3c -- validate-public-runtime-reflection-api",
        runnable_command="npm run objc3c -- validate-public-runtime-reflection-api",
        required_positive=(
            "native/objc3c/src/runtime/public/objc3_runtime_reflection.h",
            "native/objc3c/src/runtime/public/objc3_runtime_reflection.cpp",
            "tests/tooling/runtime/public_runtime_reflection_api_probe.cpp",
            "tests/tooling/test_public_runtime_reflection_api.py",
            "scripts/check_objc3c_public_runtime_reflection_api.py",
        ),
        required_negative=(
            "native/objc3c/src/runtime/public/objc3_runtime_reflection.cpp",
            "tests/tooling/test_public_runtime_reflection_api.py",
        ),
        required_diagnostic_codes=("O3RT002", "O3RT004"),
    ),
)


ADVANCED_RUNTIME_EXPECTATIONS: tuple[CapabilityRowExpectation, ...] = (
    CapabilityRowExpectation(
        issue=8155,
        capability_id="language.blocks.escape-capture-legality",
        support_claim="objc3c.behavior.language.blocks.escape-capture-legality",
        owner_phase="sema",
        behavior_fixture="tests/tooling/fixtures/native/capture_list_and_retainable_family_legality_completion_positive.objc3",
        manifest_command="npm run objc3c -- test-runtime-acceptance-block-arc",
        runnable_command="npm run objc3c -- test-runtime-acceptance-block-arc",
        required_positive=(
            "tests/tooling/fixtures/block_arc_closure/escaping_block_byref_ownership_semantic_model.json",
            "scripts/objc3c_runtime_acceptance/domains/block_arc_surface_ownership_transfer.py",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/weak_object_capture_mutation_negative.objc3",
            "tests/tooling/fixtures/native/execution/negative/escaping_owned_object_block_conflicting_capture.objc3",
        ),
        required_diagnostic_codes=("O3S301", "O3P313"),
    ),
    CapabilityRowExpectation(
        issue=8155,
        capability_id="runtime.blocks.copy-dispose-invoke",
        support_claim="objc3c.behavior.runtime.blocks.copy-dispose-invoke",
        owner_phase="runtime",
        behavior_fixture="tests/tooling/fixtures/native/execution/positive/escaping_owned_object_block_copy_dispose.objc3",
        manifest_command="npm run objc3c -- test-runtime-acceptance-block-arc",
        runnable_command="npm run objc3c -- test-runtime-acceptance-block-arc",
        required_positive=(
            "tests/tooling/fixtures/native/executable_block_object_invoke_thunk_positive.objc3",
            "tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp",
            "tests/tooling/runtime/block_runtime_owned_capture_lifetime_probe.cpp",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/execution/negative/escaping_owned_object_block_conflicting_capture.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_negative.objc3",
        ),
        required_diagnostic_codes=("O3S301",),
    ),
    CapabilityRowExpectation(
        issue=8155,
        capability_id="runtime.blocks.byref-forwarding",
        support_claim="objc3c.behavior.runtime.blocks.byref-forwarding",
        owner_phase="runtime",
        behavior_fixture="tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
        manifest_command="npm run objc3c -- test-runtime-acceptance-block-arc",
        runnable_command="npm run objc3c -- test-runtime-acceptance-block-arc",
        required_positive=(
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_byref_positive.objc3",
            "tests/tooling/fixtures/native/execution/positive/byref_capture_argument_materialization.objc3",
            "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/execution/negative/byref_capture_missing_identifier.objc3",
            "tests/tooling/fixtures/native/execution/negative/byref_capture_duplicate_binding.objc3",
        ),
        required_diagnostic_codes=("O3P313", "O3S301"),
    ),
    CapabilityRowExpectation(
        issue=8155,
        capability_id="language.arc-cleanup.integration",
        support_claim="objc3c.behavior.arc-cleanup.integration",
        owner_phase="runtime",
        behavior_fixture="tests/tooling/fixtures/native/arc_cleanup_source_construct_order_positive.objc3",
        manifest_command="npm run objc3c -- test-runtime-acceptance-arc-cleanup-integration",
        runnable_command="npm run objc3c -- test-runtime-acceptance-arc-cleanup-integration",
        required_positive=(
            "tests/tooling/fixtures/native/error_arc_cleanup_bridge_positive.objc3",
            "tests/tooling/fixtures/native/async_cleanup_integration_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
            "tests/tooling/fixtures/arc_cleanup_integration/owner_contract.json",
            "scripts/objc3c_runtime_acceptance/suite_catalog.py",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/execution/negative/escaping_owned_object_block_conflicting_capture.objc3",
            "tests/tooling/fixtures/native/throwing_call_requires_try_negative.objc3",
            "tests/tooling/fixtures/native/non_async_task_runtime_rejected.objc3",
        ),
        required_diagnostic_codes=("O3S275", "O3S301", "O3S343"),
    ),
    CapabilityRowExpectation(
        issue=8155,
        capability_id="runtime.errors.live-bridge-cleanup",
        support_claim="objc3c.behavior.runtime.error-live-bridge-cleanup",
        owner_phase="runtime",
        behavior_fixture="tests/tooling/fixtures/native/live_error_runtime_integration_positive.objc3",
        manifest_command="npm run objc3c -- validate-error-conformance",
        runnable_command="npm run objc3c -- validate-error-conformance",
        required_positive=(
            "tests/tooling/runtime/live_error_runtime_integration_probe.cpp",
            "tests/tooling/runtime/error_runtime_bridge_helper_probe.cpp",
            "tests/tooling/fixtures/error_runtime_closure/executable_proof_abi_contract.json",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/bridge_legality_nserror_missing_out_negative.objc3",
            "tests/tooling/fixtures/native/throwing_call_requires_try_negative.objc3",
        ),
        required_diagnostic_codes=("O3S275", "O3S277"),
    ),
    CapabilityRowExpectation(
        issue=8155,
        capability_id="runtime.concurrency.task-continuation-lifecycle",
        support_claim="objc3c.behavior.runtime.concurrency-task-continuation-lifecycle",
        owner_phase="runtime",
        behavior_fixture="tests/tooling/fixtures/native/live_continuation_runtime_integration_positive.objc3",
        manifest_command="npm run objc3c -- validate-concurrency-conformance",
        runnable_command="npm run objc3c -- validate-concurrency-conformance",
        required_positive=(
            "tests/tooling/runtime/continuation_runtime_helper_probe.cpp",
            "tests/tooling/runtime/live_continuation_runtime_integration_probe.cpp",
            "tests/tooling/runtime/live_task_runtime_and_executor_implementation_probe.cpp",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/non_async_task_runtime_rejected.objc3",
            "tests/tooling/fixtures/native/task_group_without_scope_rejected.objc3",
        ),
        required_diagnostic_codes=("O3S343",),
    ),
    CapabilityRowExpectation(
        issue=8155,
        capability_id="runtime.concurrency.actor-mailbox-isolation",
        support_claim="objc3c.behavior.runtime.concurrency-actor-mailbox-isolation",
        owner_phase="runtime",
        behavior_fixture="tests/tooling/fixtures/native/live_actor_mailbox_runtime_positive.objc3",
        manifest_command="npm run objc3c -- validate-concurrency-conformance",
        runnable_command="npm run objc3c -- validate-concurrency-conformance",
        required_positive=(
            "tests/native/runtime/concurrency/actor_executor_contract.objc3",
            "tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp",
            "tests/tooling/runtime/actor_runtime_executor_contract_probe.cpp",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/actor_nonisolated_executor_rejected.objc3",
            "tests/tooling/fixtures/native/non_actor_actor_hop_rejected.objc3",
        ),
        required_diagnostic_codes=("O3S342", "O3S343"),
    ),
    CapabilityRowExpectation(
        issue=8155,
        capability_id="language.metaprogramming.property-behavior-semantics",
        support_claim="objc3c.behavior.language.metaprogramming.property-behavior-semantics",
        owner_phase="sema",
        behavior_fixture="tests/tooling/fixtures/native/property_behavior_legality_positive.objc3",
        manifest_command="npm run objc3c -- test-runtime-acceptance",
        runnable_command="npm run objc3c -- test-runtime-acceptance",
        required_positive=(
            "tests/tooling/fixtures/metaprogramming_interop_closure/property_behavior_runtime_materialization_policy.json",
            "tests/tooling/runtime/expansion_host_runtime_boundary_probe.cpp",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/property_behavior_legality_negative_unsupported.objc3",
            "tests/tooling/fixtures/native/property_behavior_legality_negative_nonobject.objc3",
        ),
        required_diagnostic_codes=("O3S326", "O3S327"),
    ),
    CapabilityRowExpectation(
        issue=8155,
        capability_id="language.metaprogramming.derive-expansion-inventory",
        support_claim="objc3c.behavior.language.metaprogramming.derive-expansion-inventory",
        owner_phase="sema",
        behavior_fixture="tests/tooling/fixtures/native/derive_expansion_inventory_positive.objc3",
        manifest_command="npm run objc3c -- validate-metaprogramming-conformance",
        runnable_command="npm run objc3c -- validate-metaprogramming-conformance",
        required_positive=(
            "tests/tooling/fixtures/metaprogramming_public_surface/macro_metaprogramming_public_surface_contract.json",
            "native/objc3c/src/sema/objc3_semantic_passes_protocol_metaprogramming_helpers.inc",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/derive_expansion_inventory_negative_unsupported.objc3",
            "tests/tooling/fixtures/native/derive_expansion_inventory_negative_selector_conflict.objc3",
        ),
        required_diagnostic_codes=("O3S317", "O3S319"),
    ),
    CapabilityRowExpectation(
        issue=8155,
        capability_id="language.metaprogramming.macro-safety-sandbox-determinism",
        support_claim="objc3c.behavior.language.metaprogramming.macro-safety-sandbox-determinism",
        owner_phase="sema",
        behavior_fixture="tests/tooling/fixtures/native/macro_safety_sandbox_positive.objc3",
        manifest_command="npm run objc3c -- validate-metaprogramming-conformance",
        runnable_command="npm run objc3c -- validate-metaprogramming-conformance",
        required_positive=(
            "tests/tooling/fixtures/metaprogramming_public_surface/macro_metaprogramming_public_surface_contract.json",
            "tests/tooling/fixtures/security_hardening/macro_supply_chain_trust_registry.json",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_missing_metadata.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_invalid_package.objc3",
        ),
        required_diagnostic_codes=("O3S320", "O3S322"),
    ),
    CapabilityRowExpectation(
        issue=8155,
        capability_id="runtime.metaprogramming.host-cache-boundary",
        support_claim="objc3c.behavior.runtime.metaprogramming.host-cache-boundary",
        owner_phase="runtime",
        behavior_fixture="tests/tooling/fixtures/native/expansion_host_runtime_boundary_positive.objc3",
        manifest_command="npm run objc3c -- test-runtime-acceptance",
        runnable_command="npm run objc3c -- test-runtime-acceptance",
        required_positive=(
            "tests/tooling/fixtures/native/macro_host_process_provider.objc3",
            "tests/tooling/fixtures/native/macro_host_process_consumer.objc3",
            "tests/tooling/runtime/expansion_host_runtime_boundary_probe.cpp",
            "tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_missing_cache_key.objc3",
            "tests/tooling/fixtures/native/macro_safety_sandbox_negative_missing_sandbox_policy.objc3",
        ),
        required_diagnostic_codes=("O3S331", "O3S332"),
    ),
    CapabilityRowExpectation(
        issue=8155,
        capability_id="runtime.interop.package-loader-bridge",
        support_claim="objc3c.behavior.runtime.interop.package-loader-bridge",
        owner_phase="runtime",
        behavior_fixture="tests/tooling/fixtures/native/bridge_packaging_toolchain_consumer.objc3",
        manifest_command="npm run objc3c -- validate-interop-conformance",
        runnable_command="npm run objc3c -- validate-interop-conformance",
        required_positive=(
            "tests/tooling/fixtures/native/bridge_packaging_toolchain_provider.objc3",
            "tests/tooling/runtime/bridge_packaging_toolchain_probe.cpp",
            "tests/tooling/runtime/header_module_bridge_generation_probe.cpp",
        ),
        required_negative=(
            "tests/tooling/runtime/package_loader_fail_closed_diagnostics_probe.cpp",
            "tests/tooling/fixtures/runtime_import_interop_bridge_metadata/tampered_bridge_surface.json",
            "tests/tooling/fixtures/native/recovery/negative/negative_objcxx_swift_bridge_conflicting_metadata.objc3",
        ),
        required_diagnostic_codes=("O3PKG8052", "O3RT004"),
    ),
    CapabilityRowExpectation(
        issue=8155,
        capability_id="runtime.interop.mixed-image-replay",
        support_claim="objc3c.behavior.runtime.interop.mixed-image-replay",
        owner_phase="runtime",
        behavior_fixture="tests/tooling/fixtures/native/runtime_packaging_consumer.objc3",
        manifest_command="npm run objc3c -- validate-interop-conformance",
        runnable_command="npm run objc3c -- validate-interop-conformance",
        required_positive=(
            "tests/tooling/fixtures/native/runtime_packaging_provider.objc3",
            "tests/tooling/runtime/import_module_execution_matrix_probe.cpp",
            "tests/tooling/runtime/multi_image_registration_reset_replay_probe.cpp",
        ),
        required_negative=(
            "tests/tooling/fixtures/native/execution/negative/module_duplicate_declaration.objc3",
            "tests/tooling/fixtures/native/missing_replay_proof_rejected.objc3",
            "tests/tooling/fixtures/native/recovery/negative/negative_objcxx_swift_bridge_conflicting_metadata.objc3",
        ),
        required_diagnostic_codes=("O3RT004",),
    ),
)

ADVANCED_RUNTIME_RESERVED_BOUNDARIES = {
    "runtime.blocks.full-language-closure": "Only explicit block capture legality, copy/dispose/invoke, and byref forwarding rows may publish block support.",
    "runtime.arc.full-automation": "ARC cleanup integration is public only through the checked helper-backed row.",
    "runtime.errors.generalized-foreign-exception-abi": "Live NSError/status bridge cleanup does not claim a generalized foreign exception ABI.",
    "runtime.concurrency.broad-async-actor-closure": "Task continuation and actor mailbox rows do not claim broad async/actor ABI closure.",
    "runtime.metaprogramming.arbitrary-macro-ecosystem": "Checked metaprogramming rows do not claim arbitrary third-party macro expansion.",
    "runtime.interop.broad-runtime-closure": "Package-loader and mixed-image replay rows do not claim broad interop closure or public ABI widening.",
}

ADVANCED_RUNTIME_UMBRELLA_EVIDENCE = {
    "docs/support/hard_cutover_capability_truth.md",
    "spec/PART_6_ERRORS_RESULTS_THROWS.md",
    "spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md",
    "spec/PART_10_METAPROGRAMMING_DERIVES_MACROS_PROPERTY_BEHAVIORS.md",
    "scripts/objc3c_runtime_acceptance/domains/advanced_runtime_capability_split.py",
    "tests/tooling/test_runtime_capability_public_split.py",
}


RESERVED_UMBRELLA_ROWS = {
    "runtime.object-model.full-realization": "8154 object-model umbrella",
    "language.advanced-runtime-closure": "8155 advanced runtime umbrella",
}


def _load_json(path: Path) -> dict[str, Any]:
    with path.open(encoding="utf-8") as handle:
        payload = json.load(handle)
    if not isinstance(payload, dict):
        raise NextRuntimePublicRowsError(f"{path.relative_to(ROOT)} is not a JSON object")
    return payload


def _row_map(rows: Any, key: str) -> dict[str, dict[str, Any]]:
    if not isinstance(rows, list):
        raise NextRuntimePublicRowsError(f"expected {key} rows to be a list")
    result: dict[str, dict[str, Any]] = {}
    for row in rows:
        if isinstance(row, dict) and row.get(key) is not None:
            result[str(row[key])] = row
    return result


def _append(failures: list[str], condition: bool, message: str) -> None:
    if not condition:
        failures.append(message)


def _as_set(raw: Any) -> set[str]:
    if not isinstance(raw, list):
        return set()
    return {str(item) for item in raw}


def _path_is_repo_file(path: str) -> bool:
    return not path.startswith(("tmp/", "tmp\\")) and (ROOT / path).is_file()


def _check_paths(failures: list[str], capability_id: str, paths: set[str]) -> None:
    for path in sorted(paths):
        _append(
            failures,
            _path_is_repo_file(path),
            f"{capability_id} references missing or tmp-backed evidence path: {path}",
        )


def _check_expected_row(
    failures: list[str],
    expected: CapabilityRowExpectation,
    matrix_rows: dict[str, dict[str, Any]],
    evidence_rows: list[dict[str, Any]],
    manifest_claims: dict[str, dict[str, Any]],
    manifest_fixtures: dict[str, dict[str, Any]],
    catalog_rows: dict[str, dict[str, Any]],
) -> None:
    matrix_row = matrix_rows.get(expected.capability_id)
    _append(failures, matrix_row is not None, f"missing matrix row {expected.capability_id}")
    if matrix_row is not None:
        _append(
            failures,
            matrix_row.get("state") == "implemented",
            f"{expected.capability_id} must be implemented",
        )
        _append(
            failures,
            matrix_row.get("support_claims") == [expected.support_claim],
            f"{expected.capability_id} must publish only {expected.support_claim}",
        )
        evidence = matrix_row.get("evidence", [])
        matrix_evidence_paths = {
            str(item.get("path"))
            for item in evidence
            if isinstance(item, dict) and item.get("path") is not None
        }
        _append(
            failures,
            expected.behavior_fixture in matrix_evidence_paths,
            f"{expected.capability_id} matrix row is missing behavior fixture evidence",
        )

    manifest_claim = manifest_claims.get(expected.support_claim)
    _append(failures, manifest_claim is not None, f"missing manifest claim {expected.support_claim}")
    if manifest_claim is not None:
        _append(
            failures,
            manifest_claim.get("owner_phase") == expected.owner_phase,
            f"{expected.support_claim} manifest owner_phase drifted",
        )
        _append(
            failures,
            manifest_claim.get("behavior_fixture") == expected.behavior_fixture,
            f"{expected.support_claim} manifest behavior_fixture drifted",
        )
        _append(
            failures,
            manifest_claim.get("executable_command") == expected.manifest_command,
            f"{expected.support_claim} manifest command drifted",
        )

    manifest_fixture = manifest_fixtures.get(expected.behavior_fixture)
    _append(
        failures,
        manifest_fixture is not None,
        f"missing canonical manifest fixture {expected.behavior_fixture}",
    )
    if manifest_fixture is not None:
        _append(
            failures,
            manifest_fixture.get("owner_phase") == expected.owner_phase,
            f"{expected.behavior_fixture} fixture owner_phase drifted",
        )
        _append(
            failures,
            manifest_fixture.get("fixture_kind") == "positive",
            f"{expected.behavior_fixture} must stay positive evidence",
        )

    matching_evidence = [
        row
        for row in evidence_rows
        if row.get("capability_id") == expected.capability_id
        and row.get("support_claim") == expected.support_claim
    ]
    _append(
        failures,
        bool(matching_evidence),
        f"missing evidence-map rows for {expected.capability_id}",
    )
    evidence_paths = {
        str(row.get("path"))
        for row in matching_evidence
        if row.get("path") is not None
    }
    _append(
        failures,
        expected.behavior_fixture in evidence_paths,
        f"{expected.capability_id} evidence map is missing behavior fixture evidence",
    )

    catalog_row = catalog_rows.get(expected.support_claim)
    _append(failures, catalog_row is not None, f"missing runnable catalog row {expected.support_claim}")
    if catalog_row is None:
        return

    _append(
        failures,
        catalog_row.get("capability_id") == expected.capability_id,
        f"{expected.support_claim} catalog capability_id drifted",
    )
    _append(
        failures,
        catalog_row.get("owner_phase") == expected.owner_phase,
        f"{expected.support_claim} catalog owner_phase drifted",
    )
    _append(
        failures,
        catalog_row.get("runnable_command") == expected.runnable_command,
        f"{expected.support_claim} catalog runnable_command drifted",
    )
    _append(
        failures,
        str(catalog_row.get("runnable_command", "")).startswith(PUBLIC_COMMAND_PREFIX),
        f"{expected.support_claim} must use the public npm workflow command surface",
    )

    positive = _as_set(catalog_row.get("positive_evidence"))
    negative = _as_set(catalog_row.get("negative_evidence"))
    codes = _as_set(catalog_row.get("required_diagnostic_codes"))
    source_truth = _as_set(catalog_row.get("source_truth_requirements"))

    _append(
        failures,
        expected.behavior_fixture in positive,
        f"{expected.support_claim} catalog positive evidence omits its manifest fixture",
    )
    _append(
        failures,
        set(expected.required_positive) <= positive,
        f"{expected.support_claim} catalog positive evidence is missing required anchors",
    )
    _append(
        failures,
        set(expected.required_negative) <= negative,
        f"{expected.support_claim} catalog negative evidence is missing fail-closed anchors",
    )
    _append(
        failures,
        set(expected.required_diagnostic_codes) <= codes,
        f"{expected.support_claim} catalog diagnostic code coverage drifted",
    )
    _append(
        failures,
        bool(source_truth),
        f"{expected.support_claim} catalog must document source truth requirements",
    )

    _check_paths(
        failures,
        expected.capability_id,
        {
            expected.behavior_fixture,
            str(catalog_row.get("conformance_fixture", "")),
            str(catalog_row.get("traceability_fixture", "")),
            *positive,
            *negative,
        },
    )


def _check_advanced_runtime_reserved_umbrella(
    failures: list[str],
    matrix_rows: dict[str, dict[str, Any]],
    evidence_rows: list[dict[str, Any]],
) -> None:
    umbrella = matrix_rows.get("language.advanced-runtime-closure")
    if umbrella is None:
        return
    boundary_doc = (ROOT / "docs/support/hard_cutover_capability_truth.md").read_text(
        encoding="utf-8"
    )

    evidence = umbrella.get("evidence", [])
    umbrella_paths = {
        str(item.get("path"))
        for item in evidence
        if isinstance(item, dict) and item.get("path") is not None
    }
    _append(
        failures,
        ADVANCED_RUNTIME_UMBRELLA_EVIDENCE <= umbrella_paths,
        "language.advanced-runtime-closure umbrella is missing reserved-boundary evidence anchors",
    )
    _append(
        failures,
        all(not path.startswith(("tmp/", "tmp\\")) for path in umbrella_paths),
        "language.advanced-runtime-closure umbrella must not use tmp reports as source truth",
    )

    matching_evidence = [
        row
        for row in evidence_rows
        if row.get("capability_id") == "language.advanced-runtime-closure"
    ]
    _append(
        failures,
        matching_evidence and all(not row.get("support_claim") for row in matching_evidence),
        "language.advanced-runtime-closure evidence-map rows must remain non-claiming",
    )

    summary = str(umbrella.get("summary", ""))
    for boundary_id, required_text in ADVANCED_RUNTIME_RESERVED_BOUNDARIES.items():
        _append(
            failures,
            boundary_id.startswith("runtime."),
            f"{boundary_id} must stay scoped to runtime boundary taxonomy",
        )
        _append(
            failures,
            bool(required_text),
            f"{boundary_id} reserved-boundary description is empty",
        )
        _append(
            failures,
            boundary_id in boundary_doc,
            f"{boundary_id} must be documented as a non-claiming reserved boundary",
        )
    _append(
        failures,
        "remaining broad runtime closure stays reserved" in summary,
        "language.advanced-runtime-closure summary must keep broad runtime closure reserved",
    )


def _support_claim_suffix(claim: str) -> str:
    prefix = "objc3c.behavior."
    if claim.startswith(prefix):
        return claim[len(prefix) :]
    return claim


def _check_advanced_runtime_reserved_capabilities_fail_closed(
    failures: list[str],
    matrix_rows: dict[str, dict[str, Any]],
    evidence_rows: list[dict[str, Any]],
    manifest_claims: dict[str, dict[str, Any]],
    catalog_rows: dict[str, dict[str, Any]],
) -> None:
    reserved_capability_ids = {
        "language.advanced-runtime-closure",
        *ADVANCED_RUNTIME_RESERVED_BOUNDARIES,
    }

    for capability_id in sorted(reserved_capability_ids):
        matrix_row = matrix_rows.get(capability_id)
        if matrix_row is not None:
            _append(
                failures,
                matrix_row.get("state") in {"reserved", "internal", "rejected"},
                f"{capability_id} must fail closed if a matrix row exists",
            )
            _append(
                failures,
                not matrix_row.get("support_claims"),
                f"{capability_id} must not publish support_claims",
            )

    for row in evidence_rows:
        capability_id = str(row.get("capability_id", ""))
        support_claim = str(row.get("support_claim", ""))
        _append(
            failures,
            capability_id not in reserved_capability_ids or not support_claim,
            f"{capability_id} evidence row must stay non-claiming",
        )
        _append(
            failures,
            _support_claim_suffix(support_claim) not in reserved_capability_ids,
            f"{support_claim} must not publish a reserved advanced-runtime capability",
        )

    for claim_id, row in manifest_claims.items():
        capability_id = str(row.get("capability_id", ""))
        _append(
            failures,
            _support_claim_suffix(claim_id) not in reserved_capability_ids,
            f"{claim_id} must not publish a reserved advanced-runtime capability",
        )
        _append(
            failures,
            capability_id not in reserved_capability_ids,
            f"{claim_id} must not map to reserved capability {capability_id}",
        )

    for support_claim, row in catalog_rows.items():
        capability_id = str(row.get("capability_id", ""))
        _append(
            failures,
            _support_claim_suffix(support_claim) not in reserved_capability_ids,
            f"{support_claim} catalog row must not publish a reserved advanced-runtime capability",
        )
        _append(
            failures,
            capability_id not in reserved_capability_ids,
            f"{support_claim} catalog row must not map to reserved capability {capability_id}",
        )


def validate_next_runtime_public_rows() -> dict[str, Any]:
    matrix = _load_json(MATRIX_PATH)
    evidence_map = _load_json(EVIDENCE_MAP_PATH)
    manifest = _load_json(MANIFEST_PATH)
    catalog = _load_json(CATALOG_PATH)

    matrix_rows = _row_map(matrix.get("capabilities"), "id")
    evidence_rows = evidence_map.get("rows")
    if not isinstance(evidence_rows, list):
        raise NextRuntimePublicRowsError("evidence map rows must be a list")
    evidence_dict_rows = [row for row in evidence_rows if isinstance(row, dict)]
    manifest_claims = _row_map(manifest.get("support_claims"), "claim_id")
    manifest_fixtures = _row_map(manifest.get("fixtures"), "path")
    catalog_rows = _row_map(catalog.get("rows"), "support_claim")

    failures: list[str] = []
    issue_refs = set(catalog.get("issue_refs", []))
    for issue in (8154, 8155):
        _append(
            failures,
            issue in issue_refs,
            f"runnable evidence catalog must include issue #{issue}",
        )

    for capability_id, label in RESERVED_UMBRELLA_ROWS.items():
        row = matrix_rows.get(capability_id)
        _append(failures, row is not None, f"missing {label} reserved row")
        if row is not None:
            _append(
                failures,
                row.get("state") == "reserved",
                f"{label} must remain reserved, not implemented",
            )
            _append(
                failures,
                not row.get("support_claims"),
                f"{label} must not carry a public support claim",
            )

    expectations = OBJECT_MODEL_EXPECTATIONS + ADVANCED_RUNTIME_EXPECTATIONS
    for expected in expectations:
        _check_expected_row(
            failures,
            expected,
            matrix_rows,
            evidence_dict_rows,
            manifest_claims,
            manifest_fixtures,
            catalog_rows,
        )
    _check_advanced_runtime_reserved_umbrella(
        failures,
        matrix_rows,
        evidence_dict_rows,
    )
    _check_advanced_runtime_reserved_capabilities_fail_closed(
        failures,
        matrix_rows,
        evidence_dict_rows,
        manifest_claims,
        catalog_rows,
    )

    report = {
        "contract_id": "objc3c.next-runtime-public-rows.validation.v1",
        "status": "PASS" if not failures else "FAIL",
        "issues": [8154, 8155],
        "object_model_rows": [row.capability_id for row in OBJECT_MODEL_EXPECTATIONS],
        "advanced_runtime_rows": [
            row.capability_id for row in ADVANCED_RUNTIME_EXPECTATIONS
        ],
        "advanced_runtime_reserved_boundaries": sorted(
            ADVANCED_RUNTIME_RESERVED_BOUNDARIES
        ),
        "reserved_umbrella_rows": sorted(RESERVED_UMBRELLA_ROWS),
        "failures": failures,
    }
    if failures:
        raise NextRuntimePublicRowsError("\n".join(failures))
    return report


def main() -> int:
    try:
        report = validate_next_runtime_public_rows()
    except NextRuntimePublicRowsError as exc:
        print("objc3c-next-runtime-public-rows: FAIL")
        print(str(exc))
        return 1

    print("objc3c-next-runtime-public-rows: PASS")
    print(f"object_model_rows: {len(report['object_model_rows'])}")
    print(f"advanced_runtime_rows: {len(report['advanced_runtime_rows'])}")
    print(
        "advanced_runtime_reserved_boundaries: "
        f"{len(report['advanced_runtime_reserved_boundaries'])}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
