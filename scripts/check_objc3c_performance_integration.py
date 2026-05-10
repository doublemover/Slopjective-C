#!/usr/bin/env python3
"""Validate the integrated objc3 and comparative benchmark foundation surface."""

from __future__ import annotations

import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Sequence
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from scripts.objc3c_workflow.public_command_api import public_workflow_command
from objc3c_tooling.subprocesses import run_capture


ROOT = Path(__file__).resolve().parents[1]
PERFORMANCE_SUMMARY = ROOT / "tmp" / "reports" / "performance" / "benchmark-summary.json"
COMPARATIVE_SUMMARY = ROOT / "tmp" / "reports" / "performance" / "comparative-baselines-summary.json"
RUNNABLE_SUMMARY = ROOT / "tmp" / "reports" / "performance" / "runnable-end-to-end-summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "performance" / "integration-summary.json"






def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    steps = [
        (
            "benchmark-performance",
            run_capture(
                public_workflow_command(
                    "benchmark-performance",
                    "--warmup-runs",
                    "0",
                    "--measured-runs",
                    "1",
                )
            ),
        ),
        (
            "benchmark-comparative-baselines",
            run_capture(
                public_workflow_command(
                    "benchmark-comparative-baselines",
                    "--warmup-runs",
                    "0",
                    "--measured-runs",
                    "1",
                )
            ),
        ),
        (
            "validate-runnable-performance",
            run_capture(public_workflow_command("validate-runnable-performance")),
        ),
    ]

    failures: list[str] = []
    for name, result in steps:
        expect(result.returncode == 0, f"{name} failed", failures)

    expect(PERFORMANCE_SUMMARY.is_file(), f"missing benchmark summary: {repo_rel(PERFORMANCE_SUMMARY)}", failures)
    expect(COMPARATIVE_SUMMARY.is_file(), f"missing comparative summary: {repo_rel(COMPARATIVE_SUMMARY)}", failures)
    expect(RUNNABLE_SUMMARY.is_file(), f"missing runnable benchmark summary: {repo_rel(RUNNABLE_SUMMARY)}", failures)

    performance_summary = load_json(PERFORMANCE_SUMMARY) if PERFORMANCE_SUMMARY.is_file() else {}
    comparative_summary = load_json(COMPARATIVE_SUMMARY) if COMPARATIVE_SUMMARY.is_file() else {}
    runnable_summary = load_json(RUNNABLE_SUMMARY) if RUNNABLE_SUMMARY.is_file() else {}

    expect(
        performance_summary.get("contract_id") == "objc3c.performance.benchmark.summary.v1",
        "unexpected objc3 benchmark summary contract id",
        failures,
    )
    expect(
        comparative_summary.get("contract_id") == "objc3c.performance.comparative.baselines.summary.v1",
        "unexpected comparative benchmark summary contract id",
        failures,
    )
    expect(
        runnable_summary.get("contract_id") == "objc3c.performance.runnable.end.to.end.summary.v1",
        "unexpected runnable benchmark summary contract id",
        failures,
    )

    payload = {
        "contract_id": "objc3c.performance.integration.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/check_objc3c_performance_integration.py",
        "child_report_paths": [
            repo_rel(PERFORMANCE_SUMMARY),
            repo_rel(COMPARATIVE_SUMMARY),
            repo_rel(RUNNABLE_SUMMARY),
        ],
        "failures": failures,
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    if failures:
        print("objc3c-performance-integration: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-performance-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
