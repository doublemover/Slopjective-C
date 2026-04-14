"""Fixture helpers for runtime acceptance."""

from .native_build import (  # re-exported as a stable fixture-oriented boundary
    compile_fixture,
    compile_fixture_expect_failure,
    compile_fixture_manifest_only,
    compile_fixture_outputs,
    compile_fixture_outputs_with_args,
    compile_fixture_with_args,
    compile_negative_diagnostic_batch,
    link_fixture_executable,
    run_fixture_compile,
)

__all__ = [
    "compile_fixture",
    "compile_fixture_expect_failure",
    "compile_fixture_manifest_only",
    "compile_fixture_outputs",
    "compile_fixture_outputs_with_args",
    "compile_fixture_with_args",
    "compile_negative_diagnostic_batch",
    "link_fixture_executable",
    "run_fixture_compile",
]
