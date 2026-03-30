#!/usr/bin/env python3
"""Validate the integrated runtime-performance benchmark surface."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
PUBLIC_RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
WORKLOAD_MANIFEST = ROOT / "tests" / "tooling" / "fixtures" / "runtime_performance" / "workload_manifest.json"
BENCHMARK_SUMMARY = ROOT / "tmp" / "reports" / "runtime-performance" / "benchmark-summary.json"
RUNNABLE_SUMMARY = ROOT / "tmp" / "reports" / "runtime-performance" / "runnable-end-to-end-summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime-performance" / "integration-summary.json"


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


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    manifest = load_json(WORKLOAD_MANIFEST)
    expected_workload_ids = [
        str(row["workload_id"])
        for row in manifest.get("workload_families", [])
        if isinstance(row, dict) and row.get("workload_id")
    ]

    steps = [
        (
            "benchmark-runtime-performance",
            run_capture(
                [
                    sys.executable,
                    str(PUBLIC_RUNNER),
                    "benchmark-runtime-performance",
                    "--warmup-runs",
                    "0",
                    "--measured-runs",
                    "1",
                ]
            ),
        ),
        (
            "validate-runnable-runtime-performance",
            run_capture(
                [sys.executable, str(PUBLIC_RUNNER), "validate-runnable-runtime-performance"]
            ),
        ),
    ]

    failures: list[str] = []
    for name, result in steps:
        expect(result.returncode == 0, f"{name} failed", failures)

    expect(BENCHMARK_SUMMARY.is_file(), f"missing runtime benchmark summary: {repo_rel(BENCHMARK_SUMMARY)}", failures)
    expect(RUNNABLE_SUMMARY.is_file(), f"missing runnable runtime benchmark summary: {repo_rel(RUNNABLE_SUMMARY)}", failures)

    benchmark_summary = load_json(BENCHMARK_SUMMARY) if BENCHMARK_SUMMARY.is_file() else {}
    runnable_summary = load_json(RUNNABLE_SUMMARY) if RUNNABLE_SUMMARY.is_file() else {}

    expect(
        benchmark_summary.get("contract_id") == "objc3c.runtime.performance.summary.v1",
        "unexpected runtime benchmark summary contract id",
        failures,
    )
    expect(
        benchmark_summary.get("workload_manifest_contract_id") == manifest.get("contract_id"),
        "runtime benchmark summary did not preserve workload manifest contract id",
        failures,
    )
    expect(
        benchmark_summary.get("artifact_surface_contract_id")
        == "objc3c.runtime.performance.artifact.surface.v1",
        "runtime benchmark summary did not preserve artifact surface contract id",
        failures,
    )
    expect(
        benchmark_summary.get("selected_workload_ids") == expected_workload_ids,
        "runtime benchmark summary drifted from the checked-in workload IDs",
        failures,
    )
    packet_paths = benchmark_summary.get("packet_paths", [])
    expect(isinstance(packet_paths, list) and len(packet_paths) == len(expected_workload_ids), "runtime benchmark summary did not publish one packet path per workload", failures)
    for packet_path in packet_paths:
        if isinstance(packet_path, str):
            expect((ROOT / packet_path).is_file(), f"runtime benchmark packet missing on disk: {packet_path}", failures)
    workloads = benchmark_summary.get("workloads", [])
    expect(isinstance(workloads, list) and len(workloads) == len(expected_workload_ids), "runtime benchmark summary did not publish one workload summary per workload ID", failures)
    for workload in workloads:
        if not isinstance(workload, dict):
            failures.append("runtime benchmark workload summary was not a JSON object")
            continue
        summary = workload.get("summary", {})
        expect(
            isinstance(summary, dict) and int(summary.get("sample_count", 0)) >= 1,
            f"runtime benchmark workload {workload.get('workload_id', '<unknown>')} did not publish any measured samples",
            failures,
        )

    expect(
        runnable_summary.get("contract_id") == "objc3c.runtime.performance.runnable.end.to.end.summary.v1",
        "unexpected runnable runtime benchmark summary contract id",
        failures,
    )

    payload = {
        "contract_id": "objc3c.runtime.performance.integration.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/check_objc3c_runtime_performance_integration.py",
        "child_report_paths": [
            repo_rel(BENCHMARK_SUMMARY),
            repo_rel(RUNNABLE_SUMMARY),
        ],
        "expected_workload_ids": expected_workload_ids,
        "failures": failures,
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    if failures:
        print("objc3c-runtime-performance-integration: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-runtime-performance-integration: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
