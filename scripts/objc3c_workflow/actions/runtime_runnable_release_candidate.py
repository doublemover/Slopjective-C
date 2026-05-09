"""Runnable release-candidate workflow action owner."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_test_acceptance import run_python_check

RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_release_candidate_conformance.py"
)
RUNNABLE_RELEASE_CANDIDATE_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_release_candidate_end_to_end.py"
)


def action_validate_release_candidate_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_PY)


def action_validate_runnable_release_candidate(_: list[str]) -> int:
    return run_python_check(RUNNABLE_RELEASE_CANDIDATE_E2E_PY)
