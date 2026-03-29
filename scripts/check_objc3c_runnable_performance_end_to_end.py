#!/usr/bin/env python3
"""Validate the staged runnable benchmark surface end to end from the package root."""

from __future__ import annotations

import json
import os
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
REPORT_PATH = ROOT / "tmp" / "reports" / "performance" / "runnable-end-to-end-summary.json"
PACKAGE_CONTRACT_ID = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"
SUMMARY_CONTRACT_ID = "objc3c.performance.runnable.end.to.end.summary.v1"


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
    package_root = ROOT / "tmp" / "pkg" / "objc3c-performance-e2e" / run_id
    manifest_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    performance_root = package_root / "tmp" / "artifacts" / "performance" / "runnable-e2e"
    performance_root.mkdir(parents=True, exist_ok=True)

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
    performance_surface = manifest.get("performance_benchmark_surface", {})
    compile_wrapper = package_root / normalize_rel_path(str(manifest["compile_wrapper"]))
    benchmark_portfolio = package_root / normalize_rel_path(str(performance_surface["benchmark_portfolio"]))
    measurement_policy = package_root / normalize_rel_path(str(performance_surface["measurement_policy"]))
    benchmark_parameters = package_root / normalize_rel_path(str(performance_surface["benchmark_parameters"]))
    comparative_manifest = package_root / normalize_rel_path(str(performance_surface["comparative_baseline_manifest"]))
    telemetry_schema = package_root / normalize_rel_path(str(performance_surface["telemetry_schema"]))
    aurora_source = package_root / "showcase" / "auroraBoard" / "main.objc3"
    objc2_source = package_root / "tests" / "tooling" / "fixtures" / "performance" / "baselines" / "objc2_reference_workload.m"
    swift_source = package_root / "tests" / "tooling" / "fixtures" / "performance" / "baselines" / "swift_reference_workload.swift"
    cpp_source = package_root / "tests" / "tooling" / "fixtures" / "performance" / "baselines" / "cpp_reference_workload.cpp"

    expect(command_surfaces.get("inspect_performance") == "npm run inspect:objc3c:performance", "package manifest missing inspect_performance command surface")
    expect(command_surfaces.get("inspect_comparative_baselines") == "npm run inspect:objc3c:comparative-baselines", "package manifest missing inspect_comparative_baselines command surface")
    expect(command_surfaces.get("runnable_performance") == "npm run test:objc3c:runnable-performance", "package manifest missing runnable_performance command surface")

    for path in (
        benchmark_portfolio,
        measurement_policy,
        benchmark_parameters,
        comparative_manifest,
        telemetry_schema,
        aurora_source,
        objc2_source,
        swift_source,
        cpp_source,
    ):
        expect(path.is_file(), f"packaged runnable toolchain missing required performance file {path}")

    compile_out_dir = performance_root / "auroraBoard"
    compile_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(compile_wrapper),
            str(aurora_source),
            "--out-dir",
            str(compile_out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=package_root,
    )
    if compile_result.returncode != 0:
        raise RuntimeError("packaged compile wrapper failed for performance auroraBoard source")

    expect((compile_out_dir / "module.obj").is_file(), "packaged performance compile did not publish module.obj")

    objc2_result = run_capture(["clang", "-fsyntax-only", str(objc2_source)], cwd=package_root)
    if objc2_result.returncode != 0:
        raise RuntimeError("packaged ObjC2 baseline syntax check failed")

    cpp_exe = performance_root / "cpp_reference.exe"
    cpp_result = run_capture(["clang++", str(cpp_source), "-o", str(cpp_exe)], cwd=package_root)
    if cpp_result.returncode != 0:
        raise RuntimeError("packaged C++ baseline compile failed")
    cpp_run_result = run_capture([str(cpp_exe)], cwd=package_root)

    swift_exe = performance_root / "swift_reference.exe"
    swift_result = run_capture(["swiftc", str(swift_source), "-o", str(swift_exe)], cwd=package_root)
    if swift_result.returncode != 0:
        raise RuntimeError("packaged Swift baseline compile failed")
    swift_run_result = run_capture([str(swift_exe)], cwd=package_root)

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_performance_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "packaged_command_surfaces": {
            "inspect_performance": command_surfaces["inspect_performance"],
            "inspect_comparative_baselines": command_surfaces["inspect_comparative_baselines"],
            "runnable_performance": command_surfaces["runnable_performance"],
        },
        "packaged_performance_surface": {
            "benchmark_portfolio": repo_rel(benchmark_portfolio),
            "measurement_policy": repo_rel(measurement_policy),
            "benchmark_parameters": repo_rel(benchmark_parameters),
            "comparative_baseline_manifest": repo_rel(comparative_manifest),
            "telemetry_schema": repo_rel(telemetry_schema),
        },
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "exit_code": package_result.returncode,
            },
            {
                "action": "compile-packaged-auroraBoard",
                "exit_code": compile_result.returncode,
            },
            {
                "action": "syntax-check-packaged-objc2-baseline",
                "exit_code": objc2_result.returncode,
            },
            {
                "action": "compile-run-packaged-cpp-baseline",
                "exit_code": cpp_result.returncode,
                "runtime_exit_code": cpp_run_result.returncode,
            },
            {
                "action": "compile-run-packaged-swift-baseline",
                "exit_code": swift_result.returncode,
                "runtime_exit_code": swift_run_result.returncode,
            },
        ],
    }
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    print("objc3c-runnable-performance: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
