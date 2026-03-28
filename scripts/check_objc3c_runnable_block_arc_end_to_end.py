#!/usr/bin/env python3
"""Validate runnable block/ARC execution end to end from the staged package root."""

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
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime" / "runnable-block-arc-e2e" / "summary.json"
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.block.arc.e2e.summary.v1"
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
    package_root = ROOT / "tmp" / "pkg" / "objc3c-ba-e2e" / run_id
    manifest_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    compile_out_dir = package_root / "tmp" / "artifacts" / "runnable-block-arc-e2e" / "compile"
    linked_fixture_exe = package_root / "tmp" / "artifacts" / "runnable-block-arc-e2e" / "block-fixture.exe"
    abi_probe_exe = package_root / "tmp" / "artifacts" / "runnable-block-arc-e2e" / "block-arc-runtime-abi-probe.exe"
    byref_probe_exe = package_root / "tmp" / "artifacts" / "runnable-block-arc-e2e" / "block-byref-forwarding-probe.exe"
    smoke_fixture_list = package_root / "tmp" / "artifacts" / "runnable-block-arc-e2e" / "execution-fixtures.txt"

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

    directory_manifest_keys = {"execution_fixture_root"}

    for manifest_key in (
        "compile_wrapper",
        "runtime_library",
        "execution_smoke_script",
        "execution_replay_script",
        "execution_fixture_root",
        "runtime_public_header",
        "runtime_internal_header",
        "block_arc_fixture",
        "block_arc_runtime_abi_probe",
        "block_arc_byref_forwarding_probe",
    ):
        relative_path = manifest.get(manifest_key)
        expect(isinstance(relative_path, str) and relative_path, f"package manifest did not publish {manifest_key}")
        candidate = package_root / normalize_rel_path(relative_path)
        if manifest_key in directory_manifest_keys:
            expect(candidate.is_dir(), f"packaged runnable toolchain missing {manifest_key} at {relative_path}")
        else:
            expect(candidate.is_file(), f"packaged runnable toolchain missing {manifest_key} at {relative_path}")

    compile_script = package_root / normalize_rel_path(str(manifest["compile_wrapper"]))
    runtime_library = package_root / normalize_rel_path(str(manifest["runtime_library"]))
    block_arc_fixture = package_root / normalize_rel_path(str(manifest["block_arc_fixture"]))
    runtime_abi_probe = package_root / normalize_rel_path(str(manifest["block_arc_runtime_abi_probe"]))
    byref_forwarding_probe = package_root / normalize_rel_path(str(manifest["block_arc_byref_forwarding_probe"]))
    smoke_script = package_root / normalize_rel_path(str(manifest["execution_smoke_script"]))
    replay_script = package_root / normalize_rel_path(str(manifest["execution_replay_script"]))
    execution_fixture_root = normalize_rel_path(str(manifest["execution_fixture_root"])).rstrip("/")

    compile_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(compile_script),
            str(block_arc_fixture),
            "--out-dir",
            str(compile_out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=package_root,
    )
    if compile_result.returncode != 0:
        raise RuntimeError("packaged compile wrapper failed for the block/ARC fixture")

    compile_artifacts = {
        "manifest": compile_out_dir / "module.manifest.json",
        "registration_manifest": compile_out_dir / "module.runtime-registration-manifest.json",
        "compile_provenance": compile_out_dir / "module.compile-provenance.json",
        "object": compile_out_dir / "module.obj",
    }
    for artifact_path in compile_artifacts.values():
        expect(artifact_path.is_file(), f"packaged compile wrapper did not publish {artifact_path}")

    clangxx = find_clangxx()
    linked_fixture_exe.parent.mkdir(parents=True, exist_ok=True)

    link_fixture_result = run_capture(
        [
            clangxx,
            "-std=c++20",
            "-fms-runtime-lib=dll",
            str(compile_artifacts["object"]),
            str(runtime_library),
            "-o",
            str(linked_fixture_exe),
        ],
        cwd=package_root,
    )
    if link_fixture_result.returncode != 0:
        raise RuntimeError("packaged block/ARC fixture link failed")

    linked_fixture_run = run_capture([str(linked_fixture_exe)], cwd=package_root)
    expect(
        linked_fixture_run.returncode == 14,
        f"expected packaged block/ARC fixture to exit 14, saw {linked_fixture_run.returncode}",
    )

    abi_probe_compile_result = run_capture(
        [
            clangxx,
            "-std=c++20",
            "-fms-runtime-lib=dll",
            "-I",
            str((package_root / "native/objc3c/src").resolve()),
            str(runtime_abi_probe),
            str(runtime_library),
            "-o",
            str(abi_probe_exe),
        ],
        cwd=package_root,
    )
    if abi_probe_compile_result.returncode != 0:
        raise RuntimeError("packaged block ARC runtime ABI probe compile failed")

    abi_probe_payload = parse_json_output(
        run_capture([str(abi_probe_exe)], cwd=package_root),
        "packaged block ARC runtime ABI probe",
    )
    expect(abi_probe_payload.get("abi_status") == 0, "expected packaged block ARC ABI probe status to succeed")
    expect(abi_probe_payload.get("arc_status") == 0, "expected packaged ARC debug state snapshot to succeed")
    expect(abi_probe_payload.get("invoke_result") == 17, "expected packaged block ARC ABI probe to preserve invoke_result 17")
    expect(abi_probe_payload.get("block_promote_call_count") == 1, "expected packaged block ARC ABI probe to preserve one promote call")
    expect(abi_probe_payload.get("block_invoke_call_count") == 1, "expected packaged block ARC ABI probe to preserve one invoke call")
    expect(abi_probe_payload.get("retain_call_count") == 2, "expected packaged block ARC ABI probe to preserve two retain calls")
    expect(abi_probe_payload.get("release_call_count") == 3, "expected packaged block ARC ABI probe to preserve three release calls")
    expect(abi_probe_payload.get("autorelease_call_count") == 1, "expected packaged block ARC ABI probe to preserve one autorelease call")
    expect(abi_probe_payload.get("arc_debug_state_snapshot_symbol") == "objc3_runtime_copy_arc_debug_state_for_testing", "expected packaged block ARC ABI probe to preserve the ARC debug state snapshot symbol")

    byref_probe_compile_result = run_capture(
        [
            clangxx,
            "-std=c++20",
            "-fms-runtime-lib=dll",
            "-I",
            str((package_root / "native/objc3c/src").resolve()),
            str(byref_forwarding_probe),
            str(runtime_library),
            "-o",
            str(byref_probe_exe),
        ],
        cwd=package_root,
    )
    if byref_probe_compile_result.returncode != 0:
        raise RuntimeError("packaged block byref forwarding probe compile failed")

    byref_probe_payload = parse_json_output(
        run_capture([str(byref_probe_exe)], cwd=package_root),
        "packaged block byref forwarding probe",
    )
    expect(byref_probe_payload.get("handle", 0) > 0, "expected packaged byref forwarding probe to publish a positive handle")
    expect(byref_probe_payload.get("copy_count_after_promotion") == 1, "expected packaged byref forwarding probe to preserve one copy helper call")
    expect(byref_probe_payload.get("first_invoke_result") == 23, "expected packaged byref forwarding probe to preserve first invoke result 23")
    expect(byref_probe_payload.get("second_invoke_result") == 25, "expected packaged byref forwarding probe to preserve second invoke result 25")
    expect(byref_probe_payload.get("dispose_count_before_final_release") == 0, "expected packaged byref forwarding probe to defer dispose before final release")
    expect(byref_probe_payload.get("dispose_count_after_final_release") == 1, "expected packaged byref forwarding probe to execute dispose on final release")
    expect(byref_probe_payload.get("last_disposed_value") == 11, "expected packaged byref forwarding probe to preserve the disposed payload")
    expect(byref_probe_payload.get("invoke_after_release_result") == 0, "expected packaged byref forwarding probe to reject invoke after final release")

    smoke_fixture_list.parent.mkdir(parents=True, exist_ok=True)
    smoke_fixture_list.write_text(
        "\n".join(
            [
                f"{execution_fixture_root}/positive/arc_block_autorelease_return_positive.objc3",
                f"{execution_fixture_root}/positive/arc_cleanup_scope_positive.objc3",
                f"{execution_fixture_root}/positive/arc_implicit_cleanup_void_positive.objc3",
            ]
        )
        + "\n",
        encoding="utf-8",
    )
    smoke_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(smoke_script),
            "-FixtureList",
            str(smoke_fixture_list),
        ],
        cwd=package_root,
    )
    if smoke_result.returncode != 0:
        raise RuntimeError("packaged block/ARC execution smoke failed")

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
        "runner_path": "scripts/check_objc3c_runnable_block_arc_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "packaged_compile_fixture": repo_rel(block_arc_fixture),
        "packaged_runtime_abi_probe": repo_rel(runtime_abi_probe),
        "packaged_byref_forwarding_probe": repo_rel(byref_forwarding_probe),
        "packaged_fixture_executable": repo_rel(linked_fixture_exe),
        "packaged_runtime_abi_probe_executable": repo_rel(abi_probe_exe),
        "packaged_byref_forwarding_probe_executable": repo_rel(byref_probe_exe),
        "packaged_compile_artifacts": {
            key: repo_rel(path) for key, path in compile_artifacts.items()
        },
        "runtime_abi_probe_payload": abi_probe_payload,
        "byref_forwarding_probe_payload": byref_probe_payload,
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
                "action": "compile-block-arc-fixture",
                "exit_code": compile_result.returncode,
            },
            {
                "action": "link-packaged-block-arc-fixture",
                "exit_code": link_fixture_result.returncode,
                "expected_exit_code": 14,
                "actual_exit_code": linked_fixture_run.returncode,
            },
            {
                "action": "compile-packaged-block-arc-runtime-abi-probe",
                "exit_code": abi_probe_compile_result.returncode,
                "clangxx": clangxx,
            },
            {
                "action": "run-packaged-block-arc-runtime-abi-probe",
                "exit_code": 0,
            },
            {
                "action": "compile-packaged-block-byref-forwarding-probe",
                "exit_code": byref_probe_compile_result.returncode,
                "clangxx": clangxx,
            },
            {
                "action": "run-packaged-block-byref-forwarding-probe",
                "exit_code": 0,
            },
            {
                "action": "packaged-block-arc-execution-smoke",
                "exit_code": smoke_result.returncode,
                "report_paths": extract_report_paths(smoke_result.stdout),
                "fixture_list": repo_rel(smoke_fixture_list),
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
