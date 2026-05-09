"""Compiler-throughput performance action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

PERFORMANCE_THROUGHPUT_ACTION_SPECS: dict[str, ActionSpec] = {
    "benchmark-compiler-throughput": ActionSpec(
        "benchmark-compiler-throughput",
        "measure direct native compile throughput, wrapper cache proof, cache invalidation, macro-host cache publication, and docs-generation cost",
        "pwsh:scripts/check_objc3c_native_perf_budget.ps1",
        validation_tier="repo",
        guarantee_owner=(
            "compiler-throughput telemetry stays tied to the live native compiler "
            "executable, wrapper cache contract, macro-host artifact path, and checked-in "
            "docs generators"
        ),
        pass_through_args=True,
    ),
    "validate-compiler-throughput": ActionSpec(
        "validate-compiler-throughput",
        "run the integrated compiler-throughput benchmark and cache-proof validation flow",
        "python:scripts/check_objc3c_compiler_throughput_integration.py",
        validation_tier="repo",
        guarantee_owner=(
            "compiler-throughput benchmark outputs stay executable across the live "
            "native compiler, wrapper cache proof, macro-host artifact path, and docs "
            "generators"
        ),
    ),
    "validate-runnable-compiler-throughput": ActionSpec(
        "validate-runnable-compiler-throughput",
        "validate the staged runnable compiler-throughput surface end to end from the package root",
        "python:scripts/check_objc3c_runnable_compiler_throughput_end_to_end.py",
        validation_tier="full",
        guarantee_owner=(
            "packaged compiler-throughput fixtures, contracts, benchmark script, and "
            "command surfaces stay reproducible from the staged runnable toolchain bundle"
        ),
    ),
}
