"""Integrated stress validation action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

STRESS_VALIDATION_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-stress": ActionSpec(
        "validate-stress",
        "run the integrated fuzz, stress, reducer, and crash-triage workflow",
        "runner-internal stress validation child actions",
        validation_tier="repo",
        guarantee_owner=(
            "checked-in stress roots, deterministic malformed-input coverage, mixed-module "
            "differential validation, reducer output, and crash-triage indexes stay "
            "executable on the live workflow"
        ),
    ),
    "validate-stress-integration": ActionSpec(
        "validate-stress-integration",
        "validate the integrated stress workflow report and child artifact set",
        "python:scripts/check_objc3c_stress_integration.py",
        validation_tier="repo",
        guarantee_owner=(
            "integrated stress workflow reports stay coherent across source-surface, "
            "differential, minimization, and crash-triage outputs"
        ),
    ),
    "validate-stress-end-to-end": ActionSpec(
        "validate-stress-end-to-end",
        "validate the public end-to-end stress workflow, command appendix, and nightly wiring",
        "python:scripts/check_objc3c_stress_end_to_end.py",
        validation_tier="repo",
        guarantee_owner=(
            "public stress workflow entrypoints and nightly wiring stay coherent with "
            "the integrated stress report set"
        ),
    ),
}

__all__ = ["STRESS_VALIDATION_ACTION_SPECS"]
