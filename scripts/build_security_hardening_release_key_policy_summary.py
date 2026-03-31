#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "installer_update_release_key_hardening_policy.json"
RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_security_hardening.md"
OUT_DIR = ROOT / "tmp" / "reports" / "security-hardening" / "release-key-policy"
JSON_OUT = OUT_DIR / "release_key_policy_summary.json"
MD_OUT = OUT_DIR / "release_key_policy_summary.md"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    checks = {
        "all_upstream_contracts_exist": all((ROOT / path).is_file() for path in contract["upstream_contract_paths"]),
        "all_required_artifacts_exist": all((ROOT / path).is_file() for path in contract["required_artifacts"]),
        "all_required_reports_exist": all((ROOT / path).is_file() for path in contract["required_reports"]),
        "required_hardening_rules_present": len(contract["required_hardening_rules"]) >= 4,
        "runbook_mentions_release_key_hardening_semantics": "Current installer/update/release-key hardening semantics:" in runbook_text,
        "runbook_mentions_local_publication_environment": "local publication environment" in runbook_text,
        "runbook_mentions_fail_closed_linkage_drift": "security claims must fail closed if manifest, provenance, package, update, or" in runbook_text,
        "runbook_mentions_non_claims": "no remote key custody" in runbook_text or "no part of the current surface implies remote key custody" in runbook_text,
    }

    payload = {
        "contract_id": "objc3c.security.hardening.installer.update.release-key.policy.summary.v1",
        "source_contract_id": contract["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_security_hardening_release_key_policy_summary.py",
        "upstream_contract_count": len(contract["upstream_contract_paths"]),
        "required_artifact_count": len(contract["required_artifacts"]),
        "required_report_count": len(contract["required_reports"]),
        "required_hardening_rule_count": len(contract["required_hardening_rules"]),
        "non_claim_count": len(contract["non_claims"]),
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Installer Update Release-Key Hardening Policy Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Upstream contracts: `{payload['upstream_contract_count']}`\n"
        f"- Required artifacts: `{payload['required_artifact_count']}`\n"
        f"- Required reports: `{payload['required_report_count']}`\n"
        f"- Hardening rules: `{payload['required_hardening_rule_count']}`\n"
        f"- Non-claims: `{payload['non_claim_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
