"""Runnable runtime end-to-end workflow actions."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_test_acceptance import run_python_check

RUNNABLE_BOOTSTRAP_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_bootstrap_end_to_end.py"
)
RUNNABLE_BLOCK_ARC_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_block_arc_end_to_end.py"
)
RUNNABLE_CONCURRENCY_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_concurrency_end_to_end.py"
)
RUNNABLE_OBJECT_MODEL_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_object_model_end_to_end.py"
)
RUNNABLE_STORAGE_REFLECTION_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_storage_reflection_end_to_end.py"
)
RUNNABLE_ERROR_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_error_end_to_end.py"
RUNNABLE_INTEROP_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_interop_end_to_end.py"
RUNNABLE_METAPROGRAMMING_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_metaprogramming_end_to_end.py"
)
RUNNABLE_RELEASE_CANDIDATE_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_release_candidate_end_to_end.py"
)


def action_validate_runnable_bootstrap(_: list[str]) -> int:
    return run_python_check(RUNNABLE_BOOTSTRAP_E2E_PY)


def action_validate_runnable_block_arc(_: list[str]) -> int:
    return run_python_check(RUNNABLE_BLOCK_ARC_E2E_PY)


def action_validate_runnable_concurrency(_: list[str]) -> int:
    return run_python_check(RUNNABLE_CONCURRENCY_E2E_PY)


def action_validate_runnable_object_model(_: list[str]) -> int:
    return run_python_check(RUNNABLE_OBJECT_MODEL_E2E_PY)


def action_validate_runnable_storage_reflection(_: list[str]) -> int:
    return run_python_check(RUNNABLE_STORAGE_REFLECTION_E2E_PY)


def action_validate_runnable_error(_: list[str]) -> int:
    return run_python_check(RUNNABLE_ERROR_E2E_PY)


def action_validate_runnable_interop(_: list[str]) -> int:
    return run_python_check(RUNNABLE_INTEROP_E2E_PY)


def action_validate_runnable_metaprogramming(_: list[str]) -> int:
    return run_python_check(RUNNABLE_METAPROGRAMMING_E2E_PY)


def action_validate_runnable_release_candidate(_: list[str]) -> int:
    return run_python_check(RUNNABLE_RELEASE_CANDIDATE_E2E_PY)
