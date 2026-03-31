#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/platform_hardening/toolchain_range_replay_contract.json"
MATRIX_GENERATOR = ROOT / "scripts" / "build_objc3c_platform_support_matrix.py"
MATRIX_PATH = ROOT / "tmp" / "artifacts" / "platform-hardening" / "objc3c-platform-support-matrix.json"
PROBE_SCRIPT = ROOT / "scripts" / "probe_objc3c_llvm_capabilities.py"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "platform-hardening" / "toolchain-range-replay-summary.json"


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
    probe_summary = ROOT / contract["toolchain_probe_summary"].replace("/", "\\")

    run([sys.executable, str(MATRIX_GENERATOR)])
    run([sys.executable, str(PROBE_SCRIPT), "--summary-out", repo_rel(probe_summary)])
    run([sys.executable, str(ROOT / "scripts" / "check_objc3c_release_operations_integration.py")])
    run([sys.executable, str(ROOT / "scripts" / "check_objc3c_release_operations_end_to_end.py")])

    matrix = read_json(MATRIX_PATH)
    capabilities = read_json(probe_summary)
    release_integration = read_json(ROOT / "tmp/reports/release-operations/integration-summary.json")
    release_e2e = read_json(ROOT / "tmp/reports/release-operations/end-to-end-summary.json")

    checks = {
        "matrix_default_platform_is_windows_x64": matrix["default_platform_id"] == "windows-x64",
        "toolchain_probe_passes": capabilities.get("ok") is True,
        "toolchain_parity_ready": capabilities.get("sema_type_system_parity", {}).get("parity_ready") is True,
        "release_integration_passes": release_integration.get("status") == "PASS",
        "release_end_to_end_passes": release_e2e.get("status") == "PASS",
        "stable_artifacts_published": len(release_e2e.get("stable_artifacts", {})) >= 3,
    }

    summary = {
        "contract_id": "objc3c.platform.hardening.toolchain.range.replay.summary.v1",
        "status": "PASS" if all(checks.values()) else "FAIL",
        "support_matrix_artifact": repo_rel(MATRIX_PATH),
        "toolchain_probe_summary": repo_rel(probe_summary),
        "release_operations_integration": "tmp/reports/release-operations/integration-summary.json",
        "release_operations_end_to_end": "tmp/reports/release-operations/end-to-end-summary.json",
        "required_toolchain_claims": contract["required_toolchain_claims"],
        "checks": checks,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-platform-toolchain-range-replay: PASS")
    return 0 if summary["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
