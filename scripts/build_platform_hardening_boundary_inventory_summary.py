#!/usr/bin/env python3
from __future__ import annotations

import json
import platform
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/platform_hardening/boundary_inventory.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_platform_hardening.md"
SUPPORTED_PLATFORMS_PATH = ROOT / "tests/tooling/fixtures/packaging_channels/supported_platforms.json"
OUT_DIR = ROOT / "tmp/reports/platform-hardening/boundary-inventory"
JSON_OUT = OUT_DIR / "boundary_inventory_summary.json"
MD_OUT = OUT_DIR / "boundary_inventory_summary.md"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload


def resolve_repo_path(raw_path: str) -> Path:
    path = Path(raw_path)
    return path if path.is_absolute() else ROOT / path


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
        "exit_code": result.returncode,
        "headline": headline,
        "available": result.returncode == 0,
    }


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    supported_platforms = read_json(SUPPORTED_PLATFORMS_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    python_probe = {
        "command": [sys.executable, "--version"],
        "exit_code": 0,
        "headline": sys.version.splitlines()[0],
        "available": True,
    }
    pwsh_path = shutil.which("pwsh")
    pwsh_probe = run_probe([pwsh_path, "--version"]) if pwsh_path else {
        "command": ["pwsh", "--version"],
        "exit_code": 127,
        "headline": "pwsh not found",
        "available": False,
    }
    clang_path = shutil.which("clang++") or shutil.which("clang")
    clang_probe = run_probe([clang_path, "--version"]) if clang_path else {
        "command": ["clang++", "--version"],
        "exit_code": 127,
        "headline": "clang++ not found",
        "available": False,
    }

    supported_platform_entries = supported_platforms.get("supported_platforms", [])
    expect(isinstance(supported_platform_entries, list), "supported_platforms.json did not publish supported_platforms")

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_platform_hardening_boundary_inventory_summary.py",
        "all_authoritative_code_paths_exist": all(resolve_repo_path(path).exists() for path in contract["authoritative_code_paths"]),
        "all_policy_contract_paths_exist": all(resolve_repo_path(path).is_file() for path in contract["policy_contract_paths"]),
        "supported_platform_fixture_matches_boundary": sorted(entry["platform_id"] for entry in supported_platform_entries) == sorted(contract["supported_platform_ids"]),
        "default_platform_is_supported": supported_platforms.get("default_platform_id") in contract["supported_platform_ids"],
        "runbook_mentions_current_support_matrix": "## Current Support Matrix" in runbook_text,
        "runbook_mentions_tier_1_windows_x64": "`Tier 1`" in runbook_text and "`windows-x64`" in runbook_text,
        "runbook_mentions_fail_closed_unsupported_hosts": "Unsupported hosts and unsupported toolchain shapes must not degrade into vague" in runbook_text,
        "runbook_mentions_no_cross_platform_parity_claim": "no cross-platform parity claim exists today" in runbook_text,
        "required_tools_detected_on_current_host": python_probe["available"] and pwsh_probe["available"] and clang_probe["available"],
    }

    payload = {
        "contract_id": "objc3c.platform.hardening.boundary.inventory.summary.v1",
        "source_contract_id": contract["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_platform_hardening_boundary_inventory_summary.py",
        "authoritative_code_path_count": len(contract["authoritative_code_paths"]),
        "policy_contract_path_count": len(contract["policy_contract_paths"]),
        "public_command_count": len(contract["public_commands"]),
        "public_action_count": len(contract["public_actions"]),
        "report_path_count": len(contract["report_paths"]),
        "supported_platform_count": len(contract["supported_platform_ids"]),
        "supported_channel_count": len(contract["supported_channels"]),
        "gap_claim_count": len(contract["gap_claims"]),
        "host_probe": {
            "os": platform.platform(),
            "arch": platform.machine() or "",
            "python": python_probe,
            "pwsh": pwsh_probe,
            "clang": clang_probe,
        },
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Platform Hardening Boundary Inventory Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Authoritative code paths: `{payload['authoritative_code_path_count']}`\n"
        f"- Policy contracts: `{payload['policy_contract_path_count']}`\n"
        f"- Public commands: `{payload['public_command_count']}`\n"
        f"- Public actions: `{payload['public_action_count']}`\n"
        f"- Supported platforms: `{payload['supported_platform_count']}`\n"
        f"- Supported channels: `{payload['supported_channel_count']}`\n"
        f"- Gap claims: `{payload['gap_claim_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
