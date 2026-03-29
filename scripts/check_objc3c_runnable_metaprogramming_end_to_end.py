#!/usr/bin/env python3
"""Validate runnable metaprogramming execution end to end from the staged package root."""

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
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime" / "runnable-metaprogramming-e2e" / "summary.json"
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.metaprogramming.e2e.summary.v1"
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


def parse_key_value_output(result: subprocess.CompletedProcess[str], context: str) -> dict[str, Any]:
    if result.returncode != 0:
        raise RuntimeError(f"{context} failed with exit code {result.returncode}")
    payload: dict[str, Any] = {}
    for raw_line in result.stdout.splitlines():
        line = raw_line.strip()
        if not line or "=" not in line:
            continue
        key, value = line.split("=", 1)
        key = key.strip()
        value = value.strip()
        if re.fullmatch(r"-?\d+", value):
            payload[key] = int(value)
        elif value.lower() == "true":
            payload[key] = True
        elif value.lower() == "false":
            payload[key] = False
        else:
            payload[key] = value
    expect(payload, f"{context} did not print any key=value fields")
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


def compile_fixture(
    compile_script: Path,
    fixture_path: Path,
    compile_out_dir: Path,
    *,
    cwd: Path,
    extra_args: Sequence[str] = (),
) -> dict[str, Path]:
    compile_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(compile_script),
            str(fixture_path),
            "--out-dir",
            str(compile_out_dir),
            "--emit-prefix",
            "module",
            *extra_args,
        ],
        cwd=cwd,
    )
    if compile_result.returncode != 0:
        raise RuntimeError(f"packaged compile wrapper failed for {fixture_path}")

    artifacts = {
        "manifest": compile_out_dir / "module.manifest.json",
        "registration_manifest": compile_out_dir / "module.runtime-registration-manifest.json",
        "runtime_import_surface": compile_out_dir / "module.runtime-import-surface.json",
        "host_cache": compile_out_dir / "module.metaprogramming-macro-host-cache.json",
    }
    for artifact_path in artifacts.values():
        expect(artifact_path.is_file(), f"packaged compile wrapper did not publish {artifact_path}")
    return artifacts


def compile_probe(
    clangxx: str,
    probe_source: Path,
    runtime_library: Path,
    probe_exe: Path,
    *,
    cwd: Path,
) -> None:
    probe_exe.parent.mkdir(parents=True, exist_ok=True)
    probe_compile_result = run_capture(
        [
            clangxx,
            "-std=c++20",
            "-fms-runtime-lib=dll",
            "-I",
            str((cwd / "native/objc3c/src").resolve()),
            str(probe_source),
            str(runtime_library),
            "-o",
            str(probe_exe),
        ],
        cwd=cwd,
    )
    if probe_compile_result.returncode != 0:
        raise RuntimeError(f"packaged metaprogramming probe compile failed for {probe_source}")


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-mp-e2e" / run_id
    manifest_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    artifacts_root = package_root / "tmp" / "artifacts" / "runnable-metaprogramming-e2e"

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

    required_manifest_keys = (
        "compile_wrapper",
        "runtime_library",
        "execution_smoke_script",
        "execution_replay_script",
        "runtime_internal_header",
        "metaprogramming_runtime_fixture",
        "metaprogramming_runtime_consumer_fixture",
        "metaprogramming_runtime_probe",
    )
    for manifest_key in required_manifest_keys:
        relative_path = manifest.get(manifest_key)
        expect(isinstance(relative_path, str) and relative_path, f"package manifest did not publish {manifest_key}")
        candidate = package_root / normalize_rel_path(relative_path)
        expect(candidate.is_file(), f"packaged runnable toolchain missing {manifest_key} at {relative_path}")

    compile_script = package_root / normalize_rel_path(str(manifest["compile_wrapper"]))
    runtime_library = package_root / normalize_rel_path(str(manifest["runtime_library"]))
    smoke_script = package_root / normalize_rel_path(str(manifest["execution_smoke_script"]))
    replay_script = package_root / normalize_rel_path(str(manifest["execution_replay_script"]))
    provider_fixture = package_root / normalize_rel_path(str(manifest["metaprogramming_runtime_fixture"]))
    consumer_fixture = package_root / normalize_rel_path(str(manifest["metaprogramming_runtime_consumer_fixture"]))
    runtime_probe = package_root / normalize_rel_path(str(manifest["metaprogramming_runtime_probe"]))

    provider_artifacts = compile_fixture(
        compile_script,
        provider_fixture,
        artifacts_root / "provider-first" / "compile",
        cwd=package_root,
        extra_args=("--objc3-bootstrap-registration-order-ordinal", "1"),
    )
    provider_host_cache = load_json(provider_artifacts["host_cache"])
    for field_name, expected_value in {
        "cache_ready": True,
        "launch_attempted": True,
        "cache_hit": True,
        "cache_summary_present": True,
        "cache_runtime_import_surface_present": True,
        "cache_manifest_present": True,
        "cache_materialization_state": "materialized",
        "host_process_exit_code": 0,
        "deterministic": True,
    }.items():
        expect(
            provider_host_cache.get(field_name) == expected_value,
            f"expected packaged metaprogramming first compile to preserve {field_name}",
        )

    provider_artifacts_second = compile_fixture(
        compile_script,
        provider_fixture,
        artifacts_root / "provider-second" / "compile",
        cwd=package_root,
        extra_args=("--objc3-bootstrap-registration-order-ordinal", "1"),
    )
    provider_host_cache_second = load_json(provider_artifacts_second["host_cache"])
    for field_name, expected_value in {
        "cache_ready": True,
        "launch_attempted": False,
        "cache_hit": True,
        "cache_summary_present": True,
        "cache_runtime_import_surface_present": True,
        "cache_manifest_present": True,
        "cache_materialization_state": "cache-hit",
        "host_process_exit_code": 0,
        "deterministic": True,
    }.items():
        expect(
            provider_host_cache_second.get(field_name) == expected_value,
            f"expected packaged metaprogramming second compile to preserve {field_name}",
        )
    for field_name in (
        "cache_key",
        "cache_entry_relative_path",
        "cache_summary_relative_path",
        "cache_runtime_import_surface_relative_path",
        "cache_manifest_relative_path",
        "host_executable_relative_path",
        "cache_root_relative_path",
        "replay_key",
    ):
        expect(
            provider_host_cache_second.get(field_name) == provider_host_cache.get(field_name),
            f"expected packaged metaprogramming second compile to preserve {field_name}",
        )

    provider_runtime_import = load_json(provider_artifacts["runtime_import_surface"])
    provider_module_name = provider_runtime_import.get("module_name")
    expect(
        isinstance(provider_module_name, str) and provider_module_name != "",
        "packaged metaprogramming provider did not publish a module name",
    )

    consumer_artifacts = compile_fixture(
        compile_script,
        consumer_fixture,
        artifacts_root / "consumer" / "compile",
        cwd=package_root,
        extra_args=(
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_artifacts["runtime_import_surface"]),
        ),
    )
    link_plan = load_json(artifacts_root / "consumer" / "compile" / "module.cross-module-runtime-link-plan.json")
    expect(
        link_plan.get("metaprogramming_host_cache_imported_module_count") == 1
        and link_plan.get("metaprogramming_host_cache_imported_module_names_lexicographic")
        == [provider_module_name]
        and link_plan.get("metaprogramming_host_cache_cross_module_preservation_ready")
        is True,
        "packaged metaprogramming consumer link plan drifted from the imported host-cache module set",
    )

    clangxx = find_clangxx()
    probe_exe = artifacts_root / "probe.exe"
    compile_probe(clangxx, runtime_probe, runtime_library, probe_exe, cwd=package_root)
    probe_payload = parse_key_value_output(
        run_capture([str(probe_exe)], cwd=package_root),
        "packaged metaprogramming runtime probe",
    )
    expect(
        probe_payload.get("copy_status") == 0
        and probe_payload.get("macro_host_execution_ready") == 1
        and probe_payload.get("macro_host_process_launch_ready") == 1
        and probe_payload.get("runtime_package_loader_ready") == 0,
        "packaged metaprogramming runtime probe drifted from the live host-cache readiness boundary",
    )
    expect(
        probe_payload.get("host_executable_relative_path")
        == provider_host_cache.get("host_executable_relative_path")
        and probe_payload.get("cache_root_relative_path")
        == provider_host_cache.get("cache_root_relative_path"),
        "packaged metaprogramming runtime probe drifted from the packaged cache artifact paths",
    )

    smoke_result = run_capture(
        [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(smoke_script)],
        cwd=package_root,
    )
    if smoke_result.returncode != 0:
        raise RuntimeError("packaged execution smoke validation failed")
    replay_result = run_capture(
        [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(replay_script)],
        cwd=package_root,
    )
    if replay_result.returncode != 0:
        raise RuntimeError("packaged execution replay validation failed")

    child_report_paths = [repo_rel(manifest_path)]
    for stdout in (package_result.stdout, smoke_result.stdout, replay_result.stdout):
        child_report_paths.extend(extract_report_paths(stdout))

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_metaprogramming_end_to_end.py",
        "package_root": repo_rel(package_root),
        "package_manifest_path": repo_rel(manifest_path),
        "provider_compile_manifest_path": repo_rel(provider_artifacts["manifest"]),
        "provider_host_cache_artifact_path": repo_rel(provider_artifacts["host_cache"]),
        "provider_runtime_import_surface_path": repo_rel(provider_artifacts["runtime_import_surface"]),
        "consumer_compile_manifest_path": repo_rel(consumer_artifacts["manifest"]),
        "consumer_link_plan_path": repo_rel(artifacts_root / "consumer" / "compile" / "module.cross-module-runtime-link-plan.json"),
        "probe_path": repo_rel(runtime_probe),
        "probe_executable_path": repo_rel(probe_exe),
        "provider_module_name": provider_module_name,
        "provider_cache_key": provider_host_cache.get("cache_key"),
        "provider_first_materialization_state": provider_host_cache.get("cache_materialization_state"),
        "provider_second_materialization_state": provider_host_cache_second.get("cache_materialization_state"),
        "smoke_report_path": extract_output_value(smoke_result.stdout, "summary_path"),
        "replay_report_path": extract_output_value(replay_result.stdout, "summary_path"),
        "child_report_paths": sorted(dict.fromkeys(child_report_paths)),
    }

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
