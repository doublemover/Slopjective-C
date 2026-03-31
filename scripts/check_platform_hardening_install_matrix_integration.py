#!/usr/bin/env python3
from __future__ import annotations

import json
import os
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/platform_hardening/install_matrix_integration_contract.json"
SUMMARY_PATH = ROOT / "tmp/reports/platform-hardening/install-matrix-integration-summary.json"


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


def run(command: list[str]) -> None:
    result = subprocess.run(command, cwd=ROOT, text=True, check=False)
    if result.returncode != 0:
        raise RuntimeError(f"command failed with exit code {result.returncode}: {' '.join(command)}")


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    run([sys.executable, str(ROOT / "scripts" / "check_platform_hardening_build_package_validation.py")])
    run([sys.executable, str(ROOT / "scripts" / "check_platform_hardening_toolchain_range_replay.py")])

    matrix = read_json(ROOT / "tmp/artifacts/platform-hardening/objc3c-platform-support-matrix.json")
    build_package = read_json(ROOT / "tmp/reports/platform-hardening/build-package-validation-summary.json")
    toolchain_replay = read_json(ROOT / "tmp/reports/platform-hardening/toolchain-range-replay-summary.json")
    packaging_e2e = read_json(ROOT / "tmp/reports/package-channels/end-to-end-summary.json")

    install_root = ROOT / packaging_e2e["install_root"].replace("/", os.sep)
    offline_install_root = ROOT / packaging_e2e["offline_install_root"].replace("/", os.sep)
    offline_native = offline_install_root / "objc3c" / "artifacts" / "bin" / "objc3c-native.exe"

    checks = {
        "matrix_platform_is_windows_x64": matrix["default_platform_id"] == "windows-x64",
        "build_package_validation_passes": build_package["status"] == "PASS",
        "toolchain_range_replay_passes": toolchain_replay["status"] == "PASS",
        "primary_install_root_rolled_back": not (install_root / "objc3c").exists(),
        "offline_install_root_kept_native_executable": offline_native.is_file(),
    }

    summary = {
        "contract_id": "objc3c.platform.hardening.install.matrix.integration.summary.v1",
        "status": "PASS" if all(checks.values()) else "FAIL",
        "support_matrix_artifact": "tmp/artifacts/platform-hardening/objc3c-platform-support-matrix.json",
        "build_package_validation_summary": "tmp/reports/platform-hardening/build-package-validation-summary.json",
        "toolchain_range_replay_summary": "tmp/reports/platform-hardening/toolchain-range-replay-summary.json",
        "packaging_end_to_end_summary": "tmp/reports/package-channels/end-to-end-summary.json",
        "checks": checks,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-platform-install-matrix-integration: PASS")
    return 0 if summary["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
