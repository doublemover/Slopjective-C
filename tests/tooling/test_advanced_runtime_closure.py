from __future__ import annotations

import json
import sys
from pathlib import Path

SCRIPTS_ROOT = Path(__file__).resolve().parents[2] / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from scripts.check_objc3c_advanced_runtime_closure import (  # noqa: E402
    ADVANCED_CLOSURE_CANONICAL_SOURCE_DEBUG_MAP_BUNDLE,
    ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT,
    ADVANCED_CLOSURE_NATIVE_ARTIFACT_CONTRACT,
    ADVANCED_CLOSURE_NEGATIVE_MATRIX,
    FORBIDDEN_NATIVE_LLVM_OPERAND_MARKERS,
    FORBIDDEN_NATIVE_SUCCESS_ARTIFACTS,
    REQUIRED_NATIVE_ARTIFACTS,
    REQUIRED_INTERACTION_FEATURE_SETS,
    REQUIRED_LITERAL_NEGATIVE_CASE_IDS,
    REQUIRED_RUNTIME_SOURCE_DEBUG_EXEMPTIONS,
    REQUIRED_RUNTIME_SOURCE_DEBUG_LINKS,
    REQUIRED_UNSUPPORTED_RESERVED_CLAIMS,
    ROOT,
    validate_advanced_runtime_closure,
)


def _read_json(relative_path: str) -> dict[str, object]:
    path = ROOT / relative_path
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _assert_checked_path(relative_path: str) -> None:
    assert not relative_path.startswith(("tmp/", "temp/", "generated/", "build/", "dist/"))
    assert (ROOT / relative_path).exists()


def test_advanced_runtime_closure_enforces_combined_identity_contract() -> None:
    payload = validate_advanced_runtime_closure()

    assert payload["status"] == "PASS"
    assert payload["advanced_runtime_negative_matrix_case_count"] >= 17
    assert (
        payload["advanced_runtime_negative_matrix_interaction_count"]
        == len(REQUIRED_INTERACTION_FEATURE_SETS)
    )
    assert (
        payload["advanced_runtime_combined_identity_contract"]
        == ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT
    )
    assert payload["advanced_runtime_combined_identity_runtime_state_record_count"] >= 8
    assert payload["advanced_runtime_combined_identity_source_graph_record_count"] >= 7
    assert payload["advanced_runtime_combined_identity_debug_map_record_count"] >= 7
    assert (
        payload["advanced_runtime_combined_identity_abi_interaction_record_count"]
        >= 6
    )
    assert (
        payload["advanced_runtime_combined_identity_interaction_count"]
        == len(REQUIRED_INTERACTION_FEATURE_SETS)
    )
    assert (
        payload["advanced_runtime_canonical_source_debug_map"]
        == ADVANCED_CLOSURE_CANONICAL_SOURCE_DEBUG_MAP_BUNDLE
    )
    assert payload["advanced_runtime_canonical_source_map_record_count"] == 7
    assert payload["advanced_runtime_canonical_debug_map_record_count"] == 7
    assert payload["advanced_runtime_canonical_native_line_table_record_count"] == 7
    assert (
        payload["advanced_runtime_runtime_source_debug_link_count"]
        == len(REQUIRED_RUNTIME_SOURCE_DEBUG_LINKS)
    )
    assert (
        payload["advanced_runtime_native_artifact_contract"]
        == ADVANCED_CLOSURE_NATIVE_ARTIFACT_CONTRACT
    )
    assert payload["advanced_runtime_native_compile_attempt_status"] == "native_link_run_ready"
    assert payload["advanced_runtime_native_compile_attempt_exit_code"] == 0
    assert payload["advanced_runtime_native_compile_attempt_diagnostic_count"] == 0
    assert (
        payload["advanced_runtime_native_phase_status_contract"]
        == "objc3c.advanced-runtime.closure.typed-failure-reporting.v1"
    )
    assert payload["advanced_runtime_native_phase_statuses"] == {
        "compile": "native_compile_succeeded",
        "link": "native_link_succeeded",
        "runtime_registration": "runtime_registration_artifact_present",
        "runtime_metadata": "runtime_metadata_artifact_present",
        "error_replay": "error_replay_artifact_present",
        "execution_status": "native_execution_succeeded",
    }
    assert payload["advanced_runtime_native_artifact_ready"] is True
    assert payload["advanced_runtime_native_executable_umbrella_promoted"] is True


def test_combined_identity_contract_links_source_graph_debug_map_and_negatives() -> None:
    contract = _read_json(ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT)
    negative_matrix = _read_json(ADVANCED_CLOSURE_NEGATIVE_MATRIX)

    assert contract["issue_ref"] == 8199
    assert contract["umbrella_support_promoted"] is True
    assert contract["native_executable_umbrella_promoted"] is True
    _assert_checked_path(str(contract["positive_fixture"]))
    _assert_checked_path(str(contract["negative_matrix"]))
    _assert_checked_path(str(contract["language_semantics_contract"]))
    _assert_checked_path(str(contract["debug_source_map_validator"]))
    _assert_checked_path(str(contract["canonical_compiler_emitted_source_debug_map_bundle"]))
    _assert_checked_path(str(contract["native_artifact_contract"]))

    unsupported_policy = contract["unsupported_combination_policy"]  # type: ignore[index]
    assert unsupported_policy["diagnostic"] == "advanced-runtime.unsupported-combination"
    assert set(unsupported_policy["unsupported_statuses"]) == {"rejected", "reserved"}
    assert set(unsupported_policy["reserved_claims"]) == REQUIRED_UNSUPPORTED_RESERVED_CLAIMS

    runtime_source_debug_link_ids = {
        str(record["link_id"])
        for record in contract["runtime_state_source_debug_links"]  # type: ignore[index]
    }
    assert runtime_source_debug_link_ids == set(REQUIRED_RUNTIME_SOURCE_DEBUG_LINKS)
    runtime_source_debug_exemptions = {
        str(record["runtime_state_record_id"])
        for record in contract["runtime_state_source_debug_exemptions"]  # type: ignore[index]
    }
    assert runtime_source_debug_exemptions == REQUIRED_RUNTIME_SOURCE_DEBUG_EXEMPTIONS

    source_graph_ids = {
        str(record["record_id"])
        for record in contract["compiler_owned_source_graph_records"]  # type: ignore[index]
    }
    debug_source_graph_ids = {
        str(record["source_graph_record_id"])
        for record in contract["debug_map_records"]  # type: ignore[index]
    }
    assert debug_source_graph_ids <= source_graph_ids

    case_ids = {
        str(case["case_id"]) for case in negative_matrix["cases"]  # type: ignore[index]
    }
    assert REQUIRED_LITERAL_NEGATIVE_CASE_IDS <= case_ids
    interaction_ids = {
        str(record["interaction_id"])
        for record in contract["interaction_records"]  # type: ignore[index]
    }
    assert interaction_ids == set(REQUIRED_INTERACTION_FEATURE_SETS)
    for record in contract["interaction_records"]:  # type: ignore[index]
        assert set(record["negative_case_ids"]) <= case_ids
        assert set(record["runtime_source_debug_link_ids"]) <= runtime_source_debug_link_ids


def test_native_artifact_contract_claims_compile_artifacts_only() -> None:
    contract = _read_json(ADVANCED_CLOSURE_NATIVE_ARTIFACT_CONTRACT)

    assert contract["issue_ref"] == 8199
    assert contract["followup_issue_ref"] == 8213
    assert contract["status"] == "native_link_run_ready"
    assert contract["provider_fixture"] == "tests/native/runtime/advanced_closure/combined_provider.objc3"
    assert contract["native_compile_claimed"] is True
    assert contract["native_object_artifact_claimed"] is True
    assert contract["native_ir_artifact_claimed"] is True
    assert contract["native_manifest_artifact_claimed"] is True
    assert contract["native_link_claimed"] is True
    assert contract["native_run_claimed"] is True
    assert contract["native_executable_umbrella_promoted"] is True
    assert contract["umbrella_support_promoted"] is True
    assert contract["positive_fixture"] == "tests/native/runtime/advanced_closure/combined_positive.objc3"

    assert contract["expected_diagnostics"] == []
    assert contract["absent_diagnostic_codes"] == []
    assert tuple(contract["required_success_artifacts"]) == REQUIRED_NATIVE_ARTIFACTS
    assert tuple(contract["forbidden_success_artifacts"]) == FORBIDDEN_NATIVE_SUCCESS_ARTIFACTS
    assert tuple(contract["forbidden_llvm_operand_markers"]) == FORBIDDEN_NATIVE_LLVM_OPERAND_MARKERS
    toolchain_gate = contract["native_object_emission_toolchain_gate"]
    assert toolchain_gate == {
        "issue_ref": 8232,
        "required_tool": "llc",
        "required_probe": "llc --filetype=obj",
        "success_status": "native_object_emission_supported",
        "missing_tool_status": "native_object_emission_missing_llc",
        "missing_probe_status": "native_object_emission_filetype_obj_unavailable",
        "hosted_runner_behavior": "fail-closed-no-native-object-success-claim",
        "conformance_minima_behavior": "fail-closed-before-cross-lane-runtime-proof",
        "fallback_policy": "no-clang-fallback-success-claim",
        "diagnostic": "native object emission fail-closed: llc executable not found",
    }
    cross_lane_source = (ROOT / "scripts" / "check_objc3c_cross_lane_e2e.py").read_text(
        encoding="utf-8"
    )
    assert toolchain_gate["missing_tool_status"] in cross_lane_source
    assert "native object, package, or execution success claim is published" in cross_lane_source
    typed_reporting = contract["typed_failure_reporting"]
    assert typed_reporting["contract_id"] == "objc3c.advanced-runtime.closure.typed-failure-reporting.v1"
    assert typed_reporting["issue_ref"] == 8213
    assert typed_reporting["phase_order"] == [
        "compile",
        "link",
        "runtime_registration",
        "runtime_metadata",
        "error_replay",
        "execution_status",
    ]
    assert {
        "native_object_emission_missing_llc",
        "native_object_emission_filetype_obj_unavailable",
        "native_object_emission_backend_unavailable",
    } <= set(typed_reporting["phase_statuses"]["compile"]["values"])


def test_canonical_source_debug_map_links_every_combined_identity_record() -> None:
    contract = _read_json(ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT)
    bundle = _read_json(ADVANCED_CLOSURE_CANONICAL_SOURCE_DEBUG_MAP_BUNDLE)

    source_map_ids = {
        str(record["entry_id"]) for record in bundle["source_maps"]  # type: ignore[index]
    }
    debug_map_ids = {
        str(record["entry_id"]) for record in bundle["debug_maps"]  # type: ignore[index]
    }
    line_table_source_map_ids = {
        str(record["source_map_entry_id"])
        for record in bundle["native_line_tables"]  # type: ignore[index]
    }
    source_graph_record_kinds = {
        str(record["record_id"]): str(record["source_map_record_kind"])
        for record in contract["compiler_owned_source_graph_records"]  # type: ignore[index]
    }
    source_map_kinds = {
        str(record["entry_id"]): str(record["record_kind"])
        for record in bundle["source_maps"]  # type: ignore[index]
    }
    line_table_rows = {
        str(record["row_id"]): record
        for record in bundle["native_line_tables"]  # type: ignore[index]
    }
    contract_runtime_links = {
        str(record["link_id"]): record
        for record in contract["runtime_state_source_debug_links"]  # type: ignore[index]
    }
    bundle_runtime_links = {
        str(record["link_id"]): record
        for record in bundle["runtime_state_links"]  # type: ignore[index]
    }
    assert set(bundle_runtime_links) == set(REQUIRED_RUNTIME_SOURCE_DEBUG_LINKS)

    for record in contract["debug_map_records"]:  # type: ignore[index]
        source_map_id = str(record["source_map_entry_id"])
        assert source_map_id in source_map_ids
        assert str(record["debug_map_entry_id"]) in debug_map_ids
        assert source_map_id in line_table_source_map_ids
        assert (
            source_map_kinds[source_map_id]
            == source_graph_record_kinds[str(record["source_graph_record_id"])]
        )

    for link_id, record in contract_runtime_links.items():
        bundle_record = bundle_runtime_links[link_id]
        assert bundle_record == record
        assert str(record["source_map_entry_id"]) in source_map_ids
        assert str(record["debug_map_entry_id"]) in debug_map_ids
        for row_id in record["native_line_table_row_ids"]:
            line_row = line_table_rows[str(row_id)]
            assert line_row["source_map_entry_id"] == record["source_map_entry_id"]
            assert line_row["source_graph_node_id"] == record["source_graph_node_id"]
