#!/usr/bin/env python3
"""Run the long-horizon operations closeout gate over live evidence."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "tmp" / "reports" / "long-horizon-operations" / "closeout-gate"
JSON_OUT = OUT_DIR / "long_horizon_operations_closeout_gate.json"
MD_OUT = OUT_DIR / "long_horizon_operations_closeout_gate.md"
SUMMARY_CONTRACT_ID = "objc3c.long_horizon_operations.closeout.gate.v1"

STEPS = [
    ("build-boundary-inventory-summary", [sys.executable, "scripts/build_long_horizon_operations_boundary_inventory_summary.py"]),
    ("build-deprecation-policy-summary", [sys.executable, "scripts/build_long_horizon_operations_deprecation_policy_summary.py"]),
    ("build-migration-rollback-summary", [sys.executable, "scripts/build_long_horizon_operations_migration_rollback_summary.py"]),
    ("build-aging-cadence-summary", [sys.executable, "scripts/build_long_horizon_operations_aging_cadence_summary.py"]),
    ("build-artifact-contract-summary", [sys.executable, "scripts/build_long_horizon_operations_artifact_contract_summary.py"]),
    ("check-long-horizon-operations-integration", [sys.executable, "scripts/check_objc3c_long_horizon_operations_integration.py"]),
    ("publish-long-horizon-operations-metadata", [sys.executable, "scripts/publish_objc3c_long_horizon_operations_metadata.py"]),
    ("render-public-command-surface", [sys.executable, "scripts/render_objc3c_public_command_surface.py"]),
    ("build-public-command-contract", [sys.executable, "scripts/build_objc3c_public_command_contract.py"]),
    ("check-public-command-budget", [sys.executable, "scripts/check_objc3c_public_command_budget.py"]),
    ("check-documentation-surface", [sys.executable, "scripts/check_documentation_surface.py"]),
    ("check-repo-superclean-surface", [sys.executable, "scripts/check_repo_superclean_surface.py"]),
    ("build-residue-authenticity-inventory", [sys.executable, "scripts/build_residue_authenticity_inventory.py"]),
    ("check-source-hygiene-authenticity", [sys.executable, "scripts/check_source_hygiene_authenticity.py"]),
]

REPORT_ROOT = ROOT / "tmp" / "reports" / "long-horizon-operations"
ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "long-horizon-operations"
BOUNDARY_SUMMARY = REPORT_ROOT / "boundary-inventory-summary.json"
DEPRECATION_SUMMARY = REPORT_ROOT / "deprecation-compatibility-policy-summary.json"
MIGRATION_SUMMARY = REPORT_ROOT / "migration-rollback-support-window-summary.json"
AGING_SUMMARY = REPORT_ROOT / "aging-regression-release-cadence-summary.json"
ARTIFACT_CONTRACT_SUMMARY = REPORT_ROOT / "artifact-contract-summary.json"
EVIDENCE_SUMMARY = REPORT_ROOT / "evidence-summary.json"
INTEGRATION_SUMMARY = REPORT_ROOT / "integration-summary.json"
PUBLICATION_SUMMARY = REPORT_ROOT / "publication-summary.json"
EVIDENCE_ARTIFACT = ARTIFACT_ROOT / "long-horizon-operations-evidence.json"
PUBLICATION_ARTIFACT = ARTIFACT_ROOT / "support-window-publication.json"
PUBLIC_COMMAND_CONTRACT = ROOT / "tmp" / "artifacts" / "public-command-surface" / "objc3c-public-command-contract.json"
PACKAGER = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"

EXPECTED_PUBLIC_ACTIONS = [
    "validate-long-horizon-operations",
    "publish-long-horizon-operations",
]
EXPECTED_PUBLIC_SCRIPTS = [
    "test:objc3c:long-horizon-operations",
    "publish:objc3c:long-horizon-operations",
]
DEMOTED_OR_OUT_OF_SCOPE_CLAIMS = [
    "cross-major compatibility without generated migration replay",
    "hosted registry availability",
    "background auto-update behavior",
    "manual compatibility waivers without generated evidence",
]


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def run_step(name: str, command: list[str]) -> dict[str, object]:
    result = subprocess.run(command, cwd=ROOT, text=True, capture_output=True, check=False)
    return {
        "name": name,
        "command": command,
        "exit_code": result.returncode,
        "stdout": result.stdout,
        "stderr": result.stderr,
    }


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def summary_passes(payload: dict[str, Any]) -> bool:
    return payload.get("status") in {"PASS", "OK"} or payload.get("ok") is True


def action_names(public_command_contract: dict[str, Any]) -> set[str]:
    actions = public_command_contract.get("actions", [])
    if not isinstance(actions, list):
        return set()
    return {str(action.get("action")) for action in actions if isinstance(action, dict)}


def package_script_names(public_command_contract: dict[str, Any]) -> set[str]:
    package_scripts = public_command_contract.get("package_scripts", [])
    names: set[str] = set()
    if not isinstance(package_scripts, list):
        return names
    for script in package_scripts:
        if isinstance(script, str):
            names.add(script)
        elif isinstance(script, dict):
            if script.get("package_script") is not None:
                names.add(str(script["package_script"]))
            public_scripts = script.get("public_scripts", [])
            if isinstance(public_scripts, list):
                names.update(str(public_script) for public_script in public_scripts)
    return names


def main() -> int:
    steps: list[dict[str, object]] = []
    failures: list[str] = []
    for name, command in STEPS:
        step = run_step(name, command)
        steps.append(step)
        expect(step["exit_code"] == 0, f"{name} failed", failures)
        if step["exit_code"] != 0:
            break

    required_paths = (
        BOUNDARY_SUMMARY,
        DEPRECATION_SUMMARY,
        MIGRATION_SUMMARY,
        AGING_SUMMARY,
        ARTIFACT_CONTRACT_SUMMARY,
        EVIDENCE_SUMMARY,
        INTEGRATION_SUMMARY,
        PUBLICATION_SUMMARY,
        EVIDENCE_ARTIFACT,
        PUBLICATION_ARTIFACT,
        PUBLIC_COMMAND_CONTRACT,
    )
    for path in required_paths:
        expect(path.is_file(), f"missing expected artifact {repo_rel(path)}", failures)

    summaries = {
        "boundary": load_json(BOUNDARY_SUMMARY) if BOUNDARY_SUMMARY.is_file() else {},
        "deprecation": load_json(DEPRECATION_SUMMARY) if DEPRECATION_SUMMARY.is_file() else {},
        "migration": load_json(MIGRATION_SUMMARY) if MIGRATION_SUMMARY.is_file() else {},
        "aging": load_json(AGING_SUMMARY) if AGING_SUMMARY.is_file() else {},
        "artifact_contract": load_json(ARTIFACT_CONTRACT_SUMMARY) if ARTIFACT_CONTRACT_SUMMARY.is_file() else {},
        "evidence": load_json(EVIDENCE_SUMMARY) if EVIDENCE_SUMMARY.is_file() else {},
        "integration": load_json(INTEGRATION_SUMMARY) if INTEGRATION_SUMMARY.is_file() else {},
        "publication": load_json(PUBLICATION_SUMMARY) if PUBLICATION_SUMMARY.is_file() else {},
    }
    for name, payload in summaries.items():
        expect(summary_passes(payload), f"{name} summary did not report PASS", failures)

    evidence = load_json(EVIDENCE_ARTIFACT) if EVIDENCE_ARTIFACT.is_file() else {}
    publication = load_json(PUBLICATION_ARTIFACT) if PUBLICATION_ARTIFACT.is_file() else {}
    public_command_contract = load_json(PUBLIC_COMMAND_CONTRACT) if PUBLIC_COMMAND_CONTRACT.is_file() else {}
    claim_audit = evidence.get("claim_audit", {}) if isinstance(evidence.get("claim_audit"), dict) else {}
    operator_publication = publication.get("operator_publication", {}) if isinstance(publication.get("operator_publication"), dict) else {}

    expect(evidence.get("contract_id") == "objc3c.long_horizon_operations.evidence.v1", "evidence artifact contract drifted", failures)
    expect(publication.get("contract_id") == "objc3c.long_horizon_operations.evidence.v1", "publication artifact contract drifted", failures)
    expect(claim_audit.get("release_blockers") == [], "evidence claim audit reported release blockers", failures)
    expect(operator_publication.get("support_state") == claim_audit.get("support_state"), "operator publication support_state drifted", failures)
    expect(sorted(operator_publication.get("public_actions", [])) == sorted(EXPECTED_PUBLIC_ACTIONS), "operator publication public actions drifted", failures)
    expect(sorted(operator_publication.get("public_scripts", [])) == sorted(EXPECTED_PUBLIC_SCRIPTS), "operator publication public scripts drifted", failures)
    expect(evidence.get("rollback", {}).get("status") == "PASS", "rollback evidence did not pass", failures)
    expect(evidence.get("soak", {}).get("status") == "PASS", "soak evidence did not pass", failures)
    expect(evidence.get("aging_regression", {}).get("status") == "PASS", "aging evidence did not pass", failures)

    public_actions = action_names(public_command_contract)
    public_scripts = package_script_names(public_command_contract)
    for action in EXPECTED_PUBLIC_ACTIONS:
        expect(action in public_actions, f"public command contract missing action {action}", failures)
    for script in EXPECTED_PUBLIC_SCRIPTS:
        expect(script in public_scripts, f"public command contract missing package script {script}", failures)

    packager_text = PACKAGER.read_text(encoding="utf-8")
    for expected in ("long_horizon_operations_public_actions", "long_horizon_operations_public_scripts", "long_horizon_operations_surface"):
        expect(expected in packager_text, f"packager missing {expected}", failures)

    demoted_claims = [str(item) for item in claim_audit.get("demoted_or_out_of_scope_claims", [])]
    for claim in DEMOTED_OR_OUT_OF_SCOPE_CLAIMS:
        expect(claim in demoted_claims, f"claim audit missing demoted claim {claim}", failures)

    claim_audit_out = {
        "support_state": claim_audit.get("support_state"),
        "earned_claims": claim_audit.get("earned_claims", []),
        "demoted_or_out_of_scope_claims": demoted_claims,
        "release_blockers": failures,
    }
    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/check_m328_long_horizon_operations_closeout_gate.py",
        "step_count": len(steps),
        "steps": steps,
        "failures": failures,
        "expected_public_actions": EXPECTED_PUBLIC_ACTIONS,
        "expected_public_scripts": EXPECTED_PUBLIC_SCRIPTS,
        "claim_audit": claim_audit_out,
        "artifacts": {
            "evidence": repo_rel(EVIDENCE_ARTIFACT),
            "publication": repo_rel(PUBLICATION_ARTIFACT),
            "integration": repo_rel(INTEGRATION_SUMMARY),
            "publication_summary": repo_rel(PUBLICATION_SUMMARY),
            "public_command_contract": repo_rel(PUBLIC_COMMAND_CONTRACT),
        },
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Long-Horizon Operations Closeout Gate\n\n"
        f"- Contract: `{payload['contract_id']}`\n"
        f"- Step count: `{payload['step_count']}`\n"
        f"- Status: `{payload['status']}`\n"
        f"- Support state: `{claim_audit_out['support_state']}`\n"
        f"- Release blockers: `{len(failures)}`\n"
        f"- Public scripts: `{', '.join(EXPECTED_PUBLIC_SCRIPTS)}`\n",
        encoding="utf-8",
    )
    print(f"summary_path: {repo_rel(JSON_OUT)}")
    print("m328-long-horizon-operations-closeout-gate: PASS" if not failures else "m328-long-horizon-operations-closeout-gate: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
