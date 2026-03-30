#!/usr/bin/env python3
"""Validate the staged runnable compiler-throughput surface end to end."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
REPORT_PATH = ROOT / "tmp" / "reports" / "compiler-throughput" / "runnable-end-to-end-summary.json"
PACKAGE_CONTRACT_ID = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"
SUMMARY_CONTRACT_ID = "objc3c.compiler.throughput.runnable.end.to.end.summary.v1"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def normalize_rel_path(raw_path: str) -> str:
    return raw_path.replace("\\", "/")


def run_capture(command: Sequence[str], *, cwd: Path) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        list(command),
        cwd=cwd,
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
    expect(path.is_file(), f"expected JSON artifact was not published: {path}")
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {path} did not contain an object")
    return payload


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-compiler-throughput-e2e" / run_id
    manifest_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    summary_out = package_root / "tmp" / "reports" / "compiler-throughput" / "benchmark-summary.json"

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
    compiler_surface = manifest.get("compiler_throughput_surface", {})

    benchmark_script = package_root / "scripts" / "check_objc3c_native_perf_budget.ps1"
    runbook = package_root / normalize_rel_path(str(compiler_surface["runbook"]))
    source_surface = package_root / normalize_rel_path(str(compiler_surface["source_surface_contract"]))
    workload_manifest = package_root / normalize_rel_path(str(compiler_surface["workload_manifest"]))
    validation_tier_map = package_root / normalize_rel_path(str(compiler_surface["validation_tier_map"]))
    optimization_policy = package_root / normalize_rel_path(str(compiler_surface["optimization_policy"]))
    artifact_surface = package_root / normalize_rel_path(str(compiler_surface["artifact_surface_contract"]))
    summary_schema = package_root / normalize_rel_path(str(compiler_surface["summary_schema"]))

    expect(
        command_surfaces.get("inspect_compiler_throughput") == "npm run inspect:objc3c:compiler-throughput",
        "package manifest missing inspect_compiler_throughput command surface",
    )
    expect(
        command_surfaces.get("compiler_throughput") == "npm run test:objc3c:compiler-throughput",
        "package manifest missing compiler_throughput command surface",
    )
    expect(
        command_surfaces.get("compiler_throughput_e2e") == "npm run test:objc3c:runnable-compiler-throughput",
        "package manifest missing compiler_throughput_e2e command surface",
    )

    for path in (
        benchmark_script,
        runbook,
        source_surface,
        workload_manifest,
        validation_tier_map,
        optimization_policy,
        artifact_surface,
        summary_schema,
    ):
        expect(path.is_file(), f"packaged runnable toolchain missing required compiler-throughput file {path}")

    benchmark_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(benchmark_script),
        ],
        cwd=package_root,
    )
    if benchmark_result.returncode != 0:
        raise RuntimeError("packaged compiler-throughput benchmark failed")

    benchmark_summary = load_json(summary_out)
    expect(
        benchmark_summary.get("contract_id") == "objc3c.compiler.throughput.summary.v1",
        "packaged compiler-throughput benchmark summary contract drifted",
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_compiler_throughput_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "packaged_command_surfaces": {
            "inspect_compiler_throughput": command_surfaces["inspect_compiler_throughput"],
            "compiler_throughput": command_surfaces["compiler_throughput"],
            "compiler_throughput_e2e": command_surfaces["compiler_throughput_e2e"],
        },
        "packaged_compiler_throughput_surface": {
            "runbook": repo_rel(runbook),
            "source_surface_contract": repo_rel(source_surface),
            "workload_manifest": repo_rel(workload_manifest),
            "validation_tier_map": repo_rel(validation_tier_map),
            "optimization_policy": repo_rel(optimization_policy),
            "artifact_surface_contract": repo_rel(artifact_surface),
            "summary_schema": repo_rel(summary_schema),
        },
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "exit_code": package_result.returncode,
            },
            {
                "action": "benchmark-packaged-compiler-throughput",
                "exit_code": benchmark_result.returncode,
                "summary_path": repo_rel(summary_out),
            },
        ],
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print("objc3c-runnable-compiler-throughput: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
