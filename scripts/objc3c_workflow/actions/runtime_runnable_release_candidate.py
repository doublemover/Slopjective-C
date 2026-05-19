"""Runnable release-candidate workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_runnable_groups import RuntimeRunnableActionGroup
from .runtime_test_acceptance import run_python_check

RUNNABLE_RELEASE_CANDIDATE_ROUTE = "release-candidate"
VALIDATE_RELEASE_CANDIDATE_CONFORMANCE_ACTION = (
    f"validate-{RUNNABLE_RELEASE_CANDIDATE_ROUTE}-conformance"
)
VALIDATE_RUNNABLE_RELEASE_CANDIDATE_ACTION = (
    f"validate-runnable-{RUNNABLE_RELEASE_CANDIDATE_ROUTE}"
)

RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_SCRIPT = (
    "scripts/check_objc3c_runnable_release_candidate_conformance.py"
)
RUNNABLE_RELEASE_CANDIDATE_E2E_SCRIPT = (
    "scripts/check_objc3c_runnable_release_candidate_end_to_end.py"
)
RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_BACKEND = (
    f"python:{RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_SCRIPT}"
)
RUNNABLE_RELEASE_CANDIDATE_E2E_BACKEND = (
    f"python:{RUNNABLE_RELEASE_CANDIDATE_E2E_SCRIPT}"
)
RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_PY = (
    ROOT / RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_SCRIPT
)
RUNNABLE_RELEASE_CANDIDATE_E2E_PY = ROOT / RUNNABLE_RELEASE_CANDIDATE_E2E_SCRIPT


def action_validate_release_candidate_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_PY)


def action_validate_runnable_release_candidate(_: list[str]) -> int:
    return run_python_check(RUNNABLE_RELEASE_CANDIDATE_E2E_PY)


RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_RELEASE_CANDIDATE_CONFORMANCE_ACTION,
    summary=(
        "validate runnable release-candidate conformance across the integrated live "
        "workflow"
    ),
    backend=RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_BACKEND,
    handler=action_validate_release_candidate_conformance,
    guarantee_owner=(
        "integrated public-claims strict-profile and release-candidate conformance "
        "over the live runtime architecture workflow"
    ),
)
RUNNABLE_RELEASE_CANDIDATE_E2E_ACTION = RuntimeRunnableActionGroup(
    action=VALIDATE_RUNNABLE_RELEASE_CANDIDATE_ACTION,
    summary=(
        "validate runnable release-candidate packaging and validation end to end "
        "from the package root"
    ),
    backend=RUNNABLE_RELEASE_CANDIDATE_E2E_BACKEND,
    handler=action_validate_runnable_release_candidate,
    guarantee_owner=(
        "packaged compile, release-candidate validation, runtime probe execution, "
        "smoke, and replay from the staged runnable toolchain bundle"
    ),
)


__all__ = [
    "RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_ACTION",
    "RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_BACKEND",
    "RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_PY",
    "RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_SCRIPT",
    "RUNNABLE_RELEASE_CANDIDATE_E2E_ACTION",
    "RUNNABLE_RELEASE_CANDIDATE_E2E_BACKEND",
    "RUNNABLE_RELEASE_CANDIDATE_E2E_PY",
    "RUNNABLE_RELEASE_CANDIDATE_E2E_SCRIPT",
    "RUNNABLE_RELEASE_CANDIDATE_ROUTE",
    "VALIDATE_RELEASE_CANDIDATE_CONFORMANCE_ACTION",
    "VALIDATE_RUNNABLE_RELEASE_CANDIDATE_ACTION",
    "action_validate_release_candidate_conformance",
    "action_validate_runnable_release_candidate",
]
