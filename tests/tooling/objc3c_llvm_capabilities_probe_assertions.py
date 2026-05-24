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
    assert payload["clangxx"]["version_duration_ms"] >= 0.0
    assert payload["llvm_ar"]["version_duration_ms"] >= 0.0
    assert payload["llvm_config"]["version_duration_ms"] >= 0.0
    assert payload["llc_features"]["help_duration_ms"] >= 0.0
    assert payload["llc_features"]["version_with_filetype_duration_ms"] >= 0.0
    assert payload["llc_features"]["supports_filetype_obj"] is True
    assert payload["llvm_config_features"]["headers_libraries_discovered"] is True
    assert payload["toolchain_resolution"]["clang"]["configured_path"] == "clang"
    assert payload["toolchain_resolution"]["llc"]["configured_path"] == "llc"
    assert payload["toolchain_resolution"]["clang++"]["configured_path"] == "clang++"
    assert payload["toolchain_resolution"]["llvm-ar"]["configured_path"] == "llvm-ar"
    assert payload["toolchain_resolution"]["llvm-config"]["configured_path"] == "llvm-config"
    assert payload["toolchain_resolution"]["llvm-config"]["includedir"] == "/opt/llvm/include"
    assert payload["toolchain_resolution"]["llvm-config"]["libdir"] == "/opt/llvm/lib"
    assert (
        payload["toolchain_resolution"]["llvm-config"][
            "headers_libraries_discovery_source"
        ]
        == "llvm-config"
    )
    assert payload["toolchain_identity"]["contract_id"] == "objc3c.llvm.coherent-toolchain-identity.v1"
    assert payload["toolchain_identity"]["version_status"] == "coherent"
    assert payload["toolchain_identity"]["claimable"] is True
    assert llvm_matrix["contract_id"] == "objc3c.llvm.version_support_matrix.v1"
    assert llvm_matrix["issue_ref"] == 8232
    assert llvm_matrix["native_object_emission_contract"] == {
        "contract_id": "objc3c.llvm.native-object-emission.fail-closed.v1",
        "issue_ref": 8232,
        "required_tool": "llc",
        "required_probe": "llc --filetype=obj",
        "required_target_probe": "llc --filetype=obj --mtriple=<target> emits a non-empty object",
        "status": "native_object_emission_supported",
        "missing_llc_status": "native_object_emission_missing_llc",
        "missing_filetype_status": "native_object_emission_filetype_obj_unavailable",
        "target_object_status": "native_object_emission_target_object_unavailable",
        "mixed_toolchain_status": "native_object_emission_mixed_toolchain_root",
        "mismatched_version_status": "native_object_emission_mismatched_tool_versions",
        "unsupported_version_status": "native_object_emission_unsupported_tool_version",
        "unresolved_version_status": "native_object_emission_unresolved_tool_version",
        "hosted_runner_behavior": "fail-closed-no-native-object-success-claim",
        "task_hygiene_behavior": "skip-no-success-claim-when-native-object-emission-unavailable",
        "conformance_minima_behavior": "fail-closed-before-cross-lane-runtime-proof",
        "required_conformance_minima_env": "OBJC3C_REQUIRE_HOSTED_NATIVE_OBJECT_EMISSION",
        "fallback_policy": "no-clang-fallback-success-claim",
        "coherent_toolchain_policy": "no-mixed-root-or-mismatched-version-success-claim",
    }
    assert [record["tool_name"] for record in llvm_matrix["llvm_tool_records"]] == [
        "clang",
        "clang++",
        "llc",
        "llvm-ar",
        "llvm-config",
    ]
    matrix_entry = llvm_matrix["toolchain_matrix_entries"][0]
    assert matrix_entry["support_status"] == "supported"
    assert matrix_entry["object_emission_capability"] == "supported"
    assert matrix_entry["package_capability"] == "supported"
    assert matrix_entry["native_execution_capability"] == "supported"
    assert "llvm-direct-object-emission" in matrix_entry["supported_features"]
    assert "package-archive-tool" in matrix_entry["supported_features"]
    assert "headers-libraries-discovery" in matrix_entry["supported_features"]
    assert "coherent-llvm-toolchain-identity" in matrix_entry["supported_features"]
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


def assert_target_object_unsupported_payload(payload: dict[str, Any]) -> None:
    sema_parity = payload["sema_type_system_parity"]
    llvm_matrix = payload["llvm_support_matrix"]

    assert payload["ok"] is False
    assert payload["llc"]["found"] is True
    assert payload["llc_features"]["supports_filetype_obj"] is True
    assert payload["llc_features"]["supports_target_object_emission"] is False
    assert payload["llc_features"]["target_object_exit_code"] == 1
    assert (
        llvm_matrix["native_object_emission_contract"]["status"]
        == "native_object_emission_target_object_unavailable"
    )
    assert sema_parity["deterministic_type_metadata_handoff"] is False
    assert any(
        blocker.startswith("llc target object emission failed for ")
        for blocker in sema_parity["blockers"]
    )
    matrix_entry = llvm_matrix["toolchain_matrix_entries"][0]
    assert matrix_entry["object_emission_capability"] == "rejected"
    assert any(
        feature["feature"] == "llvm-direct-object-emission"
        and feature["reason"].startswith("llc target object emission failed for ")
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


def assert_llvm_ar_missing_payload(payload: dict[str, Any]) -> None:
    llvm_matrix = payload["llvm_support_matrix"]
    matrix_entry = llvm_matrix["toolchain_matrix_entries"][0]

    assert payload["ok"] is False
    assert payload["llvm_ar"]["found"] is False
    assert any("llvm-ar executable not found" in failure for failure in payload["failures"])
    assert matrix_entry["object_emission_capability"] == "supported"
    assert matrix_entry["package_capability"] == "rejected"
    assert matrix_entry["native_execution_capability"] == "supported"
    assert any(
        feature["feature"] == "package-archive-tool"
        for feature in matrix_entry["rejected_features"]
    )


def assert_llvm_config_headers_missing_payload(payload: dict[str, Any]) -> None:
    llvm_matrix = payload["llvm_support_matrix"]
    matrix_entry = llvm_matrix["toolchain_matrix_entries"][0]

    assert payload["ok"] is False
    assert payload["llvm_config"]["found"] is True
    assert payload["llvm_config_features"]["headers_libraries_discovered"] is False
    assert any("headers/libs discovery unavailable" in failure for failure in payload["failures"])
    assert matrix_entry["object_emission_capability"] == "supported"
    assert matrix_entry["package_capability"] == "rejected"
    assert matrix_entry["native_execution_capability"] == "rejected"
    assert any(
        feature["feature"] == "headers-libraries-discovery"
        for feature in matrix_entry["rejected_features"]
    )


def assert_mismatched_llvm_tool_versions_payload(payload: dict[str, Any]) -> None:
    identity = payload["toolchain_identity"]
    matrix_entry = payload["llvm_support_matrix"]["toolchain_matrix_entries"][0]

    assert payload["ok"] is False
    assert identity["version_status"] == "mismatched"
    assert identity["claimable"] is False
    assert (
        payload["llvm_support_matrix"]["native_object_emission_contract"]["status"]
        == "native_object_emission_mismatched_tool_versions"
    )
    assert matrix_entry["object_emission_capability"] == "rejected"
    assert matrix_entry["package_capability"] == "rejected"
    assert matrix_entry["native_execution_capability"] == "rejected"
    assert any(
        feature["feature"] == "coherent-llvm-toolchain-identity"
        for feature in matrix_entry["rejected_features"]
    )


def assert_mixed_toolchain_root_payload(payload: dict[str, Any]) -> None:
    identity = payload["toolchain_identity"]
    matrix_entry = payload["llvm_support_matrix"]["toolchain_matrix_entries"][0]

    assert payload["ok"] is False
    assert identity["root_status"] == "mixed"
    assert identity["claimable"] is False
    assert (
        payload["llvm_support_matrix"]["native_object_emission_contract"]["status"]
        == "native_object_emission_mixed_toolchain_root"
    )
    assert matrix_entry["support_status"] == "rejected"
    assert matrix_entry["object_emission_capability"] == "rejected"


def assert_windows_install_root_header_library_payload(payload: dict[str, Any]) -> None:
    matrix_entry = payload["llvm_support_matrix"]["toolchain_matrix_entries"][0]
    resolution = payload["toolchain_resolution"]["llvm-config"]

    assert payload["ok"] is True
    assert payload["llvm_config"]["found"] is False
    assert payload["llvm_config_features"]["headers_libraries_discovered"] is True
    assert payload["llvm_config_features"]["discovery_source"] == "install-root"
    assert payload["toolchain_identity"]["claimable"] is True
    assert resolution["headers_libraries_discovered"] is True
    assert resolution["headers_libraries_discovery_source"] == "install-root"
    assert resolution["includedir"].endswith("LLVM\\include") or resolution[
        "includedir"
    ].endswith("LLVM/include")
    assert resolution["libdir"].endswith("LLVM\\lib") or resolution["libdir"].endswith(
        "LLVM/lib"
    )
    assert matrix_entry["support_status"] == "supported"
    assert matrix_entry["package_capability"] == "supported"
    assert matrix_entry["native_execution_capability"] == "supported"


def assert_package_wires_llvm_capability_probe_script(
    payload: dict[str, Any],
) -> None:
    scripts = payload["scripts"]

    assert list(scripts) == ["objc3c"]
    assert scripts["objc3c"] == "python -m scripts.objc3c_workflow"
