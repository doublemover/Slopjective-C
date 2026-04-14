"""Native build and fixture compilation facade for runtime acceptance."""

from .core import (
    compile_command,
    compile_fixture,
    compile_fixture_expect_failure,
    compile_fixture_manifest_only,
    compile_fixture_outputs,
    compile_fixture_outputs_with_args,
    compile_fixture_with_args,
    compile_negative_diagnostic_batch,
    ensure_native_binaries,
    find_clangxx,
    link_fixture_executable,
    run_fixture_compile,
)

__all__ = [
    "compile_command",
    "compile_fixture",
    "compile_fixture_expect_failure",
    "compile_fixture_manifest_only",
    "compile_fixture_outputs",
    "compile_fixture_outputs_with_args",
    "compile_fixture_with_args",
    "compile_negative_diagnostic_batch",
    "ensure_native_binaries",
    "find_clangxx",
    "link_fixture_executable",
    "run_fixture_compile",
]
