"""Runtime performance action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

PERFORMANCE_RUNTIME_ACTION_SPECS: dict[str, ActionSpec] = {
    "benchmark-runtime-performance": ActionSpec(
        "benchmark-runtime-performance",
        "measure the live runtime startup dispatch reflection and ownership hot paths and write reproducible telemetry packets",
        "python:scripts/benchmark_objc3c_runtime_performance.py",
        validation_tier="repo",
        guarantee_owner=(
            "runtime hot-path telemetry stays tied to the live runtime acceptance probes "
            "and counter snapshots"
        ),
        pass_through_args=True,
    ),
    "validate-runtime-performance": ActionSpec(
        "validate-runtime-performance",
        "run the integrated runtime hot-path benchmark and packaged validation flow",
        "python:scripts/check_objc3c_runtime_performance_integration.py",
        validation_tier="repo",
        guarantee_owner=(
            "runtime hot-path benchmark outputs stay executable across the live runtime "
            "probes and the staged runnable bundle"
        ),
    ),
    "validate-runnable-runtime-performance": ActionSpec(
        "validate-runnable-runtime-performance",
        "validate the staged runnable runtime-performance surface end to end from the package root",
        "python:scripts/check_objc3c_runnable_runtime_performance_end_to_end.py",
        validation_tier="full",
        guarantee_owner=(
            "packaged runtime-performance fixtures, contracts, and benchmark command "
            "surfaces stay reproducible from the staged runnable toolchain bundle"
        ),
    ),
}
