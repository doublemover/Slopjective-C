"""Packaged and comparative performance action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

PERFORMANCE_PACKAGING_ACTION_SPECS: dict[str, ActionSpec] = {
    "benchmark-comparative-baselines": ActionSpec(
        "benchmark-comparative-baselines",
        "measure the checked-in ObjC2 Swift and C++ baseline workloads and write reproducible comparison telemetry packets",
        "python:scripts/run_objc3c_comparative_baselines.py",
        validation_tier="repo",
        guarantee_owner=(
            "comparative baseline telemetry stays tied to checked-in language fixtures "
            "and recorded availability states"
        ),
        pass_through_args=True,
    ),
    "validate-runnable-performance": ActionSpec(
        "validate-runnable-performance",
        "validate the staged runnable toolchain performance surface end to end from the package root",
        "python:scripts/check_objc3c_runnable_performance_end_to_end.py",
        validation_tier="full",
        guarantee_owner=(
            "packaged benchmark fixtures, schemas, and benchmark command surfaces stay "
            "reproducible from the staged runnable toolchain bundle"
        ),
    ),
    "validate-performance-foundation": ActionSpec(
        "validate-performance-foundation",
        "run the integrated benchmark and comparative baseline validation flow",
        "python:scripts/check_objc3c_performance_integration.py",
        validation_tier="repo",
        guarantee_owner=(
            "benchmark foundations stay executable across live objc3 workloads, "
            "comparative baselines, and the staged runnable bundle"
        ),
    ),
}
