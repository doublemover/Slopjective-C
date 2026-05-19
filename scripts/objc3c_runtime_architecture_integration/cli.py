"""CLI orchestration for runtime architecture integration validation."""

from __future__ import annotations

from .fixtures import run_runtime_architecture_inputs
from .rendering import render_summary_path, write_integration_summary
from .summary import build_integration_summary
from .validation_checks import validate_runtime_architecture_reports


def main() -> int:
    run_runtime_architecture_inputs()
    validated = validate_runtime_architecture_reports()
    payload = build_integration_summary(validated)
    summary_path = write_integration_summary(payload)
    render_summary_path(summary_path)
    return 0
