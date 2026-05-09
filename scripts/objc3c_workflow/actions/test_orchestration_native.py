"""Focused native execution and fixture test orchestration actions."""

from __future__ import annotations

import sys

from ..commands import pwsh_file, run
from .hosted_llvm_summary import hosted_llc_object_emission_available
from .test_orchestration_paths import (
    BEHAVIOR_MATRIX_PY,
    COMPILE_WRAPPER_SELF_AUDIT_PY,
    LLVM_CAPABILITY_ROUTING_TESTS,
    MATRIX_PS1,
    NEGATIVE_EXPECTATIONS_PS1,
    RECOVERY_PS1,
    REPLAY_PS1,
    SMOKE_PS1,
)


def action_test_behavior_matrix(_: list[str]) -> int:
    return run([sys.executable, str(BEHAVIOR_MATRIX_PY)])


def action_test_recovery(rest: list[str]) -> int:
    return pwsh_file(RECOVERY_PS1, *rest)


def action_test_execution_smoke(rest: list[str]) -> int:
    return pwsh_file(SMOKE_PS1, *rest)


def action_test_hosted_execution_smoke(_: list[str]) -> int:
    if not hosted_llc_object_emission_available():
        print(
            "Skipping execution smoke: hosted runner does not provide "
            "llc --filetype=obj capability."
        )
        return 0
    return pwsh_file(SMOKE_PS1)


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
