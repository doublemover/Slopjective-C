"""Stress and fuzz action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

STRESS_ACTION_SPECS: dict[str, ActionSpec] = {
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
    "test-lowering-runtime-stress": ActionSpec(
        "test-lowering-runtime-stress",
        "run the bounded lowering/runtime stress harness over checked-in fixtures",
        "python:scripts/run_objc3c_lowering_runtime_stress.py",
        validation_tier="repo",
        guarantee_owner=(
            "lowering-heavy compile paths and execution-smoke subsets stay runnable "
            "through the live compiler and runtime path"
        ),
        pass_through_args=True,
    ),
    "test-mixed-module-differential": ActionSpec(
        "test-mixed-module-differential",
        "run mixed-module and import/export differential stress validation",
        "python:scripts/run_objc3c_mixed_module_differential.py",
        validation_tier="repo",
        guarantee_owner=(
            "provider-consumer import/export and mixed-image surfaces stay executable "
            "on the live runtime acceptance path"
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
