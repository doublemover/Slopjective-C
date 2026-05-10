from __future__ import annotations

from typing import Any


def assert_success_payload(payload: dict[str, Any]) -> None:
    sema_parity = payload["sema_type_system_parity"]

    assert payload["mode"] == "objc3c-llvm-capabilities-v2"
    assert payload["ok"] is True
    assert payload["failures"] == []
    assert payload["clang"]["version_duration_ms"] >= 0.0
    assert payload["llc"]["version_duration_ms"] >= 0.0
    assert payload["llc_features"]["help_duration_ms"] >= 0.0
    assert payload["llc_features"]["version_with_filetype_duration_ms"] >= 0.0
    assert payload["llc_features"]["supports_filetype_obj"] is True
    assert sema_parity["deterministic_semantic_diagnostics"] is True
    assert sema_parity["deterministic_type_metadata_handoff"] is True
    assert sema_parity["parity_ready"] is True
    assert sema_parity["blockers"] == []


def assert_llc_missing_payload(payload: dict[str, Any]) -> None:
    sema_parity = payload["sema_type_system_parity"]

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


def assert_filetype_support_payload(payload: dict[str, Any]) -> None:
    assert payload["ok"] is True
    assert payload["llc_features"]["supports_filetype_obj"] is True
    assert payload["sema_type_system_parity"]["parity_ready"] is True


def assert_clang_missing_payload(payload: dict[str, Any]) -> None:
    sema_parity = payload["sema_type_system_parity"]

    assert payload["clang"]["found"] is False
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
    assert sema_parity["parity_ready"] is False
    assert "llc executable missing" in sema_parity["blockers"]


def assert_package_wires_llvm_capability_probe_script(
    payload: dict[str, Any],
) -> None:
    scripts = payload["scripts"]

    assert "check:objc3c:llvm-capabilities" in scripts
    command = scripts["check:objc3c:llvm-capabilities"]
    assert "scripts/probe_objc3c_llvm_capabilities.py" in command
    assert "tmp/artifacts/objc3c-native/llvm_capabilities/summary.json" in command
