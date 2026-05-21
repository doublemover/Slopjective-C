#!/usr/bin/env python3
"""Validate the staged runnable runtime-performance surface end to end."""

from __future__ import annotations

import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.paths import normalize_rel_path, repo_rel
from objc3c_tooling.json_io import require_json_object as load_json, write_json_file
from objc3c_tooling.subprocesses import run_capture


ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime-performance" / "runnable-end-to-end-summary.json"
PACKAGE_CONTRACT_ID = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"
SUMMARY_CONTRACT_ID = "objc3c.runtime.performance.runnable.end.to.end.summary.v1"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)

def main() -> int:
    run_id = datetime.now().strftime("%H%M%S%f")
    package_root = ROOT / "tmp" / "p" / f"rp{run_id}"
    manifest_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    summary_out = package_root / "tmp" / "reports" / "runtime-performance" / "packaged-benchmark-summary.json"

    package_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(PACKAGE_PS1),
            "-PackageRoot",
            str(package_root),
        ],
        cwd=ROOT,
    )
    if package_result.returncode != 0:
        raise RuntimeError("runnable toolchain package command failed")

    manifest = load_json(manifest_path)
    expect(manifest.get("contract_id") == PACKAGE_CONTRACT_ID, "unexpected package contract id")

    command_surfaces = manifest.get("command_surfaces", {})
    runtime_surface = manifest.get("runtime_performance_surface", {})

    benchmark_script = package_root / "scripts" / "benchmark_objc3c_runtime_performance.py"
    acceptance_script = package_root / "scripts" / "check_objc3c_runtime_acceptance.py"
    workload_manifest = package_root / normalize_rel_path(str(runtime_surface["workload_manifest"]))
    fixture_manifest = package_root / normalize_rel_path(str(runtime_surface["executable_fixture_manifest"]))
    source_surface = package_root / normalize_rel_path(str(runtime_surface["source_surface_contract"]))
    artifact_surface = package_root / normalize_rel_path(str(runtime_surface["artifact_surface_contract"]))
    optimization_policy = package_root / normalize_rel_path(str(runtime_surface["optimization_policy"]))
    workload_replay_contract = package_root / normalize_rel_path(
        str(runtime_surface["workload_replay_contract"])
    )
    metadata_resilience_contract = package_root / normalize_rel_path(
        str(runtime_surface["metadata_resilience_contract"])
    )
    stress_sanitizer_contract = package_root / normalize_rel_path(
        str(runtime_surface["stress_sanitizer_contract"])
    )
    telemetry_schema = package_root / normalize_rel_path(str(runtime_surface["telemetry_schema"]))
    source_readme = package_root / normalize_rel_path(str(runtime_surface["source_readme"]))
    runbook = package_root / normalize_rel_path(str(runtime_surface["runbook"]))

    expect(
        command_surfaces.get("inspect_runtime_performance") == "npm run objc3c -- benchmark-runtime-performance",
        "package manifest missing inspect_runtime_performance command surface",
    )
    expect(
        command_surfaces.get("runtime_performance") == "npm run objc3c -- validate-runtime-performance",
        "package manifest missing runtime_performance command surface",
    )
    expect(
        command_surfaces.get("runtime_performance_e2e") == "npm run objc3c -- validate-runnable-runtime-performance",
        "package manifest missing runtime_performance_e2e command surface",
    )

    for path in (
        benchmark_script,
        acceptance_script,
        workload_manifest,
        fixture_manifest,
        source_surface,
        artifact_surface,
        optimization_policy,
        workload_replay_contract,
        metadata_resilience_contract,
        stress_sanitizer_contract,
        telemetry_schema,
        source_readme,
        runbook,
    ):
        expect(path.is_file(), f"packaged runnable toolchain missing required runtime-performance file {path}")

    benchmark_result = run_capture(
        [
            sys.executable,
            str(benchmark_script),
            "--warmup-runs",
            "0",
            "--measured-runs",
            "1",
            "--fixture-manifest",
            str(fixture_manifest),
            "--summary-out",
            str(summary_out),
        ],
        cwd=package_root,
    )
    if benchmark_result.returncode != 0:
        raise RuntimeError("packaged runtime-performance benchmark failed")

    benchmark_summary = load_json(summary_out)
    workload_payload = load_json(workload_manifest)
    expected_workload_ids = [
        str(row["workload_id"])
        for row in workload_payload.get("workload_families", [])
        if isinstance(row, dict) and row.get("workload_id")
    ]

    expect(
        benchmark_summary.get("contract_id") == "objc3c.runtime.performance.summary.v1",
        "packaged runtime-performance benchmark summary contract drifted",
    )
    expect(
        benchmark_summary.get("selected_workload_ids") == expected_workload_ids,
        "packaged runtime-performance benchmark summary drifted from workload manifest",
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_runtime_performance_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "packaged_command_surfaces": {
            "inspect_runtime_performance": command_surfaces["inspect_runtime_performance"],
            "runtime_performance": command_surfaces["runtime_performance"],
            "runtime_performance_e2e": command_surfaces["runtime_performance_e2e"],
        },
        "packaged_runtime_performance_surface": {
            "runbook": repo_rel(runbook),
            "source_surface_contract": repo_rel(source_surface),
            "workload_manifest": repo_rel(workload_manifest),
            "executable_fixture_manifest": repo_rel(fixture_manifest),
            "artifact_surface_contract": repo_rel(artifact_surface),
            "optimization_policy": repo_rel(optimization_policy),
            "workload_replay_contract": repo_rel(workload_replay_contract),
            "metadata_resilience_contract": repo_rel(metadata_resilience_contract),
            "stress_sanitizer_contract": repo_rel(stress_sanitizer_contract),
            "telemetry_schema": repo_rel(telemetry_schema),
        },
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "exit_code": package_result.returncode,
            },
            {
                "action": "benchmark-packaged-runtime-performance",
                "exit_code": benchmark_result.returncode,
                "summary_path": repo_rel(summary_out),
            },
        ],
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print("objc3c-runnable-runtime-performance: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
