#!/usr/bin/env python3
"""Validate the staged runnable objc3c toolchain end to end."""

from __future__ import annotations

import json
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
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime" / "runnable-bootstrap-e2e" / "summary.json"
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.bootstrap.e2e.summary.v1"
PACKAGE_CONTRACT_ID = "objc3c-runnable-build-install-run-package/runnable_suite-d002-v1"


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
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"invalid JSON artifact at {repo_rel(path)}: {exc}") from exc
    expect(isinstance(payload, dict), f"JSON artifact at {repo_rel(path)} did not contain an object")
    return payload


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-native-runnable-toolchain-validation" / run_id
    manifest_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    compile_out_dir = package_root / "tmp" / "artifacts" / "runnable-bootstrap-e2e" / "compile"

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
    expect(
        normalize_rel_path(str(manifest.get("package_root", ""))) == repo_rel(package_root),
        "runnable toolchain package manifest drifted from the staged package root",
    )

    for manifest_key in (
        "native_executable",
        "frontend_c_api_runner",
        "runtime_library",
        "compile_wrapper",
        "runtime_launch_contract_script",
        "compile_proof_script",
        "execution_smoke_script",
        "execution_replay_script",
        "canonical_runnable_fixture",
    ):
        relative_path = manifest.get(manifest_key)
        expect(isinstance(relative_path, str) and relative_path, f"package manifest did not publish {manifest_key}")
        candidate = package_root / normalize_rel_path(relative_path)
        expect(candidate.is_file(), f"packaged runnable toolchain missing {manifest_key} at {relative_path}")

    compile_script = package_root / normalize_rel_path(str(manifest["compile_wrapper"]))
    canonical_fixture = package_root / normalize_rel_path(str(manifest["canonical_runnable_fixture"]))
    packaged_smoke_script = package_root / normalize_rel_path(str(manifest["execution_smoke_script"]))
    packaged_replay_script = package_root / normalize_rel_path(str(manifest["execution_replay_script"]))

    compile_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(compile_script),
            str(canonical_fixture),
            "--out-dir",
            str(compile_out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=package_root,
    )
    if compile_result.returncode != 0:
        raise RuntimeError("packaged compile wrapper failed for the canonical runnable fixture")

    compile_artifacts = {
        "manifest": compile_out_dir / "module.manifest.json",
        "registration_manifest": compile_out_dir / "module.runtime-registration-manifest.json",
        "compile_provenance": compile_out_dir / "module.compile-provenance.json",
        "object": compile_out_dir / "module.obj",
    }
    for artifact_name, artifact_path in compile_artifacts.items():
        expect(artifact_path.is_file(), f"packaged compile wrapper did not publish {artifact_name} at {artifact_path}")

    compile_provenance = load_json(compile_artifacts["compile_provenance"])
    truthfulness = compile_provenance.get("compile_output_truthfulness", {})
    expect(
        isinstance(truthfulness, dict)
        and truthfulness.get("truthful") is True,
        "packaged compile wrapper did not publish a truthful compile output envelope",
    )

    smoke_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(packaged_smoke_script),
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
            str(packaged_replay_script),
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
        "runner_path": "scripts/check_objc3c_runnable_bootstrap_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "package_contract_id": manifest["contract_id"],
        "packaged_compile_fixture": repo_rel(canonical_fixture),
        "packaged_compile_out_dir": repo_rel(compile_out_dir),
        "packaged_compile_artifacts": {
            key: repo_rel(path) for key, path in compile_artifacts.items()
        },
        "packaged_command_surfaces": {
            "compile": manifest["compile_wrapper"],
            "smoke": manifest["execution_smoke_script"],
            "replay": manifest["execution_replay_script"],
        },
        "child_report_paths": [
            *extract_report_paths(package_result.stdout),
            *extract_report_paths(smoke_result.stdout),
            *extract_report_paths(replay_result.stdout),
        ],
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "command": [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", "scripts/package_objc3c_runnable_toolchain.ps1"],
                "exit_code": package_result.returncode,
                "package_root": extract_output_value(package_result.stdout, "package_root"),
                "manifest": extract_output_value(package_result.stdout, "manifest"),
            },
            {
                "action": "compile-canonical-runnable",
                "command": [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(Path(manifest["compile_wrapper"]).as_posix())],
                "exit_code": compile_result.returncode,
            },
            {
                "action": "packaged-execution-smoke",
                "command": [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(Path(manifest["execution_smoke_script"]).as_posix()), "-Limit", "12"],
                "exit_code": smoke_result.returncode,
                "report_paths": extract_report_paths(smoke_result.stdout),
            },
            {
                "action": "packaged-execution-replay",
                "command": [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(Path(manifest["execution_replay_script"]).as_posix()), "-CaseId", "canonical-runnable"],
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
