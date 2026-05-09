"""Public test orchestration and native execution fixture workflow actions."""

from __future__ import annotations

import sys

from ..commands import pwsh_file, run, workflow_command
from ..composite_validation import run_composite_validation
from ..environment import PWSH, ROOT
from .application_surfaces import (
    CONFORMANCE_CORPUS_INTEGRATION_PY,
    STDLIB_ADVANCED_INTEGRATION_PY,
    STDLIB_FOUNDATION_INTEGRATION_PY,
    STDLIB_PROGRAM_INTEGRATION_PY,
)
from .developer_tooling import (
    BONUS_EXPERIENCE_INTEGRATION_PY,
    DEVELOPER_TOOLING_INTEGRATION_PY,
)
from .hygiene import TASK_HYGIENE_PY
from .runtime_tests import RUNTIME_ACCEPTANCE_PY

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
    "tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_routes_backend_from_capabilities_when_enabled",
    "tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_fail_closes_when_capability_parity_is_unavailable",
    "tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_fail_closes_when_capability_routing_is_requested_without_summary",
    "tests/tooling/test_objc3c_driver_llvm_capability_routing_extraction.py",
    "tests/tooling/test_objc3c_driver_cli_extraction.py",
)


def action_test_behavior_matrix(_: list[str]) -> int:
    return run([sys.executable, str(BEHAVIOR_MATRIX_PY)])


def action_test_smoke(_: list[str]) -> int:
    return run_composite_validation(
        "test-smoke",
        [
            ("test-behavior-matrix", [sys.executable, str(BEHAVIOR_MATRIX_PY)]),
            (
                "test-runtime-acceptance-fast",
                [sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "fast"],
            ),
            (
                "test-execution-replay-focused",
                [
                    PWSH,
                    "-NoProfile",
                    "-ExecutionPolicy",
                    "Bypass",
                    "-File",
                    str(REPLAY_PS1),
                    "-Limit",
                    "1",
                ],
            ),
        ],
    )


def action_test_ci(_: list[str]) -> int:
    return run_composite_validation(
        "test-ci",
        [
            ("task-hygiene", [sys.executable, str(TASK_HYGIENE_PY)]),
            (
                "validate-developer-tooling",
                [sys.executable, str(DEVELOPER_TOOLING_INTEGRATION_PY)],
            ),
            (
                "validate-bonus-experiences",
                [sys.executable, str(BONUS_EXPERIENCE_INTEGRATION_PY)],
            ),
            (
                "validate-stdlib-foundation",
                [sys.executable, str(STDLIB_FOUNDATION_INTEGRATION_PY)],
            ),
            (
                "validate-stdlib-advanced",
                [sys.executable, str(STDLIB_ADVANCED_INTEGRATION_PY)],
            ),
            (
                "validate-stdlib-program",
                [sys.executable, str(STDLIB_PROGRAM_INTEGRATION_PY)],
            ),
            (
                "validate-performance-governance",
                workflow_command("validate-performance-governance"),
            ),
            (
                "test-execution-smoke",
                [
                    PWSH,
                    "-NoProfile",
                    "-ExecutionPolicy",
                    "Bypass",
                    "-File",
                    str(SMOKE_PS1),
                ],
            ),
            ("test-runtime-acceptance", [sys.executable, str(RUNTIME_ACCEPTANCE_PY)]),
            (
                "test-execution-replay",
                [
                    PWSH,
                    "-NoProfile",
                    "-ExecutionPolicy",
                    "Bypass",
                    "-File",
                    str(REPLAY_PS1),
                ],
            ),
        ],
    )


def action_test_recovery(rest: list[str]) -> int:
    return pwsh_file(RECOVERY_PS1, *rest)


def action_test_execution_smoke(rest: list[str]) -> int:
    return pwsh_file(SMOKE_PS1, *rest)


def action_test_execution_replay(rest: list[str]) -> int:
    return pwsh_file(REPLAY_PS1, *rest)


def action_test_execution_replay_focused(_: list[str]) -> int:
    return pwsh_file(REPLAY_PS1, "-Limit", "1")


def action_test_compile_wrapper_self_audit(_: list[str]) -> int:
    return run([sys.executable, str(COMPILE_WRAPPER_SELF_AUDIT_PY)])


def action_test_llvm_capability_routing(_: list[str]) -> int:
    return run([sys.executable, "-m", "pytest", *LLVM_CAPABILITY_ROUTING_TESTS, "-q"])


def action_test_fixture_matrix(rest: list[str]) -> int:
    return pwsh_file(MATRIX_PS1, *rest)


def action_test_negative_expectations(rest: list[str]) -> int:
    return pwsh_file(NEGATIVE_EXPECTATIONS_PS1, *rest)


def action_test_full(_: list[str]) -> int:
    return run_composite_validation(
        "test-full",
        [
            ("test-behavior-matrix", [sys.executable, str(BEHAVIOR_MATRIX_PY)]),
            (
                "test-compile-wrapper-self-audit",
                [sys.executable, str(COMPILE_WRAPPER_SELF_AUDIT_PY)],
            ),
            (
                "test-execution-smoke",
                [
                    PWSH,
                    "-NoProfile",
                    "-ExecutionPolicy",
                    "Bypass",
                    "-File",
                    str(SMOKE_PS1),
                    "-Limit",
                    "24",
                ],
            ),
            (
                "test-runtime-acceptance-fast",
                [sys.executable, str(RUNTIME_ACCEPTANCE_PY), "--suite", "fast"],
            ),
            (
                "test-execution-replay-focused",
                [
                    PWSH,
                    "-NoProfile",
                    "-ExecutionPolicy",
                    "Bypass",
                    "-File",
                    str(REPLAY_PS1),
                    "-Limit",
                    "1",
                ],
            ),
        ],
    )


def action_test_nightly(_: list[str]) -> int:
    return run_composite_validation(
        "test-nightly",
        [
            (
                "test-execution-smoke",
                [
                    PWSH,
                    "-NoProfile",
                    "-ExecutionPolicy",
                    "Bypass",
                    "-File",
                    str(SMOKE_PS1),
                ],
            ),
            ("test-runtime-acceptance", [sys.executable, str(RUNTIME_ACCEPTANCE_PY)]),
            (
                "test-execution-replay",
                [
                    PWSH,
                    "-NoProfile",
                    "-ExecutionPolicy",
                    "Bypass",
                    "-File",
                    str(REPLAY_PS1),
                ],
            ),
            (
                "validate-conformance-corpus",
                [sys.executable, str(CONFORMANCE_CORPUS_INTEGRATION_PY)],
            ),
            ("validate-stress", workflow_command("validate-stress")),
            ("validate-external-validation", workflow_command("validate-external-validation")),
            (
                "validate-public-conformance-reporting",
                workflow_command("validate-public-conformance-reporting"),
            ),
            (
                "validate-performance-governance",
                workflow_command("validate-performance-governance"),
            ),
            ("validate-release-foundation", workflow_command("validate-release-foundation")),
            ("validate-packaging-channels", workflow_command("validate-packaging-channels")),
            ("validate-release-operations", workflow_command("validate-release-operations")),
            (
                "validate-distribution-credibility",
                workflow_command("validate-distribution-credibility"),
            ),
            (
                "test-recovery",
                [
                    PWSH,
                    "-NoProfile",
                    "-ExecutionPolicy",
                    "Bypass",
                    "-File",
                    str(RECOVERY_PS1),
                ],
            ),
            (
                "test-fixture-matrix",
                [
                    PWSH,
                    "-NoProfile",
                    "-ExecutionPolicy",
                    "Bypass",
                    "-File",
                    str(MATRIX_PS1),
                ],
            ),
            (
                "test-negative-expectations",
                [
                    PWSH,
                    "-NoProfile",
                    "-ExecutionPolicy",
                    "Bypass",
                    "-File",
                    str(NEGATIVE_EXPECTATIONS_PS1),
                ],
            ),
        ],
    )
