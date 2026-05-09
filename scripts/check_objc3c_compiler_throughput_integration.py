#!/usr/bin/env python3
"""Validate the integrated compiler-throughput benchmark and cache-proof surface."""

from __future__ import annotations

import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.public_runner import public_workflow_command
from objc3c_tooling.subprocesses import run_capture


ROOT = Path(__file__).resolve().parents[1]
ARTIFACT_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "compiler_throughput" / "artifact_surface.json"
SUMMARY = ROOT / "tmp" / "reports" / "compiler-throughput" / "benchmark-summary.json"
REPORT = ROOT / "tmp" / "reports" / "compiler-throughput" / "integration-summary.json"






def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    step = run_capture(public_workflow_command("benchmark-compiler-throughput"))
    failures: list[str] = []
    expect(step.returncode == 0, "benchmark-compiler-throughput failed", failures)
    expect(ARTIFACT_SURFACE.is_file(), f"missing artifact surface: {repo_rel(ARTIFACT_SURFACE)}", failures)
    expect(SUMMARY.is_file(), f"missing summary report: {repo_rel(SUMMARY)}", failures)

    artifact_surface = load_json(ARTIFACT_SURFACE) if ARTIFACT_SURFACE.is_file() else {}
    summary = load_json(SUMMARY) if SUMMARY.is_file() else {}

    expect(
        artifact_surface.get("contract_id") == "objc3c.compiler.throughput.artifact.surface.v1",
        "unexpected compiler-throughput artifact surface contract id",
        failures,
    )
    expect(
        summary.get("contract_id") == "objc3c.compiler.throughput.summary.v1",
        "unexpected compiler-throughput summary contract id",
        failures,
    )
    expect(
        summary.get("benchmark_kind") == "native-direct-compile-throughput",
        "unexpected compiler-throughput benchmark kind",
        failures,
    )
    for field in artifact_surface.get("required_summary_fields", []):
        expect(field in summary, f"compiler-throughput summary missing required field '{field}'", failures)

    payload = {
        "contract_id": "objc3c.compiler.throughput.integration.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/check_objc3c_compiler_throughput_integration.py",
        "child_report_paths": [repo_rel(SUMMARY)],
        "failures": failures,
    }
    REPORT.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT, payload)
    print(f"summary_path: {repo_rel(REPORT)}")
    if failures:
        print("compiler-throughput-integration: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("compiler-throughput-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
