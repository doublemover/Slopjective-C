#!/usr/bin/env python3
from __future__ import annotations

import json
import platform
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel

ROOT = Path(__file__).resolve().parents[1]
BOUNDARY_CONTRACT = ROOT / "tests/tooling/fixtures/platform_hardening/boundary_inventory.json"
TIER_POLICY = ROOT / "tests/tooling/fixtures/platform_hardening/platform_support_tier_policy.json"
SUPPORTED_PLATFORMS = ROOT / "tests/tooling/fixtures/packaging_channels/supported_platforms.json"
ARTIFACT_PATH = ROOT / "tmp/artifacts/platform-hardening/objc3c-platform-support-matrix.json"
SUMMARY_PATH = ROOT / "tmp/reports/platform-hardening/platform-support-matrix-summary.json"



def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload


def run_probe(command: list[str]) -> dict[str, Any]:
    result = subprocess.run(command, cwd=ROOT, text=True, capture_output=True, check=False)
    headline = ""
    for stream in (result.stdout, result.stderr):
        for line in stream.splitlines():
            stripped = line.strip()
            if stripped:
                headline = stripped
                break
        if headline:
            break
    return {
        "command": command,
        "available": result.returncode == 0,
        "exit_code": result.returncode,
        "headline": headline,
    }


def main() -> int:
    boundary = read_json(BOUNDARY_CONTRACT)
    tier_policy = read_json(TIER_POLICY)
    supported_platforms = read_json(SUPPORTED_PLATFORMS)

    python_probe = {
        "command": [sys.executable, "--version"],
        "available": True,
        "exit_code": 0,
        "headline": sys.version.splitlines()[0],
    }
    pwsh_path = shutil.which("pwsh")
    clang_path = shutil.which("clang++") or shutil.which("clang")
    pwsh_probe = run_probe([pwsh_path, "--version"]) if pwsh_path else {
        "command": ["pwsh", "--version"],
        "available": False,
        "exit_code": 127,
        "headline": "pwsh not found",
    }
    clang_probe = run_probe([clang_path, "--version"]) if clang_path else {
        "command": ["clang++", "--version"],
        "available": False,
        "exit_code": 127,
        "headline": "clang++ not found",
    }

    payload = {
        "contract_id": "objc3c.platform.hardening.support.matrix.v1",
        "schema_version": 1,
        "generated_at_utc": datetime.now(timezone.utc).isoformat().replace("+00:00", "Z"),
        "default_platform_id": supported_platforms["default_platform_id"],
        "platform_count": len(supported_platforms["supported_platforms"]),
        "platforms": supported_platforms["supported_platforms"],
        "channels": boundary["supported_channels"],
        "tiers": tier_policy["tiers"],
        "current_host": {
          "os": platform.platform(),
          "arch": platform.machine() or "",
        },
        "required_tool_probes": {
          "python": python_probe,
          "pwsh": pwsh_probe,
          "clang": clang_probe,
        },
        "claim_boundary": {
          "supported_platform_ids": boundary["supported_platform_ids"],
          "gap_claims": boundary["gap_claims"],
          "forbidden_claims": tier_policy["forbidden_claims"],
        },
        "publication_surface": {
          "package_bridge": "objc3c",
          "inspect_support_matrix_command": "build-platform-support-matrix",
          "package_command": "package-runnable-toolchain",
          "package_channels_command": "build-package-channels",
          "packaging_validation_command": "validate-packaging-channels",
          "packaging_end_to_end_command": "validate-packaging-channels-end-to-end",
          "platform_hardening_validation_command": "validate-platform-hardening",
          "platform_hardening_end_to_end_command": "validate-platform-hardening-end-to-end",
          "release_operations_command": "validate-release-operations",
          "release_operations_end_to_end_command": "validate-release-operations-end-to-end"
        }
    }

    ARTIFACT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(ARTIFACT_PATH, payload)

    summary = {
        "contract_id": "objc3c.platform.hardening.support.matrix.summary.v1",
        "status": "PASS",
        "artifact_path": repo_rel(ARTIFACT_PATH),
        "default_platform_id": payload["default_platform_id"],
        "platform_count": payload["platform_count"],
        "channel_count": len(payload["channels"]),
        "tier_count": len(payload["tiers"]),
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, summary)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print(f"artifact_path: {repo_rel(ARTIFACT_PATH)}")
    print("objc3c-platform-support-matrix: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
