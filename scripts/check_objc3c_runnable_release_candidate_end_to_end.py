#!/usr/bin/env python3
"""Validate runnable release-candidate packaging and validation end to end from the staged package root."""

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
REPORT_PATH = (
    ROOT / "tmp" / "reports" / "runtime" / "runnable-release-candidate-e2e" / "summary.json"
)
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.release.candidate.e2e.summary.v1"
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
        raise RuntimeError(f"packaged release-candidate probe compile failed for {probe_source}")


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-release-candidate-e2e" / run_id
    manifest_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    compile_out_dir = package_root / "tmp" / "artifacts" / "runnable-release-candidate-e2e" / "compile"
    validate_out_dir = package_root / "tmp" / "artifacts" / "runnable-release-candidate-e2e" / "validate"
    claim_probe_exe = package_root / "tmp" / "artifacts" / "runnable-release-candidate-e2e" / "claim_probe.exe"
    evidence_probe_exe = package_root / "tmp" / "artifacts" / "runnable-release-candidate-e2e" / "evidence_probe.exe"

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
        "native_executable",
        "compile_wrapper",
        "runtime_library",
        "execution_smoke_script",
        "execution_replay_script",
        "runtime_public_header",
        "runtime_internal_header",
        "release_evidence_dashboard_schema",
        "release_evidence_gate_script",
        "release_evidence_runbook",
        "release_candidate_fixture",
        "release_candidate_claim_probe",
        "release_candidate_evidence_probe",
    )
    for manifest_key in required_manifest_keys:
        relative_path = manifest.get(manifest_key)
        expect(isinstance(relative_path, str) and relative_path, f"package manifest did not publish {manifest_key}")
        candidate = package_root / normalize_rel_path(relative_path)
        expect(candidate.is_file(), f"packaged runnable toolchain missing {manifest_key} at {relative_path}")

    native_executable = package_root / normalize_rel_path(str(manifest["native_executable"]))
    compile_script = package_root / normalize_rel_path(str(manifest["compile_wrapper"]))
    runtime_library = package_root / normalize_rel_path(str(manifest["runtime_library"]))
    smoke_script = package_root / normalize_rel_path(str(manifest["execution_smoke_script"]))
    replay_script = package_root / normalize_rel_path(str(manifest["execution_replay_script"]))
    release_candidate_fixture = package_root / normalize_rel_path(str(manifest["release_candidate_fixture"]))
    claim_probe = package_root / normalize_rel_path(str(manifest["release_candidate_claim_probe"]))
    evidence_probe = package_root / normalize_rel_path(str(manifest["release_candidate_evidence_probe"]))

    compile_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(compile_script),
            str(release_candidate_fixture),
            "--out-dir",
            str(compile_out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=package_root,
    )
    if compile_result.returncode != 0:
        raise RuntimeError("packaged compile wrapper failed for the release-candidate fixture")

    compile_artifacts = {
        "manifest": compile_out_dir / "module.manifest.json",
        "registration_manifest": compile_out_dir / "module.runtime-registration-manifest.json",
        "compile_provenance": compile_out_dir / "module.compile-provenance.json",
        "conformance_report": compile_out_dir / "module.objc3-conformance-report.json",
        "conformance_publication": compile_out_dir / "module.objc3-conformance-publication.json",
        "advanced_feature_gate": compile_out_dir / "module.objc3-advanced-feature-gate.json",
        "release_candidate_matrix": compile_out_dir / "module.objc3-release-candidate-matrix.json",
        "object": compile_out_dir / "module.obj",
    }
    for artifact_path in compile_artifacts.values():
        expect(artifact_path.is_file(), f"packaged compile wrapper did not publish {artifact_path}")

    validation_result = run_capture(
        [
            str(native_executable),
            "--validate-objc3-conformance",
            str(compile_artifacts["conformance_report"]),
            "--out-dir",
            str(validate_out_dir),
            "--emit-prefix",
            "module",
            "--emit-objc3-conformance-format",
            "json",
        ],
        cwd=package_root,
    )
    if validation_result.returncode != 0:
        raise RuntimeError("packaged release-candidate validation failed")

    validate_artifacts = sorted(path.name for path in validate_out_dir.glob("module.objc3-*.json"))
    expect(
        validate_artifacts
        == [
            "module.objc3-advanced-feature-gate.json",
            "module.objc3-conformance-validation.json",
            "module.objc3-dashboard-status.json",
            "module.objc3-release-candidate-matrix.json",
            "module.objc3-release-evidence-operation.json",
        ],
        "packaged release-candidate validation drifted from the final artifact inventory",
    )

    clangxx = find_clangxx()
    compile_probe(clangxx, claim_probe, runtime_library, claim_probe_exe, cwd=package_root)
    compile_probe(clangxx, evidence_probe, runtime_library, evidence_probe_exe, cwd=package_root)

    claim_payload = parse_key_value_output(
        run_capture([str(claim_probe_exe)], cwd=package_root),
        "packaged release-candidate claim ABI probe",
    )
    expect(
        claim_payload.get("copy_status") == 0
        and claim_payload.get("claim_bundle_ready") == 1
        and claim_payload.get("deterministic") == 1,
        "expected packaged release-candidate claim ABI probe to publish a ready deterministic snapshot",
    )
    expect(
        claim_payload.get("dashboard_schema_path")
        == "schemas/objc3-conformance-dashboard-status-v1.schema.json"
        and claim_payload.get("gate_script_path") == "scripts/check_release_evidence.py"
        and claim_payload.get("runbook_reference_path")
        == "spec/conformance/release_evidence_gate_maintenance.md",
        "expected packaged release-candidate claim ABI probe to preserve the packaged release evidence references",
    )

    evidence_payload = parse_key_value_output(
        run_capture([str(evidence_probe_exe)], cwd=package_root),
        "packaged release-candidate evidence probe",
    )
    expect(
        evidence_payload.get("copy_status") == 0
        and evidence_payload.get("validation_artifact_ready") == 1
        and evidence_payload.get("release_evidence_operation_ready") == 1
        and evidence_payload.get("dashboard_status_ready") == 1
        and evidence_payload.get("advanced_feature_gate_ready") == 1
        and evidence_payload.get("release_candidate_matrix_ready") == 1
        and evidence_payload.get("deprecated_paths_shutdown") == 1
        and evidence_payload.get("deterministic") == 1,
        "expected packaged release-candidate evidence probe to publish a ready deterministic implementation snapshot",
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
        "runner_path": "scripts/check_objc3c_runnable_release_candidate_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "packaged_compile_fixture": repo_rel(release_candidate_fixture),
        "packaged_validate_out_dir": repo_rel(validate_out_dir),
        "packaged_compile_artifacts": {
            key: repo_rel(path) for key, path in compile_artifacts.items()
        },
        "packaged_validate_artifacts": validate_artifacts,
        "release_candidate_claim_probe_payload": claim_payload,
        "release_candidate_evidence_probe_payload": evidence_payload,
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
                "action": "compile-release-candidate-fixture",
                "exit_code": compile_result.returncode,
            },
            {
                "action": "validate-release-candidate-fixture",
                "exit_code": validation_result.returncode,
            },
            {
                "action": "compile-release-candidate-claim-probe",
                "exit_code": 0,
                "clangxx": clangxx,
            },
            {
                "action": "compile-release-candidate-evidence-probe",
                "exit_code": 0,
                "clangxx": clangxx,
            },
            {
                "action": "run-release-candidate-claim-probe",
                "exit_code": 0,
            },
            {
                "action": "run-release-candidate-evidence-probe",
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
