"""Stress source-surface, fuzz, reducer, and triage action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

STRESS_SOURCE_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-stress-surface": ActionSpec(
        "check-stress-surface",
        "validate the checked-in fuzz, stress, minimization, and triage source surface",
        "python:scripts/check_stress_source_surface.py",
        validation_tier="repo",
        guarantee_owner=(
            "stress source roots, machine-owned artifact boundaries, and checked-in "
            "minimization contracts stay explicit and coherent"
        ),
    ),
    "test-fuzz-safety": ActionSpec(
        "test-fuzz-safety",
        "run the deterministic malformed-input parser/sema stress gate",
        "python:scripts/run_objc3c_fuzz_safety.py",
        validation_tier="repo",
        guarantee_owner=(
            "malformed parser/sema inputs stay fail-closed and deterministic through "
            "the live compiler path"
        ),
        pass_through_args=True,
    ),
    "test-stress-minimization": ActionSpec(
        "test-stress-minimization",
        "reduce failing checked-in malformed inputs into machine-owned replay capsules",
        "python:scripts/run_objc3c_stress_minimization.py",
        validation_tier="repo",
        guarantee_owner=(
            "deterministic reducer output stays tied to checked-in failing inputs and "
            "live compiler signatures"
        ),
        pass_through_args=True,
    ),
    "test-stress-crash-triage": ActionSpec(
        "test-stress-crash-triage",
        "build crash-signature triage indexes and replay requests from minimized stress cases",
        "python:scripts/run_objc3c_stress_crash_triage.py",
        validation_tier="repo",
        guarantee_owner=(
            "crash-signature grouping and replay requests stay derived from machine-owned "
            "minimized stress artifacts"
        ),
        pass_through_args=True,
    ),
}

__all__ = ["STRESS_SOURCE_ACTION_SPECS"]
