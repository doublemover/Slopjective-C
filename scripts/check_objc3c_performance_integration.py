#!/usr/bin/env python3
"""Validate the integrated objc3 and comparative benchmark foundation surface."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Sequence


ROOT = Path(__file__).resolve().parents[1]
PUBLIC_RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
PERFORMANCE_SUMMARY = ROOT / "tmp" / "reports" / "performance" / "benchmark-summary.json"
COMPARATIVE_SUMMARY = ROOT / "tmp" / "reports" / "performance" / "comparative-baselines-summary.json"
RUNNABLE_SUMMARY = ROOT / "tmp" / "reports" / "performance" / "runnable-end-to-end-summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "performance" / "integration-summary.json"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        list(command),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def load_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    steps = [
        (
            "benchmark-performance",
            run_capture(
                [
                    sys.executable,
                    str(PUBLIC_RUNNER),
                    "benchmark-performance",
                    "--warmup-runs",
                    "0",
                    "--measured-runs",
                    "1",
                ]
            ),
        ),
        (
            "benchmark-comparative-baselines",
            run_capture(
                [
                    sys.executable,
                    str(PUBLIC_RUNNER),
                    "benchmark-comparative-baselines",
                    "--warmup-runs",
                    "0",
                    "--measured-runs",
                    "1",
                ]
            ),
        ),
        (
            "validate-runnable-performance",
            run_capture([sys.executable, str(PUBLIC_RUNNER), "validate-runnable-performance"]),
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
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
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
