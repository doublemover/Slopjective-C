#!/usr/bin/env python3
"""Validate runnable storage/reflection execution end to end from the staged package root."""

from __future__ import annotations

import json
import os
import re
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime" / "runnable-storage-reflection-e2e" / "summary.json"
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.storage.reflection.e2e.summary.v1"
PACKAGE_CONTRACT_ID = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


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


def extract_output_value(stdout: str, key: str) -> str | None:
    prefix = f"{key}:"
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith(prefix):
            return line.split(":", 1)[1].strip()
    return None


def extract_report_paths(stdout: str) -> list[str]:
    report_paths: list[str] = []
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith("summary_path:"):
            report_paths.append(line.split(":", 1)[1].strip().replace("\\", "/"))
            continue
        match = re.search(r"runtime-acceptance:\s+PASS\s+\((.+)\)", line)
        if match:
            candidate = Path(match.group(1).strip())
            try:
                report_paths.append(repo_rel(candidate))
            except ValueError:
                report_paths.append(match.group(1).strip().replace("\\", "/"))
            continue
        if line.startswith("public-workflow-report:"):
            report_paths.append(line.split(":", 1)[1].strip().replace("\\", "/"))
    return report_paths


def load_json(path: Path) -> dict[str, Any]:
    expect(path.is_file(), f"expected JSON artifact was not published: {repo_rel(path)}")
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {repo_rel(path)} did not contain an object")
    return payload


def parse_json_output(result: subprocess.CompletedProcess[str], context: str) -> dict[str, Any]:
    if result.returncode != 0:
        raise RuntimeError(f"{context} failed with exit code {result.returncode}")
    payload = json.loads(result.stdout)
    expect(isinstance(payload, dict), f"{context} did not print a JSON object")
    return payload


def find_clangxx() -> str:
    llvm_root = os.environ.get("LLVM_ROOT")
    if llvm_root:
        candidate = Path(llvm_root) / "bin" / "clang++.exe"
        if candidate.is_file():
            return str(candidate)
    candidate = shutil.which("clang++")
    if candidate:
        return candidate
    raise RuntimeError("clang++ not found; set LLVM_ROOT or ensure clang++ is on PATH")


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-sr-e2e" / run_id
    manifest_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    compile_out_dir = package_root / "tmp" / "artifacts" / "runnable-storage-reflection-e2e" / "compile"
    probe_exe = package_root / "tmp" / "artifacts" / "runnable-storage-reflection-e2e" / "probe.exe"

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
    expect(
        manifest.get("contract_id") == PACKAGE_CONTRACT_ID,
        "runnable toolchain package manifest published the wrong contract id",
    )

    for manifest_key in (
        "compile_wrapper",
        "runtime_library",
        "execution_smoke_script",
        "execution_replay_script",
        "runtime_public_header",
        "runtime_internal_header",
        "storage_reflection_fixture",
        "storage_reflection_probe",
    ):
        relative_path = manifest.get(manifest_key)
        expect(isinstance(relative_path, str) and relative_path, f"package manifest did not publish {manifest_key}")
        candidate = package_root / normalize_rel_path(relative_path)
        expect(candidate.is_file(), f"packaged runnable toolchain missing {manifest_key} at {relative_path}")

    compile_script = package_root / normalize_rel_path(str(manifest["compile_wrapper"]))
    storage_fixture = package_root / normalize_rel_path(str(manifest["storage_reflection_fixture"]))
    runtime_library = package_root / normalize_rel_path(str(manifest["runtime_library"]))
    storage_probe = package_root / normalize_rel_path(str(manifest["storage_reflection_probe"]))
    smoke_script = package_root / normalize_rel_path(str(manifest["execution_smoke_script"]))
    replay_script = package_root / normalize_rel_path(str(manifest["execution_replay_script"]))

    compile_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(compile_script),
            str(storage_fixture),
            "--out-dir",
            str(compile_out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=package_root,
    )
    if compile_result.returncode != 0:
        raise RuntimeError("packaged compile wrapper failed for the storage/reflection fixture")

    compile_artifacts = {
        "manifest": compile_out_dir / "module.manifest.json",
        "registration_manifest": compile_out_dir / "module.runtime-registration-manifest.json",
        "compile_provenance": compile_out_dir / "module.compile-provenance.json",
        "object": compile_out_dir / "module.obj",
    }
    for artifact_path in compile_artifacts.values():
        expect(artifact_path.is_file(), f"packaged compile wrapper did not publish {artifact_path}")

    clangxx = find_clangxx()
    probe_exe.parent.mkdir(parents=True, exist_ok=True)
    probe_compile_result = run_capture(
        [
            clangxx,
            "-std=c++20",
            "-fms-runtime-lib=dll",
            "-I",
            str((package_root / "native/objc3c/src").resolve()),
            str(storage_probe),
            str(compile_artifacts["object"]),
            str(runtime_library),
            "-o",
            str(probe_exe),
        ],
        cwd=package_root,
    )
    if probe_compile_result.returncode != 0:
        raise RuntimeError("packaged storage/reflection probe compile failed")

    probe_payload = parse_json_output(
        run_capture([str(probe_exe)], cwd=package_root),
        "packaged storage/reflection runtime probe",
    )
    box_entry = probe_payload.get("box_entry", {})
    implementation_surface = probe_payload.get("implementation_surface", {})
    current_value = probe_payload.get("current_value_property", {})
    weak_value = probe_payload.get("weak_value_property", {})
    guarded_value = probe_payload.get("guarded_value_property", {})

    expect(box_entry.get("found") == 1, "expected packaged Box class lookup to succeed")
    expect(box_entry.get("runtime_property_accessor_count", 0) >= 5, "expected packaged Box to expose five runtime-backed accessors")
    expect(box_entry.get("runtime_instance_size_bytes", 0) >= 40, "expected packaged Box instance layout to preserve five storage slots")
    expect(implementation_surface.get("property_registry_ready") == 1, "expected packaged implementation snapshot to report property registry readiness")
    expect(implementation_surface.get("runtime_accessor_dispatch_ready") == 1, "expected packaged implementation snapshot to report accessor dispatch readiness")
    expect(implementation_surface.get("reflection_query_ready") == 1, "expected packaged implementation snapshot to report reflection readiness")
    expect(implementation_surface.get("deterministic") == 1, "expected packaged implementation snapshot to report deterministic handoff")
    expect(current_value.get("found") == 1, "expected packaged currentValue property lookup to succeed")
    expect(current_value.get("has_runtime_getter") == 1 and current_value.get("has_runtime_setter") == 1, "expected packaged currentValue accessors to stay runtime-backed")
    expect(weak_value.get("ownership_runtime_hook_profile") == "objc-weak-side-table", "expected packaged weakValue hook profile to preserve objc weak side-table handling")
    expect(guarded_value.get("ownership_runtime_hook_profile") == "objc-unowned-safe-guard", "expected packaged guardedValue hook profile to preserve safe-unowned guarding")

    smoke_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(smoke_script),
            "-Limit",
            "12",
        ],
        cwd=package_root,
    )
    if smoke_result.returncode != 0:
        raise RuntimeError("packaged execution smoke failed")

    replay_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(replay_script),
            "-CaseId",
            "canonical-runnable",
        ],
        cwd=package_root,
    )
    if replay_result.returncode != 0:
        raise RuntimeError("packaged execution replay proof failed")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_storage_reflection_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "packaged_compile_fixture": repo_rel(storage_fixture),
        "packaged_probe": repo_rel(storage_probe),
        "packaged_probe_executable": repo_rel(probe_exe),
        "packaged_compile_artifacts": {
            key: repo_rel(path) for key, path in compile_artifacts.items()
        },
        "probe_payload": probe_payload,
        "child_report_paths": [
            *extract_report_paths(package_result.stdout),
            *extract_report_paths(smoke_result.stdout),
            *extract_report_paths(replay_result.stdout),
        ],
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "exit_code": package_result.returncode,
                "package_root": extract_output_value(package_result.stdout, "package_root"),
                "manifest": extract_output_value(package_result.stdout, "manifest"),
            },
            {
                "action": "compile-storage-reflection-fixture",
                "exit_code": compile_result.returncode,
            },
            {
                "action": "compile-packaged-storage-reflection-probe",
                "exit_code": probe_compile_result.returncode,
                "clangxx": clangxx,
            },
            {
                "action": "run-packaged-storage-reflection-probe",
                "exit_code": 0,
            },
            {
                "action": "packaged-execution-smoke",
                "exit_code": smoke_result.returncode,
                "report_paths": extract_report_paths(smoke_result.stdout),
            },
            {
                "action": "packaged-execution-replay",
                "exit_code": replay_result.returncode,
                "report_paths": extract_report_paths(replay_result.stdout),
            },
        ],
    }

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
