"""Public fixture compile entrypoints for runtime acceptance."""

from __future__ import annotations

from .fixture_compile_runner import run_fixture_compile
from .fixture_negative_diagnostics import NegativeDiagnosticExpectation
from .fixture_negative_diagnostics import compile_fixture_expect_failure
from .fixture_negative_diagnostics import compile_negative_diagnostic_batch
from .fixture_output_contracts import compile_fixture
from .fixture_output_contracts import compile_fixture_manifest_only
from .fixture_output_contracts import compile_fixture_outputs
from .fixture_output_contracts import compile_fixture_outputs_with_args
from .fixture_output_contracts import compile_fixture_with_args
from .fixture_output_contracts import compile_live_error_runtime_fixture_outputs


__all__ = [
    "NegativeDiagnosticExpectation",
    "compile_fixture",
    "compile_fixture_expect_failure",
    "compile_fixture_manifest_only",
    "compile_live_error_runtime_fixture_outputs",
    "compile_fixture_outputs",
    "compile_fixture_outputs_with_args",
    "compile_fixture_with_args",
    "compile_negative_diagnostic_batch",
    "run_fixture_compile",
]
