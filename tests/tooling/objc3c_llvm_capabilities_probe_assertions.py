from __future__ import annotations

from typing import Any


def assert_success_payload(payload: dict[str, Any]) -> None:
    sema_parity = payload["sema_type_system_parity"]
    llvm_matrix = payload["llvm_support_matrix"]

    assert payload["mode"] == "objc3c-llvm-capabilities-v2"
    assert payload["ok"] is True
    assert payload["failures"] == []
    assert payload["clang"]["version_duration_ms"] >= 0.0
    assert payload["llc"]["version_duration_ms"] >= 0.0
    assert payload["llc_features"]["help_duration_ms"] >= 0.0
    assert payload["llc_features"]["version_with_filetype_duration_ms"] >= 0.0
    assert payload["llc_features"]["supports_filetype_obj"] is True
    assert payload["toolchain_resolution"]["clang"]["configured_path"] == "clang"
    assert payload["toolchain_resolution"]["llc"]["configured_path"] == "llc"
    assert llvm_matrix["contract_id"] == "objc3c.llvm.version_support_matrix.v1"
    assert llvm_matrix["issue_ref"] == 8232
    assert llvm_matrix["native_object_emission_contract"] == {
        "contract_id": "objc3c.llvm.native-object-emission.fail-closed.v1",
        "issue_ref": 8232,
        "required_tool": "llc",
        "required_probe": "llc --filetype=obj",
        "status": "native_object_emission_supported",
        "missing_llc_status": "native_object_emission_missing_llc",
        "missing_filetype_status": "native_object_emission_filetype_obj_unavailable",
        "hosted_runner_behavior": "fail-closed-no-native-object-success-claim",
        "conformance_minima_behavior": "fail-closed-before-cross-lane-runtime-proof",
        "fallback_policy": "no-clang-fallback-success-claim",
    }
    assert [record["tool_name"] for record in llvm_matrix["llvm_tool_records"]] == [
        "clang",
        "llc",
        "archive-tool",
    ]
    matrix_entry = llvm_matrix["toolchain_matrix_entries"][0]
    assert matrix_entry["support_status"] == "supported"
    assert matrix_entry["object_emission_capability"] == "supported"
    assert "llvm-direct-object-emission" in matrix_entry["supported_features"]
    assert sema_parity["deterministic_semantic_diagnostics"] is True
    assert sema_parity["deterministic_type_metadata_handoff"] is True
    assert sema_parity["parity_ready"] is True
    assert sema_parity["blockers"] == []


def assert_llc_missing_payload(payload: dict[str, Any]) -> None:
    sema_parity = payload["sema_type_system_parity"]
    llvm_matrix = payload["llvm_support_matrix"]

    assert payload["ok"] is False
    assert any("llc executable not found" in failure for failure in payload["failures"])
    assert sema_parity["deterministic_semantic_diagnostics"] is True
    assert sema_parity["deterministic_type_metadata_handoff"] is False
    assert sema_parity["parity_ready"] is False
    assert "llc executable missing" in sema_parity["blockers"]
    assert any(
        "sema/type-system parity capability unavailable:" in failure
        for failure in payload["failures"]
    )
    matrix_entry = llvm_matrix["toolchain_matrix_entries"][0]
    assert llvm_matrix["native_object_emission_contract"]["status"] == "native_object_emission_missing_llc"
    assert matrix_entry["support_status"] == "rejected"
    assert matrix_entry["object_emission_capability"] == "rejected"
    assert any(
        feature["feature"] == "llvm-direct-object-emission"
        for feature in matrix_entry["rejected_features"]
    )


def assert_filetype_support_payload(payload: dict[str, Any]) -> None:
    assert payload["ok"] is True
    assert payload["llc_features"]["supports_filetype_obj"] is True
    assert (
        payload["llvm_support_matrix"]["native_object_emission_contract"]["status"]
        == "native_object_emission_supported"
    )
    assert payload["sema_type_system_parity"]["parity_ready"] is True


def assert_filetype_unsupported_payload(payload: dict[str, Any]) -> None:
    sema_parity = payload["sema_type_system_parity"]
    llvm_matrix = payload["llvm_support_matrix"]

    assert payload["ok"] is False
    assert payload["llc"]["found"] is True
    assert payload["llc_features"]["supports_filetype_obj"] is False
    assert (
        llvm_matrix["native_object_emission_contract"]["status"]
        == "native_object_emission_filetype_obj_unavailable"
    )
    assert sema_parity["deterministic_type_metadata_handoff"] is False
    assert "llc missing --filetype=obj support" in sema_parity["blockers"]
    matrix_entry = llvm_matrix["toolchain_matrix_entries"][0]
    assert matrix_entry["object_emission_capability"] == "rejected"
    assert any(
        feature["feature"] == "llvm-direct-object-emission"
        and feature["reason"] == "llc missing --filetype=obj support"
        for feature in matrix_entry["rejected_features"]
    )


def assert_clang_missing_payload(payload: dict[str, Any]) -> None:
    sema_parity = payload["sema_type_system_parity"]

    assert payload["clang"]["found"] is False
    assert payload["toolchain_resolution"]["clang"]["diagnostic"] == "clang executable not found: clang"
    assert payload["llc_features"]["supports_filetype_obj"] is True
    assert sema_parity["deterministic_semantic_diagnostics"] is False
    assert sema_parity["deterministic_type_metadata_handoff"] is False
    assert sema_parity["parity_ready"] is False
    assert "clang executable missing" in sema_parity["blockers"]
    assert any(
        "sema/type-system parity capability unavailable:" in failure
        for failure in payload["failures"]
    )


def assert_llc_launch_file_not_found_payload(payload: dict[str, Any]) -> None:
    sema_parity = payload["sema_type_system_parity"]

    assert payload["llc"]["found"] is False
    assert payload["llc"]["version_exit_code"] == 127
    assert payload["llc"]["diagnostic"] == "llc executable not found: llc"
    assert payload["toolchain_resolution"]["llc"]["shadowing_status"] == "path-unresolved"
    assert (
        payload["llvm_support_matrix"]["native_object_emission_contract"]["status"]
        == "native_object_emission_missing_llc"
    )
    assert sema_parity["parity_ready"] is False
    assert "llc executable missing" in sema_parity["blockers"]


def assert_package_wires_llvm_capability_probe_script(
    payload: dict[str, Any],
) -> None:
    scripts = payload["scripts"]

    assert list(scripts) == ["objc3c"]
    assert scripts["objc3c"] == "python -m scripts.objc3c_workflow"
