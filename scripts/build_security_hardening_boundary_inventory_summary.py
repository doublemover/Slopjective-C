#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "boundary_inventory.json"
RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_security_hardening.md"
OUT_DIR = ROOT / "tmp" / "reports" / "security-hardening" / "boundary-inventory"
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


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_security_hardening_boundary_inventory_summary.py",
        "all_authoritative_code_paths_exist": all(resolve_repo_path(path).exists() for path in contract["authoritative_code_paths"]),
        "all_policy_contract_paths_exist": all(resolve_repo_path(path).is_file() for path in contract["policy_contract_paths"]),
        "all_macro_security_fixture_paths_exist": all(resolve_repo_path(path).is_file() for path in contract["macro_security_fixture_paths"]),
        "runbook_mentions_current_security_posture": "## Current Security Posture" in runbook_text,
        "runbook_mentions_macro_package_and_provenance": "### Macro, Package, And Provenance Trust" in runbook_text,
        "runbook_mentions_installer_update_and_release_trust": "### Installer, Update, And Release Trust" in runbook_text,
        "runbook_mentions_runtime_hardening": "### Runtime Hardening And Memory-Safety Boundaries" in runbook_text,
        "runbook_mentions_disclosure_and_response": "### Disclosure And Response Boundary" in runbook_text,
        "runbook_mentions_no_hosted_advisory_service": "no hosted advisory service" in runbook_text,
    }

    payload = {
        "contract_id": "objc3c.security.hardening.boundary.inventory.summary.v1",
        "source_contract_id": contract["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_security_hardening_boundary_inventory_summary.py",
        "authoritative_code_path_count": len(contract["authoritative_code_paths"]),
        "policy_contract_path_count": len(contract["policy_contract_paths"]),
        "macro_security_fixture_count": len(contract["macro_security_fixture_paths"]),
        "public_command_count": len(contract["public_commands"]),
        "public_action_count": len(contract["public_actions"]),
        "report_path_count": len(contract["report_paths"]),
        "gap_claim_count": len(contract["gap_claims"]),
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Security Hardening Boundary Inventory Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Authoritative code paths: `{payload['authoritative_code_path_count']}`\n"
        f"- Policy contracts: `{payload['policy_contract_path_count']}`\n"
        f"- Macro security fixtures: `{payload['macro_security_fixture_count']}`\n"
        f"- Public commands: `{payload['public_command_count']}`\n"
        f"- Public actions: `{payload['public_action_count']}`\n"
        f"- Report paths: `{payload['report_path_count']}`\n"
        f"- Gap claims: `{payload['gap_claim_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
