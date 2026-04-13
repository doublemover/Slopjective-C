#!/usr/bin/env python3
"""Run the adoption and evaluator-legibility closeout gate over live evidence."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "tmp" / "reports" / "adoption-legibility" / "closeout-gate"
JSON_OUT = OUT_DIR / "adoption_legibility_closeout_gate.json"
MD_OUT = OUT_DIR / "adoption_legibility_closeout_gate.md"
SUMMARY_CONTRACT_ID = "objc3c.adoption_legibility.closeout.gate.v1"
MILESTONE_NUMBER = 412
REPO = "doublemover/Slopjective-C"

STEPS = [
    ("build-boundary-inventory-summary", [sys.executable, "scripts/build_adoption_legibility_boundary_inventory_summary.py"]),
    ("build-public-claim-policy-summary", [sys.executable, "scripts/build_adoption_legibility_public_claim_policy_summary.py"]),
    ("build-capability-comparison-summary", [sys.executable, "scripts/build_adoption_legibility_capability_comparison_summary.py"]),
    ("build-migration-playbook-summary", [sys.executable, "scripts/build_adoption_legibility_migration_playbook_summary.py"]),
    ("build-artifact-contract-summary", [sys.executable, "scripts/build_adoption_legibility_artifact_contract_summary.py"]),
    ("check-adoption-legibility-integration", [sys.executable, "scripts/check_objc3c_adoption_legibility_integration.py"]),
    ("publish-adoption-legibility-metadata", [sys.executable, "scripts/publish_objc3c_adoption_legibility_metadata.py"]),
    ("render-public-command-surface", [sys.executable, "scripts/render_objc3c_public_command_surface.py"]),
    ("build-public-command-contract", [sys.executable, "scripts/build_objc3c_public_command_contract.py"]),
    ("check-public-command-budget", [sys.executable, "scripts/check_objc3c_public_command_budget.py"]),
    ("check-documentation-surface", [sys.executable, "scripts/check_documentation_surface.py"]),
    ("check-repo-superclean-surface", [sys.executable, "scripts/check_repo_superclean_surface.py"]),
    ("build-residue-authenticity-inventory", [sys.executable, "scripts/build_residue_authenticity_inventory.py"]),
    ("check-source-hygiene-authenticity", [sys.executable, "scripts/check_source_hygiene_authenticity.py"]),
    ("parse-runnable-toolchain-packager", ["pwsh", "-NoProfile", "-Command", "$null = [scriptblock]::Create((Get-Content -Raw scripts/package_objc3c_runnable_toolchain.ps1)); 'package-script-parse: OK'"]),
    ("fetch-live-milestone-state", ["gh", "api", f"repos/{REPO}/milestones/{MILESTONE_NUMBER}", "--jq", "{number,title,state,open_issues,closed_issues}"]),
]

REPORT_ROOT = ROOT / "tmp" / "reports" / "adoption-legibility"
ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "adoption-legibility"
BOUNDARY_SUMMARY = REPORT_ROOT / "boundary-inventory-summary.json"
PUBLIC_CLAIM_SUMMARY = REPORT_ROOT / "public-claim-policy-summary.json"
COMPARISON_SUMMARY = REPORT_ROOT / "capability-comparison-summary.json"
MIGRATION_SUMMARY = REPORT_ROOT / "migration-playbook-summary.json"
ARTIFACT_CONTRACT_SUMMARY = REPORT_ROOT / "artifact-contract-summary.json"
EVIDENCE_SUMMARY = REPORT_ROOT / "evidence-summary.json"
INTEGRATION_SUMMARY = REPORT_ROOT / "integration-summary.json"
PUBLICATION_SUMMARY = REPORT_ROOT / "publication-summary.json"
EVIDENCE_ARTIFACT = ARTIFACT_ROOT / "adoption-legibility-evidence.json"
PUBLICATION_ARTIFACT = ARTIFACT_ROOT / "evaluator-publication.json"
PUBLIC_COMMAND_CONTRACT = ROOT / "tmp" / "artifacts" / "public-command-surface" / "objc3c-public-command-contract.json"
PACKAGER = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"

EXPECTED_PUBLIC_ACTIONS = [
    "validate-adoption-legibility",
    "publish-adoption-legibility",
]
EXPECTED_PUBLIC_SCRIPTS = [
    "test:objc3c:adoption-legibility",
    "publish:objc3c:adoption-legibility",
]
DEMOTED_OR_OUT_OF_SCOPE_CLAIMS = [
    "drop-in Objective-C 2 replacement",
    "zero-risk migration",
    "performance leadership",
    "hosted registry or IDE marketplace parity",
    "private maintainer context as an evaluator prerequisite",
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


def parse_milestone_state(step: dict[str, object]) -> dict[str, Any]:
    if step.get("exit_code") != 0:
        return {}
    stdout = str(step.get("stdout") or "").strip()
    if not stdout:
        return {}
    return json.loads(stdout)


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
        PUBLIC_CLAIM_SUMMARY,
        COMPARISON_SUMMARY,
        MIGRATION_SUMMARY,
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
        "public_claim": load_json(PUBLIC_CLAIM_SUMMARY) if PUBLIC_CLAIM_SUMMARY.is_file() else {},
        "comparison": load_json(COMPARISON_SUMMARY) if COMPARISON_SUMMARY.is_file() else {},
        "migration": load_json(MIGRATION_SUMMARY) if MIGRATION_SUMMARY.is_file() else {},
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
    evaluator_publication = publication.get("evaluator_publication", {}) if isinstance(publication.get("evaluator_publication"), dict) else {}

    expect(evidence.get("contract_id") == "objc3c.adoption_legibility.evidence.v1", "evidence artifact contract drifted", failures)
    expect(publication.get("contract_id") == "objc3c.adoption_legibility.evidence.v1", "publication artifact contract drifted", failures)
    expect(claim_audit.get("release_blockers") == [], "evidence claim audit reported release blockers", failures)
    expect(evaluator_publication.get("support_state") == claim_audit.get("support_state"), "evaluator publication support_state drifted", failures)
    expect(sorted(evaluator_publication.get("public_actions", [])) == sorted(EXPECTED_PUBLIC_ACTIONS), "evaluator publication public actions drifted", failures)
    expect(sorted(evaluator_publication.get("public_scripts", [])) == sorted(EXPECTED_PUBLIC_SCRIPTS), "evaluator publication public scripts drifted", failures)
    expect(evidence.get("evaluator_path", {}).get("status") == "PASS", "evaluator path evidence did not pass", failures)
    expect(evidence.get("migration_playbook", {}).get("status") == "PASS", "migration playbook evidence did not pass", failures)
    expect(evidence.get("comparison_matrix", {}).get("status") == "PASS", "comparison matrix evidence did not pass", failures)
    expect(evidence.get("onboarding", {}).get("status") == "PASS", "onboarding evidence did not pass", failures)

    public_actions = action_names(public_command_contract)
    public_scripts = package_script_names(public_command_contract)
    for action in EXPECTED_PUBLIC_ACTIONS:
        expect(action in public_actions, f"public command contract missing action {action}", failures)
    for script in EXPECTED_PUBLIC_SCRIPTS:
        expect(script in public_scripts, f"public command contract missing package script {script}", failures)

    packager_text = PACKAGER.read_text(encoding="utf-8")
    for expected in ("adoption_legibility_public_actions", "adoption_legibility_public_scripts", "adoption_legibility_surface"):
        expect(expected in packager_text, f"packager missing {expected}", failures)

    demoted_claims = [str(item) for item in claim_audit.get("demoted_or_out_of_scope_claims", [])]
    for claim in DEMOTED_OR_OUT_OF_SCOPE_CLAIMS:
        expect(claim in demoted_claims, f"claim audit missing demoted claim {claim}", failures)

    milestone_step = next((step for step in steps if step.get("name") == "fetch-live-milestone-state"), {})
    milestone_state = parse_milestone_state(milestone_step) if milestone_step else {}
    expect(milestone_state.get("number") == MILESTONE_NUMBER, "live milestone number drifted", failures)
    expect(milestone_state.get("state") == "open", "live milestone should remain open until closeout issue is closed", failures)
    expect(milestone_state.get("open_issues") == 1, "live milestone should have exactly one open issue before closeout closure", failures)

    claim_audit_out = {
        "support_state": claim_audit.get("support_state"),
        "earned_claims": claim_audit.get("earned_claims", []),
        "demoted_or_out_of_scope_claims": demoted_claims,
        "release_blockers": failures,
    }
    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/check_m331_adoption_legibility_closeout_gate.py",
        "step_count": len(steps),
        "steps": steps,
        "failures": failures,
        "expected_public_actions": EXPECTED_PUBLIC_ACTIONS,
        "expected_public_scripts": EXPECTED_PUBLIC_SCRIPTS,
        "live_milestone_state": milestone_state,
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
        "# Adoption And Legibility Closeout Gate\n\n"
        f"- Contract: `{payload['contract_id']}`\n"
        f"- Step count: `{payload['step_count']}`\n"
        f"- Status: `{payload['status']}`\n"
        f"- Support state: `{claim_audit_out['support_state']}`\n"
        f"- Live milestone open issues before closure: `{milestone_state.get('open_issues')}`\n"
        f"- Release blockers: `{len(failures)}`\n"
        f"- Public scripts: `{', '.join(EXPECTED_PUBLIC_SCRIPTS)}`\n",
        encoding="utf-8",
    )
    print(f"summary_path: {repo_rel(JSON_OUT)}")
    print("m331-adoption-legibility-closeout-gate: PASS" if not failures else "m331-adoption-legibility-closeout-gate: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
