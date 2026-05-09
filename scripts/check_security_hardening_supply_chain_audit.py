#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import python_script_command, run_completed as run_step


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "supply_chain_audit_contract.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "security-hardening" / "supply-chain-audit-summary.json"

STEP_COMMANDS = {
    "build_security_hardening_response_policy_summary": python_script_command("scripts/build_security_hardening_response_policy_summary.py"),
    "build_security_hardening_macro_trust_policy_summary": python_script_command("scripts/build_security_hardening_macro_trust_policy_summary.py"),
    "build_security_hardening_release_key_policy_summary": python_script_command("scripts/build_security_hardening_release_key_policy_summary.py"),
    "check_release_evidence": python_script_command("scripts/check_release_evidence.py"),
    "check_source_hygiene_authenticity": python_script_command("scripts/check_source_hygiene_authenticity.py"),
    "publish_objc3c_release_provenance": python_script_command("scripts/publish_objc3c_release_provenance.py"),
    "build_objc3c_update_manifest": python_script_command("scripts/build_objc3c_update_manifest.py"),
    "publish_objc3c_release_operations_metadata": python_script_command("scripts/publish_objc3c_release_operations_metadata.py"),
    "publish_objc3c_distribution_trust_report": python_script_command("scripts/publish_objc3c_distribution_trust_report.py")
}



def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
    return payload




def main() -> int:
    contract = read_json(CONTRACT_PATH)
    steps: list[dict[str, Any]] = []
    failures: list[str] = []

    for step_name in contract["required_steps"]:
        command = STEP_COMMANDS[step_name]
        result = run_step(command)
        steps.append(
            {
                "step": step_name,
                "command": command,
                "exit_code": result.returncode,
                "stdout": result.stdout,
                "stderr": result.stderr,
            }
        )
        if result.returncode != 0:
            failures.append(f"{step_name} failed")
            break

    for raw_path in contract["required_artifacts"]:
        path = ROOT / raw_path
        if not path.is_file():
            failures.append(f"missing required artifact {raw_path}")

    for raw_path in contract["required_reports"]:
        path = ROOT / raw_path
        if not path.is_file():
            failures.append(f"missing required report {raw_path}")

    release_evidence = read_json(ROOT / "tmp/reports/release_evidence/evidence-index.json") if (ROOT / "tmp/reports/release_evidence/evidence-index.json").is_file() else {}
    update_manifest = read_json(ROOT / "tmp/artifacts/release-operations/update-manifest/objc3c-update-manifest.json") if (ROOT / "tmp/artifacts/release-operations/update-manifest/objc3c-update-manifest.json").is_file() else {}
    upgrade_support_report = read_json(ROOT / "tmp/artifacts/release-operations/publication/objc3c-upgrade-support-report.json") if (ROOT / "tmp/artifacts/release-operations/publication/objc3c-upgrade-support-report.json").is_file() else {}
    channel_catalog = read_json(ROOT / "tmp/artifacts/release-operations/publication/objc3c-release-channel-catalog.json") if (ROOT / "tmp/artifacts/release-operations/publication/objc3c-release-channel-catalog.json").is_file() else {}
    trust_report = read_json(ROOT / "tmp/artifacts/distribution-credibility/report/objc3c-distribution-trust-report.json") if (ROOT / "tmp/artifacts/distribution-credibility/report/objc3c-distribution-trust-report.json").is_file() else {}

    checks = {
        "release_evidence_schema_matches": release_evidence.get("schema_id") == "objc3-conformance-evidence-index/v1",
        "update_manifest_has_platform_support_matrix": isinstance(update_manifest.get("platform_support_matrix"), str) and bool(update_manifest.get("platform_support_matrix")),
        "upgrade_support_report_has_platform_support_matrix": isinstance(upgrade_support_report.get("platform_support_matrix"), str) and bool(upgrade_support_report.get("platform_support_matrix")),
        "channel_catalog_has_platform_support_matrix": isinstance(channel_catalog.get("platform_support_matrix"), str) and bool(channel_catalog.get("platform_support_matrix")),
        "trust_report_passes": trust_report.get("status") == "PASS",
        "trust_report_references_release_operations": any("release-operations" in str(path) for path in trust_report.get("evidence_paths", [])),
    }
    for key, passed in checks.items():
        if not passed:
            failures.append(f"audit check failed: {key}")

    payload = {
        "contract_id": "objc3c.security.hardening.supply.chain.audit.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "steps": steps,
        "required_artifacts": contract["required_artifacts"],
        "required_reports": contract["required_reports"],
        "checks": checks,
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-security-hardening-supply-chain-audit: PASS" if not failures else "objc3c-security-hardening-supply-chain-audit: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
