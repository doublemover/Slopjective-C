#!/usr/bin/env python3
from __future__ import annotations

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.subprocesses import run_capture
import json
from pathlib import Path
from typing import Any
from scripts.objc3c_workflow.public_command_api import public_workflow_command


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "installer_update_release_key_hardening_policy.json"
RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_security_hardening.md"
OUT_DIR = ROOT / "tmp" / "reports" / "security-hardening" / "release-key-policy"
JSON_OUT = OUT_DIR / "release_key_policy_summary.json"
MD_OUT = OUT_DIR / "release_key_policy_summary.md"

UPSTREAM_OUTPUT_PRODUCERS = {
    "tmp/artifacts/release-foundation/manifest/objc3c-release-manifest.json": ("validate-release-foundation",),
    "tmp/artifacts/release-foundation/sbom/objc3c-release-sbom.json": ("validate-release-foundation",),
    "tmp/artifacts/release-foundation/attestation/objc3c-release-attestation.json": ("validate-release-foundation",),
    "tmp/artifacts/release-operations/update-manifest/objc3c-update-manifest.json": ("validate-release-operations",),
    "tmp/artifacts/release-operations/publication/objc3c-upgrade-support-report.json": ("validate-release-operations",),
    "tmp/artifacts/release-operations/publication/objc3c-release-channel-catalog.json": ("validate-release-operations",),
    "tmp/reports/release-foundation/publication-summary.json": ("validate-release-foundation",),
    "tmp/reports/release-operations/publication-summary.json": ("validate-release-operations",),
    "tmp/reports/release-operations/end-to-end-summary.json": ("validate-release-operations",),
    "tmp/reports/distribution-credibility/publication-summary.json": (
        "check-distribution-credibility-surface",
        "check-distribution-credibility-schema-surface",
        "build-distribution-credibility-dashboard",
        "publish-distribution-credibility",
    ),
}


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload


def ensure_upstream_outputs(contract: dict[str, Any]) -> list[str]:
    """Build missing upstream release evidence through canonical workflow actions."""
    requested_actions: list[str] = []
    seen_actions: set[str] = set()
    for rel_path in [*contract["required_artifacts"], *contract["required_reports"]]:
        path_text = str(rel_path)
        if (ROOT / path_text).is_file():
            continue
        actions = UPSTREAM_OUTPUT_PRODUCERS.get(path_text)
        if actions is None:
            continue
        for action in actions:
            if action not in seen_actions:
                requested_actions.append(action)
                seen_actions.add(action)

    for action in requested_actions:
        result = run_capture(public_workflow_command(action))
        if result.returncode != 0:
            raise RuntimeError(f"failed to build upstream output via {action}")
    return requested_actions


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    try:
        upstream_actions_executed = ensure_upstream_outputs(contract)
    except RuntimeError as exc:
        print(f"release-key-policy-summary: FAIL\n- {exc}")
        return 1

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
        "upstream_actions_executed": upstream_actions_executed,
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    write_json_file(JSON_OUT, payload)
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
