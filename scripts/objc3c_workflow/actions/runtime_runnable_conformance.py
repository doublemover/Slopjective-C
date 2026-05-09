"""Runnable runtime conformance workflow actions."""

from __future__ import annotations

from ..environment import ROOT
from .runtime_test_acceptance import run_python_check

RUNNABLE_BLOCK_ARC_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_block_arc_conformance.py"
)
RUNNABLE_CONCURRENCY_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_concurrency_conformance.py"
)
RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_object_model_conformance.py"
)
RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_storage_reflection_conformance.py"
)
RUNNABLE_ERROR_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_error_conformance.py"
)
RUNNABLE_INTEROP_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_interop_conformance.py"
)
RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_metaprogramming_conformance.py"
)
RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_release_candidate_conformance.py"
)


def action_validate_block_arc_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_BLOCK_ARC_CONFORMANCE_PY)


def action_validate_concurrency_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_CONCURRENCY_CONFORMANCE_PY)


def action_validate_object_model_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY)


def action_validate_storage_reflection_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY)


def action_validate_error_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_ERROR_CONFORMANCE_PY)


def action_validate_interop_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_INTEROP_CONFORMANCE_PY)


def action_validate_metaprogramming_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY)


def action_validate_release_candidate_conformance(_: list[str]) -> int:
    return run_python_check(RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_PY)
