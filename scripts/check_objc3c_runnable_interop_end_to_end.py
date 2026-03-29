#!/usr/bin/env python3
"""Validate runnable mixed-module interop execution end to end from the staged package root."""

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
REPORT_PATH = ROOT / "tmp" / "reports" / "runtime" / "runnable-interop-e2e" / "summary.json"
SUMMARY_CONTRACT_ID = "objc3c.runtime.runnable.interop.e2e.summary.v1"
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
    expect(path.is_file(), f"expected JSON artifact was not published: {path}")
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON artifact at {path} did not contain an object")
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
        "bridge_header": compile_out_dir / "module.interop-bridge.h",
        "bridge_modulemap": compile_out_dir / "module.interop-bridge.modulemap",
        "bridge_json": compile_out_dir / "module.interop-bridge.json",
        "object": compile_out_dir / "module.obj",
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
        raise RuntimeError(f"packaged interop probe compile failed for {probe_source}")


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-interop-e2e" / run_id
    manifest_path = package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    artifacts_root = package_root / "tmp" / "artifacts" / "runnable-interop-e2e"

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
        "interop_runtime_fixture",
        "interop_runtime_consumer_fixture",
        "interop_header_bridge_fixture",
        "interop_header_bridge_consumer_fixture",
        "interop_packaging_probe",
        "interop_bridge_generation_probe",
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
    provider_fixture = package_root / normalize_rel_path(str(manifest["interop_runtime_fixture"]))
    consumer_fixture = package_root / normalize_rel_path(str(manifest["interop_runtime_consumer_fixture"]))
    header_provider_fixture = package_root / normalize_rel_path(str(manifest["interop_header_bridge_fixture"]))
    header_consumer_fixture = package_root / normalize_rel_path(str(manifest["interop_header_bridge_consumer_fixture"]))
    packaging_probe = package_root / normalize_rel_path(str(manifest["interop_packaging_probe"]))
    bridge_probe = package_root / normalize_rel_path(str(manifest["interop_bridge_generation_probe"]))

    provider_artifacts = compile_fixture(
        compile_script,
        provider_fixture,
        artifacts_root / "provider" / "compile",
        cwd=package_root,
        extra_args=("--objc3-bootstrap-registration-order-ordinal", "1"),
    )
    consumer_compile_dir = artifacts_root / "consumer" / "compile"
    consumer_compile_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(compile_script),
            str(consumer_fixture),
            "--out-dir",
            str(consumer_compile_dir),
            "--emit-prefix",
            "module",
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_artifacts["runtime_import_surface"]),
        ],
        cwd=package_root,
    )
    if consumer_compile_result.returncode != 0:
        raise RuntimeError("packaged compile wrapper failed for the interop consumer fixture")

    consumer_artifacts = {
        "manifest": consumer_compile_dir / "module.manifest.json",
        "registration_manifest": consumer_compile_dir / "module.runtime-registration-manifest.json",
        "cross_module_link_plan": consumer_compile_dir / "module.cross-module-runtime-link-plan.json",
        "cross_module_linker_rsp": consumer_compile_dir / "module.cross-module-runtime-linker-options.rsp",
        "runtime_metadata_linker_rsp": consumer_compile_dir / "module.runtime-metadata-linker-options.rsp",
        "object": consumer_compile_dir / "module.obj",
    }
    for artifact_path in consumer_artifacts.values():
        expect(artifact_path.is_file(), f"packaged interop consumer compile did not publish {artifact_path}")

    provider_bridge_json = load_json(provider_artifacts["bridge_json"])
    provider_import_surface = load_json(provider_artifacts["runtime_import_surface"])
    consumer_link_plan = load_json(consumer_artifacts["cross_module_link_plan"])
    expect(
        consumer_link_plan.get("module_image_count") == 2
        and consumer_link_plan.get("direct_import_input_count") == 1,
        "packaged interop consumer link plan drifted from the two-image mixed-module topology",
    )
    expect(
        consumer_link_plan.get("expected_interop_bridge_header_artifact_relative_path")
        == "module.interop-bridge.h"
        and consumer_link_plan.get("expected_interop_bridge_module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and consumer_link_plan.get("expected_interop_bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "packaged interop consumer link plan drifted from the bridge artifact paths",
    )
    expect(
        consumer_link_plan.get("interop_ffi_imported_module_count") == 1
        and consumer_link_plan.get("interop_header_module_bridge_imported_module_count") == 1,
        "packaged interop consumer link plan drifted from the imported ffi and bridge module counts",
    )
    expect(
        isinstance(provider_import_surface.get("module_name"), str)
        and provider_import_surface.get("module_name") != ""
        and provider_bridge_json.get("header_artifact_relative_path") == "module.interop-bridge.h"
        and provider_bridge_json.get("module_artifact_relative_path") == "module.interop-bridge.modulemap"
        and provider_bridge_json.get("bridge_artifact_relative_path") == "module.interop-bridge.json",
        "packaged interop provider artifacts drifted from the emitted provider identity or bridge artifact paths",
    )

    header_provider_artifacts = compile_fixture(
        compile_script,
        header_provider_fixture,
        artifacts_root / "header-provider" / "compile",
        cwd=package_root,
        extra_args=("--objc3-bootstrap-registration-order-ordinal", "1"),
    )
    header_consumer_compile_dir = artifacts_root / "header-consumer" / "compile"
    header_consumer_compile_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(compile_script),
            str(header_consumer_fixture),
            "--out-dir",
            str(header_consumer_compile_dir),
            "--emit-prefix",
            "module",
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(header_provider_artifacts["runtime_import_surface"]),
        ],
        cwd=package_root,
    )
    if header_consumer_compile_result.returncode != 0:
        raise RuntimeError("packaged compile wrapper failed for the header-bridge consumer fixture")

    header_consumer_link_plan = load_json(
        header_consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
    )
    expect(
        "m274_header_module_bridge_provider"
        in header_consumer_link_plan.get(
            "interop_header_module_bridge_imported_module_names_lexicographic", []
        ),
        "packaged header-bridge consumer link plan drifted from the imported bridge provider identity",
    )

    clangxx = find_clangxx()
    packaging_probe_exe = artifacts_root / "bridge_packaging_toolchain_probe.exe"
    compile_probe(clangxx, packaging_probe, runtime_library, packaging_probe_exe, cwd=package_root)
    packaging_probe_payload = parse_key_value_output(
        run_capture([str(packaging_probe_exe)], cwd=package_root),
        "packaged interop packaging-topology probe",
    )
    for field_name, expected_value in {
        "copy_status": 0,
        "packaging_topology_ready": 1,
        "operator_visible_evidence_ready": 1,
        "header_generation_ready": 0,
        "module_generation_ready": 0,
        "bridge_generation_ready": 0,
        "deterministic": 1,
    }.items():
        expect(
            packaging_probe_payload.get(field_name) == expected_value,
            f"expected packaged interop packaging probe to preserve {field_name}",
        )
    expect(
        packaging_probe_payload.get("runtime_support_library_archive_relative_path")
        == consumer_link_plan.get("runtime_support_library_archive_relative_path"),
        "packaged interop packaging probe drifted from the emitted runtime archive path",
    )

    bridge_probe_exe = artifacts_root / "header_module_bridge_generation_probe.exe"
    compile_probe(clangxx, bridge_probe, runtime_library, bridge_probe_exe, cwd=package_root)
    bridge_probe_payload = parse_key_value_output(
        run_capture([str(bridge_probe_exe)], cwd=package_root),
        "packaged interop bridge-generation probe",
    )
    for field_name, expected_value in {
        "copy_status": 0,
        "runtime_generation_ready": 1,
        "cross_module_packaging_ready": 1,
        "header_generation_ready": 1,
        "module_generation_ready": 1,
        "bridge_generation_ready": 1,
        "deterministic": 1,
    }.items():
        expect(
            bridge_probe_payload.get(field_name) == expected_value,
            f"expected packaged interop bridge probe to preserve {field_name}",
        )
    expect(
        bridge_probe_payload.get("header_artifact_relative_path")
        == consumer_link_plan.get("expected_interop_bridge_header_artifact_relative_path")
        == provider_bridge_json.get("header_artifact_relative_path"),
        "packaged interop bridge probe drifted from the emitted bridge header path",
    )
    expect(
        bridge_probe_payload.get("module_artifact_relative_path")
        == consumer_link_plan.get("expected_interop_bridge_module_artifact_relative_path")
        == provider_bridge_json.get("module_artifact_relative_path"),
        "packaged interop bridge probe drifted from the emitted bridge modulemap path",
    )
    expect(
        bridge_probe_payload.get("bridge_artifact_relative_path")
        == consumer_link_plan.get("expected_interop_bridge_artifact_relative_path")
        == provider_bridge_json.get("bridge_artifact_relative_path"),
        "packaged interop bridge probe drifted from the emitted bridge json path",
    )

    smoke_result = run_capture(
        [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(smoke_script)],
        cwd=package_root,
    )
    if smoke_result.returncode != 0:
        raise RuntimeError("packaged execution smoke validation failed")

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
        "runner_path": "scripts/check_objc3c_runnable_interop_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "packaged_provider_fixture": repo_rel(provider_fixture),
        "packaged_consumer_fixture": repo_rel(consumer_fixture),
        "packaged_packaging_probe": repo_rel(packaging_probe),
        "packaged_bridge_probe": repo_rel(bridge_probe),
        "packaged_probe_executables": {
            "packaging_probe": repo_rel(packaging_probe_exe),
            "bridge_probe": repo_rel(bridge_probe_exe),
        },
        "provider_bridge_json_path": repo_rel(provider_artifacts["bridge_json"]),
        "consumer_cross_module_link_plan_path": repo_rel(
            consumer_artifacts["cross_module_link_plan"]
        ),
        "packaging_probe_payload": packaging_probe_payload,
        "bridge_probe_payload": bridge_probe_payload,
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
                "action": "compile-interop-provider-fixture",
                "exit_code": 0,
            },
            {
                "action": "compile-interop-consumer-fixture",
                "exit_code": consumer_compile_result.returncode,
            },
            {
                "action": "compile-header-bridge-provider-fixture",
                "exit_code": 0,
            },
            {
                "action": "compile-header-bridge-consumer-fixture",
                "exit_code": header_consumer_compile_result.returncode,
            },
            {
                "action": "compile-packaged-interop-probes",
                "exit_code": 0,
                "clangxx": clangxx,
            },
            {
                "action": "run-packaged-interop-probes",
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
