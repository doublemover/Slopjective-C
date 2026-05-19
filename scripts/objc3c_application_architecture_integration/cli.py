"""CLI orchestration for application architecture integration validation."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.subprocesses import python_script_command

from .contracts import (
    CANONICAL_SUMMARY_PATH,
    CANONICAL_WORKSPACE_PY,
    SHOWCASE_INTEGRATION_PY,
    SHOWCASE_SUMMARY_PATH,
    STDLIB_PROGRAM_INTEGRATION_PY,
    STDLIB_PROGRAM_SUMMARY_PATH,
    TEMPLATE_HARNESS_PY,
    TEMPLATE_SUMMARY_PATH,
)
from .execution import expect, require_executed_summary, summary_passes
from .rendering import render_failures, render_summary_path, write_integration_summary
from .summary import build_integration_payload, build_step_result


def run_integration_step(
    *,
    name: str,
    script_path: Path,
    summary_path: Path,
    failures: list[str],
    step_results: list[dict[str, Any]],
) -> dict[str, Any]:
    command = python_script_command(script_path)
    summary = require_executed_summary(
        name=name,
        command=command,
        summary_path=summary_path,
        failures=failures,
    )
    step_results.append(
        build_step_result(
            name=name,
            command=command,
            summary_path=summary_path,
            summary=summary,
        )
    )
    return summary


def main() -> int:
    failures: list[str] = []
    step_results: list[dict[str, Any]] = []

    template_summary = run_integration_step(
        name="template-harness",
        script_path=TEMPLATE_HARNESS_PY,
        summary_path=TEMPLATE_SUMMARY_PATH,
        failures=failures,
        step_results=step_results,
    )
    canonical_summary = run_integration_step(
        name="canonical-workspace",
        script_path=CANONICAL_WORKSPACE_PY,
        summary_path=CANONICAL_SUMMARY_PATH,
        failures=failures,
        step_results=step_results,
    )
    showcase_summary = run_integration_step(
        name="showcase-integration",
        script_path=SHOWCASE_INTEGRATION_PY,
        summary_path=SHOWCASE_SUMMARY_PATH,
        failures=failures,
        step_results=step_results,
    )
    if not summary_passes(showcase_summary):
        expect(False, "showcase integration summary did not report PASS", failures)

    stdlib_program_summary = run_integration_step(
        name="stdlib-program-integration",
        script_path=STDLIB_PROGRAM_INTEGRATION_PY,
        summary_path=STDLIB_PROGRAM_SUMMARY_PATH,
        failures=failures,
        step_results=step_results,
    )
    if not summary_passes(stdlib_program_summary):
        expect(False, "stdlib program integration summary did not report PASS", failures)

    expect(
        template_summary.get("status") == "PASS",
        "template harness summary did not report PASS",
        failures,
    )
    expect(
        canonical_summary.get("status") == "PASS",
        "canonical application workspace summary did not report PASS",
        failures,
    )
    expect(
        canonical_summary.get("example_count") == 3,
        "canonical application workspace did not include all showcase examples",
        failures,
    )
    expect(
        canonical_summary.get("architecture_layer_count") == 4,
        "canonical application workspace layer count drifted from the canonical contract",
        failures,
    )
    expect(
        showcase_summary.get("contract_id") == "objc3c.showcase.integration.summary.v1",
        "showcase integration summary contract drifted",
        failures,
    )
    expect(
        stdlib_program_summary.get("contract_id") == "objc3c.stdlib.program.integration.summary.v1",
        "stdlib program integration summary contract drifted",
        failures,
    )

    payload = build_integration_payload(failures=failures, step_results=step_results)
    summary_path = write_integration_summary(payload)
    render_summary_path(summary_path)
    if failures:
        render_failures(failures)
        return 1
    print("application-architecture-integration: PASS")
    return 0
