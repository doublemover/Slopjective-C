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
from scripts.objc3c_workflow.public_command_api import public_workflow_command
from objc3c_tooling.subprocesses import run_capture


ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "compiler_throughput" / "source_surface.json"
WORKLOAD_MANIFEST = ROOT / "tests" / "tooling" / "fixtures" / "compiler_throughput" / "workload_manifest.json"
VALIDATION_TIER_MAP = ROOT / "tests" / "tooling" / "fixtures" / "compiler_throughput" / "validation_tier_map.json"
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
    expect(SOURCE_SURFACE.is_file(), f"missing source surface: {repo_rel(SOURCE_SURFACE)}", failures)
    expect(WORKLOAD_MANIFEST.is_file(), f"missing workload manifest: {repo_rel(WORKLOAD_MANIFEST)}", failures)
    expect(VALIDATION_TIER_MAP.is_file(), f"missing validation tier map: {repo_rel(VALIDATION_TIER_MAP)}", failures)
    expect(ARTIFACT_SURFACE.is_file(), f"missing artifact surface: {repo_rel(ARTIFACT_SURFACE)}", failures)
    expect(SUMMARY.is_file(), f"missing summary report: {repo_rel(SUMMARY)}", failures)

    source_surface = load_json(SOURCE_SURFACE) if SOURCE_SURFACE.is_file() else {}
    workload_manifest = load_json(WORKLOAD_MANIFEST) if WORKLOAD_MANIFEST.is_file() else {}
    validation_tier_map = load_json(VALIDATION_TIER_MAP) if VALIDATION_TIER_MAP.is_file() else {}
    artifact_surface = load_json(ARTIFACT_SURFACE) if ARTIFACT_SURFACE.is_file() else {}
    summary = load_json(SUMMARY) if SUMMARY.is_file() else {}

    expect(
        source_surface.get("contract_id") == "objc3c.compiler.throughput.source.surface.v1",
        "unexpected compiler-throughput source surface contract id",
        failures,
    )
    expect(
        workload_manifest.get("contract_id") == "objc3c.compiler.throughput.workload.manifest.v1",
        "unexpected compiler-throughput workload manifest contract id",
        failures,
    )
    expect(
        validation_tier_map.get("contract_id") == "objc3c.compiler.throughput.validation.tier.map.v1",
        "unexpected compiler-throughput validation tier map contract id",
        failures,
    )
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
    workload_actions = [
        str(workload.get("workflow_action"))
        for workload in workload_manifest.get("workloads", [])
        if isinstance(workload, dict)
    ]
    tier_actions = [
        str(tier.get("public_action"))
        for tier in validation_tier_map.get("tiers", [])
        if isinstance(tier, dict)
    ]
    for required_action in ("compile-objc3c", "test-nightly"):
        expect(
            required_action in workload_actions or required_action in tier_actions,
            f"compiler-throughput ownership no longer covers {required_action}",
            failures,
        )

    payload = {
        "contract_id": "objc3c.compiler.throughput.integration.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/check_objc3c_compiler_throughput_integration.py",
        "source_surface_path": repo_rel(SOURCE_SURFACE),
        "workload_manifest_path": repo_rel(WORKLOAD_MANIFEST),
        "validation_tier_map_path": repo_rel(VALIDATION_TIER_MAP),
        "artifact_surface_path": repo_rel(ARTIFACT_SURFACE),
        "child_report_paths": [repo_rel(SUMMARY)],
        "workload_actions": workload_actions,
        "validation_tier_actions": tier_actions,
        "required_summary_fields": artifact_surface.get("required_summary_fields", []),
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
