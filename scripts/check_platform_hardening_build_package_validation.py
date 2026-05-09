#!/usr/bin/env python3
from __future__ import annotations

import json
import platform
import os
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.public_runner import public_workflow_command
from objc3c_tooling.subprocesses import python_script_command, run_capture

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/platform_hardening/build_package_validation_contract.json"
MATRIX_GENERATOR = ROOT / "scripts" / "build_objc3c_platform_support_matrix.py"
MATRIX_PATH = ROOT / "tmp" / "artifacts" / "platform-hardening" / "objc3c-platform-support-matrix.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "platform-hardening" / "build-package-validation-summary.json"



def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload




def host_matches_supported_platform(default_platform_id: str) -> bool:
    host_os = platform.system().lower()
    host_arch = (platform.machine() or "").lower()
    return default_platform_id == "windows-x64" and host_os == "windows" and host_arch in {"amd64", "x86_64"}


def main() -> int:
    run_capture(python_script_command(MATRIX_GENERATOR))
    contract = read_json(CONTRACT_PATH)
    matrix = read_json(MATRIX_PATH)

    for field_name in contract["required_matrix_fields"]:
        if field_name not in matrix:
            raise RuntimeError(f"platform support matrix missing required field {field_name}")
    if not host_matches_supported_platform(str(matrix["default_platform_id"])):
        raise RuntimeError("current host fell outside the checked-in supported platform matrix")
    for tool_name, probe in matrix["required_tool_probes"].items():
        if not probe.get("available"):
            raise RuntimeError(f"required tool probe failed for {tool_name}")

    steps: list[dict[str, Any]] = []
    command_map = {
        "build-native-binaries": public_workflow_command("build-native-binaries"),
        "package-runnable-toolchain": public_workflow_command("package-runnable-toolchain"),
        "build-package-channels": python_script_command(ROOT / "scripts" / "build_objc3c_package_channels.py"),
        "check-packaging-channels-end-to-end": python_script_command(ROOT / "scripts" / "check_objc3c_packaging_channels_end_to_end.py"),
    }
    for step_name in contract["required_steps"]:
        command = command_map[step_name]
        result = run_capture(command)
        steps.append({
            "step": step_name,
            "command": command,
            "exit_code": result.returncode,
            "ok": result.returncode == 0,
        })
        if result.returncode != 0:
            raise RuntimeError(f"platform build/package step failed: {step_name}")

    for raw_path in contract["required_reports"]:
        report_path = ROOT / raw_path.replace("/", "\\")
        if not report_path.is_file():
            raise RuntimeError(f"required package report missing: {raw_path}")

    summary = {
        "contract_id": "objc3c.platform.hardening.build.package.validation.summary.v1",
        "status": "PASS",
        "support_matrix_artifact": repo_rel(MATRIX_PATH),
        "default_platform_id": matrix["default_platform_id"],
        "current_host": matrix["current_host"],
        "steps": steps,
        "required_reports": contract["required_reports"],
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-platform-build-package-validation: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
