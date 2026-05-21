#!/usr/bin/env python3
"""Validate the integrated runtime-performance benchmark surface."""

from __future__ import annotations

import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from scripts.objc3c_workflow.public_command_api import public_workflow_command
from objc3c_tooling.subprocesses import run_capture


SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "runtime_performance" / "source_surface.json"
WORKLOAD_MANIFEST = ROOT / "tests" / "tooling" / "fixtures" / "runtime_performance" / "workload_manifest.json"
ARTIFACT_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "runtime_performance" / "artifact_surface.json"
BENCHMARK_SUMMARY = ROOT / "tmp" / "reports" / "runtime-performance" / "benchmark-summary.json"
RUNNABLE_SUMMARY = ROOT / "tmp" / "reports" / "runtime-performance" / "runnable-end-to-end-summary.json"
SCALE_EVIDENCE_SUMMARY = ROOT / "tmp" / "reports" / "runtime-performance" / "scale-evidence-summary.json"
SCALE_EVIDENCE_PROBE = ROOT / "scripts" / "probe_objc3c_runtime_scale_evidence.py"
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime-performance" / "integration-summary.json"






def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    source_surface = load_json(SOURCE_SURFACE)
    manifest = load_json(WORKLOAD_MANIFEST)
    artifact_surface = load_json(ARTIFACT_SURFACE)
    expected_workload_ids = [
        str(row["workload_id"])
        for row in manifest.get("workload_families", [])
        if isinstance(row, dict) and row.get("workload_id")
    ]

    steps = [
        (
            "benchmark-runtime-performance",
            run_capture(
                public_workflow_command(
                    "benchmark-runtime-performance",
                    "--warmup-runs",
                    "0",
                    "--measured-runs",
                    "1",
                )
            ),
        ),
        (
            "validate-runnable-runtime-performance",
            run_capture(
                public_workflow_command("validate-runnable-runtime-performance")
            ),
        ),
        (
            "probe-runtime-scale-evidence",
            run_capture([sys.executable, str(SCALE_EVIDENCE_PROBE)]),
        ),
    ]

    failures: list[str] = []
    for name, result in steps:
        expect(result.returncode == 0, f"{name} failed", failures)

    expect(
        source_surface.get("contract_id") == "objc3c.runtime.performance.source.surface.v1",
        "unexpected runtime-performance source surface contract id",
        failures,
    )
    expect(
        artifact_surface.get("contract_id") == "objc3c.runtime.performance.artifact.surface.v1",
        "unexpected runtime-performance artifact surface contract id",
        failures,
    )
    expect(BENCHMARK_SUMMARY.is_file(), f"missing runtime benchmark summary: {repo_rel(BENCHMARK_SUMMARY)}", failures)
    expect(RUNNABLE_SUMMARY.is_file(), f"missing runnable runtime benchmark summary: {repo_rel(RUNNABLE_SUMMARY)}", failures)
    expect(SCALE_EVIDENCE_SUMMARY.is_file(), f"missing runtime scale evidence summary: {repo_rel(SCALE_EVIDENCE_SUMMARY)}", failures)

    benchmark_summary = load_json(BENCHMARK_SUMMARY) if BENCHMARK_SUMMARY.is_file() else {}
    runnable_summary = load_json(RUNNABLE_SUMMARY) if RUNNABLE_SUMMARY.is_file() else {}
    scale_evidence_summary = load_json(SCALE_EVIDENCE_SUMMARY) if SCALE_EVIDENCE_SUMMARY.is_file() else {}

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
    packet_payloads: list[dict[str, Any]] = []
    for packet_path in packet_paths:
        if isinstance(packet_path, str):
            expect((ROOT / packet_path).is_file(), f"runtime benchmark packet missing on disk: {packet_path}", failures)
            if (ROOT / packet_path).is_file():
                packet_payloads.append(load_json(ROOT / packet_path))
    for required_field in artifact_surface.get("required_packet_fields", []):
        for packet in packet_payloads:
            expect(
                required_field in packet,
                f"runtime telemetry packet missing required field '{required_field}'",
                failures,
            )
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
    expect(
        scale_evidence_summary.get("contract_id") == "objc3c.runtime.performance.scale.evidence.summary.v1",
        "unexpected runtime scale evidence summary contract id",
        failures,
    )
    expect(
        scale_evidence_summary.get("status") == "PASS",
        "runtime scale evidence summary did not pass",
        failures,
    )
    expect(
        scale_evidence_summary.get("support_authority") is False,
        "runtime scale evidence summary must remain replay evidence, not support authority",
        failures,
    )
    scale_counts = scale_evidence_summary.get("summary_counts", {})
    if not isinstance(scale_counts, dict):
        scale_counts = {}
    expect(
        int(scale_counts.get("stress_scale", 0)) >= 8,
        "runtime scale evidence summary did not preserve the checked stress scale floor",
        failures,
    )
    expect(
        int(scale_counts.get("scale_scenarios", 0)) >= 4,
        "runtime scale evidence summary did not preserve the checked scale scenario floor",
        failures,
    )

    payload = {
        "contract_id": "objc3c.runtime.performance.integration.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/check_objc3c_runtime_performance_integration.py",
        "source_surface_path": repo_rel(SOURCE_SURFACE),
        "workload_manifest_path": repo_rel(WORKLOAD_MANIFEST),
        "artifact_surface_path": repo_rel(ARTIFACT_SURFACE),
        "child_report_paths": [
            repo_rel(BENCHMARK_SUMMARY),
            repo_rel(RUNNABLE_SUMMARY),
            repo_rel(SCALE_EVIDENCE_SUMMARY),
        ],
        "expected_workload_ids": expected_workload_ids,
        "hot_path_families": source_surface.get("hot_path_families", []),
        "required_packet_fields": artifact_surface.get("required_packet_fields", []),
        "failures": failures,
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
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
