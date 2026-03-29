#!/usr/bin/env python3
"""Validate runnable concurrency execution end to end from the staged package root."""

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
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime" / "runnable-concurrency-e2e" / "summary.json"
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.concurrency.e2e.summary.v1"
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
        ],
        cwd=cwd,
    )
    if compile_result.returncode != 0:
        raise RuntimeError(f"packaged compile wrapper failed for {repo_rel(fixture_path)}")

    artifacts = {
        "manifest": compile_out_dir / "module.manifest.json",
        "registration_manifest": compile_out_dir / "module.runtime-registration-manifest.json",
        "compile_provenance": compile_out_dir / "module.compile-provenance.json",
        "object": compile_out_dir / "module.obj",
    }
    for artifact_path in artifacts.values():
        expect(artifact_path.is_file(), f"packaged compile wrapper did not publish {artifact_path}")
    return artifacts


def compile_probe(
    clangxx: str,
    probe_source: Path,
    object_file: Path,
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
            str(object_file),
            str(runtime_library),
            "-o",
            str(probe_exe),
        ],
        cwd=cwd,
    )
    if probe_compile_result.returncode != 0:
        raise RuntimeError(f"packaged concurrency probe compile failed for {repo_rel(probe_source)}")


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-cc-e2e" / run_id
    manifest_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    artifacts_root = package_root / "tmp" / "artifacts" / "runnable-concurrency-e2e"

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
        "runtime_public_header",
        "runtime_internal_header",
        "continuation_runtime_fixture",
        "continuation_runtime_probe",
        "task_runtime_fixture",
        "task_runtime_probe",
        "actor_runtime_fixture",
        "actor_runtime_probe",
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

    continuation_fixture = package_root / normalize_rel_path(str(manifest["continuation_runtime_fixture"]))
    continuation_probe = package_root / normalize_rel_path(str(manifest["continuation_runtime_probe"]))
    task_fixture = package_root / normalize_rel_path(str(manifest["task_runtime_fixture"]))
    task_probe = package_root / normalize_rel_path(str(manifest["task_runtime_probe"]))
    actor_fixture = package_root / normalize_rel_path(str(manifest["actor_runtime_fixture"]))
    actor_probe = package_root / normalize_rel_path(str(manifest["actor_runtime_probe"]))

    continuation_artifacts = compile_fixture(
        compile_script,
        continuation_fixture,
        artifacts_root / "continuation" / "compile",
        cwd=package_root,
    )
    task_artifacts = compile_fixture(
        compile_script,
        task_fixture,
        artifacts_root / "task" / "compile",
        cwd=package_root,
    )
    actor_artifacts = compile_fixture(
        compile_script,
        actor_fixture,
        artifacts_root / "actor" / "compile",
        cwd=package_root,
    )

    clangxx = find_clangxx()
    continuation_exe = artifacts_root / "continuation" / "probe.exe"
    task_exe = artifacts_root / "task" / "probe.exe"
    actor_exe = artifacts_root / "actor" / "probe.exe"
    compile_probe(
        clangxx,
        continuation_probe,
        continuation_artifacts["object"],
        runtime_library,
        continuation_exe,
        cwd=package_root,
    )
    compile_probe(
        clangxx,
        task_probe,
        task_artifacts["object"],
        runtime_library,
        task_exe,
        cwd=package_root,
    )
    compile_probe(
        clangxx,
        actor_probe,
        actor_artifacts["object"],
        runtime_library,
        actor_exe,
        cwd=package_root,
    )

    continuation_payload = parse_key_value_output(
        run_capture([str(continuation_exe)], cwd=package_root),
        "packaged concurrency continuation probe",
    )
    for field, expected_value in {
        "runTask": 7,
        "loadValue": 7,
        "copy_status": 0,
        "allocation_call_count": 2,
        "handoff_call_count": 2,
        "resume_call_count": 2,
        "live_continuation_handle_count": 0,
        "last_handoff_executor_tag": 1,
        "last_resume_return_value": 7,
    }.items():
        expect(
            continuation_payload.get(field) == expected_value,
            f"expected packaged concurrency continuation probe to preserve {field}",
        )

    task_payload = parse_key_value_output(
        run_capture([str(task_exe)], cwd=package_root),
        "packaged concurrency task probe",
    )
    for field, expected_value in {
        "spawn_group": 111,
        "scope": 1,
        "add_task": 1,
        "cancelled": 0,
        "wait_next": 23,
        "hop": 23,
        "cancel_all": 31,
        "on_cancel": 41,
        "spawn_detached": 121,
        "copy_status": 0,
        "spawn_call_count": 2,
        "scope_call_count": 1,
        "add_task_call_count": 1,
        "wait_next_call_count": 1,
        "cancel_all_call_count": 1,
        "cancellation_poll_call_count": 1,
        "on_cancel_call_count": 1,
        "executor_hop_call_count": 1,
        "last_spawn_kind": 2,
        "last_spawn_executor_tag": 3,
        "last_wait_next_result": 23,
        "last_executor_hop_executor_tag": 2,
        "last_executor_hop_value": 23,
    }.items():
        expect(
            task_payload.get(field) == expected_value,
            f"expected packaged concurrency task probe to preserve {field}",
        )

    actor_payload = parse_key_value_output(
        run_capture([str(actor_exe)], cwd=package_root),
        "packaged concurrency actor probe",
    )
    for field, expected_value in {
        "copy_status": 0,
        "replay": 1,
        "guard": 1,
        "isolation": 1,
        "bound": 1,
        "enqueued": 23,
        "drained": 23,
        "replay_proof_call_count": 1,
        "race_guard_call_count": 1,
        "isolation_thunk_call_count": 1,
        "bind_executor_call_count": 1,
        "mailbox_enqueue_call_count": 1,
        "mailbox_drain_call_count": 1,
        "last_replay_proof_executor_tag": 1,
        "last_race_guard_executor_tag": 1,
        "last_isolation_executor_tag": 1,
        "last_bound_actor_handle": 41,
        "last_bound_executor_tag": 1,
        "last_mailbox_actor_handle": 41,
        "last_mailbox_enqueued_value": 23,
        "last_mailbox_executor_tag": 1,
        "last_mailbox_depth": 0,
        "last_mailbox_drained_value": 23,
    }.items():
        expect(
            actor_payload.get(field) == expected_value,
            f"expected packaged concurrency actor probe to preserve {field}",
        )

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
        "runner_path": "scripts/check_objc3c_runnable_concurrency_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "packaged_probe_executables": {
            "continuation": repo_rel(continuation_exe),
            "task": repo_rel(task_exe),
            "actor": repo_rel(actor_exe),
        },
        "packaged_compile_artifacts": {
            "continuation": {key: repo_rel(path) for key, path in continuation_artifacts.items()},
            "task": {key: repo_rel(path) for key, path in task_artifacts.items()},
            "actor": {key: repo_rel(path) for key, path in actor_artifacts.items()},
        },
        "probe_payloads": {
            "continuation": continuation_payload,
            "task": task_payload,
            "actor": actor_payload,
        },
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
            {"action": "compile-continuation-fixture", "exit_code": 0},
            {"action": "compile-task-fixture", "exit_code": 0},
            {"action": "compile-actor-fixture", "exit_code": 0},
            {"action": "compile-packaged-continuation-probe", "exit_code": 0, "clangxx": clangxx},
            {"action": "compile-packaged-task-probe", "exit_code": 0, "clangxx": clangxx},
            {"action": "compile-packaged-actor-probe", "exit_code": 0, "clangxx": clangxx},
            {"action": "run-packaged-continuation-probe", "exit_code": 0},
            {"action": "run-packaged-task-probe", "exit_code": 0},
            {"action": "run-packaged-actor-probe", "exit_code": 0},
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
