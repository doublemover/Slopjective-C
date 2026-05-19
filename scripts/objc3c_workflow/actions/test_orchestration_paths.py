"""Path and test-target constants for public test orchestration actions."""

from __future__ import annotations

from ..environment import ROOT

COMPILE_WRAPPER_SELF_AUDIT_PY = (
    ROOT / "scripts" / "check_objc3c_compile_wrapper_self_audit.py"
)
SMOKE_PS1 = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
REPLAY_PS1 = ROOT / "scripts" / "check_objc3c_execution_replay_proof.ps1"
RECOVERY_PS1 = ROOT / "scripts" / "check_objc3c_native_recovery_contract.ps1"
MATRIX_PS1 = ROOT / "scripts" / "run_objc3c_native_fixture_matrix.ps1"
NEGATIVE_EXPECTATIONS_PS1 = ROOT / "scripts" / "check_objc3c_negative_fixture_expectations.ps1"
BEHAVIOR_MATRIX_PY = ROOT / "scripts" / "check_objc3c_behavior_matrix.py"

LLVM_CAPABILITY_ROUTING_TESTS = (
    "tests/tooling/test_probe_objc3c_llvm_capabilities.py",
    "tests/tooling/test_objc3c_library_cli_parity_source_mode.py::test_parity_source_mode_routes_backend_from_capabilities_when_enabled",
    "tests/tooling/test_objc3c_library_cli_parity_source_mode.py::test_parity_source_mode_fail_closes_when_capability_parity_is_unavailable",
    "tests/tooling/test_objc3c_library_cli_parity_source_mode.py::test_parity_source_mode_fail_closes_when_capability_routing_is_requested_without_summary",
    "tests/tooling/test_objc3c_driver_llvm_capability_routing_extraction.py",
    "tests/tooling/test_objc3c_driver_cli_extraction.py",
)
