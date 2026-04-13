#!/usr/bin/env python3
"""Run the application-architecture closeout gate over live public workflow evidence."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "tmp" / "reports" / "application-architecture-testing" / "closeout-gate"
JSON_OUT = OUT_DIR / "application_architecture_testing_closeout_gate.json"
MD_OUT = OUT_DIR / "application_architecture_testing_closeout_gate.md"
SUMMARY_CONTRACT_ID = "objc3c.application.architecture.testing.closeout.gate.v1"

STEPS = [
    ("build-boundary-inventory-summary", [sys.executable, "scripts/build_application_architecture_testing_boundary_inventory_summary.py"]),
    ("build-first-party-testing-semantics-summary", [sys.executable, "scripts/build_application_architecture_testing_semantic_summary.py"]),
    ("build-project-template-workspace-summary", [sys.executable, "scripts/build_application_architecture_template_workspace_summary.py"]),
    ("build-canonical-application-layering-summary", [sys.executable, "scripts/build_application_architecture_layering_summary.py"]),
    ("build-artifact-contract-summary", [sys.executable, "scripts/build_application_architecture_artifact_contract_summary.py"]),
    ("check-template-harness", [sys.executable, "scripts/check_application_architecture_template_harness.py"]),
    ("materialize-canonical-application-workspace", [sys.executable, "scripts/materialize_objc3c_canonical_application_workspace.py"]),
    ("check-application-architecture-integration", [sys.executable, "scripts/check_objc3c_application_architecture_integration.py"]),
    ("check-runnable-application-architecture-end-to-end", [sys.executable, "scripts/check_objc3c_runnable_application_architecture_end_to_end.py"]),
    ("render-public-command-surface", [sys.executable, "scripts/render_objc3c_public_command_surface.py"]),
    ("build-public-command-contract", [sys.executable, "scripts/build_objc3c_public_command_contract.py"]),
    ("check-public-command-budget", [sys.executable, "scripts/check_objc3c_public_command_budget.py"]),
    ("check-documentation-surface", [sys.executable, "scripts/check_documentation_surface.py"]),
    ("check-repo-superclean-surface", [sys.executable, "scripts/check_repo_superclean_surface.py"]),
    ("build-residue-authenticity-inventory", [sys.executable, "scripts/build_residue_authenticity_inventory.py"]),
    ("check-source-hygiene-authenticity", [sys.executable, "scripts/check_source_hygiene_authenticity.py"]),
]

BOUNDARY_SUMMARY = ROOT / "tmp" / "reports" / "application-architecture-testing" / "boundary-inventory-summary.json"
TESTING_SEMANTICS_SUMMARY = ROOT / "tmp" / "reports" / "application-architecture-testing" / "first-party-testing-semantics-summary.json"
TEMPLATE_WORKSPACE_SUMMARY = ROOT / "tmp" / "reports" / "application-architecture-testing" / "project-template-workspace-semantics-summary.json"
LAYERING_SUMMARY = ROOT / "tmp" / "reports" / "application-architecture-testing" / "canonical-application-layering-summary.json"
ARTIFACT_CONTRACT_SUMMARY = ROOT / "tmp" / "reports" / "application-architecture-testing" / "artifact-contract-summary.json"
TEMPLATE_HARNESS_SUMMARY = ROOT / "tmp" / "reports" / "application-architecture-testing" / "template-harness-summary.json"
CANONICAL_WORKSPACE_SUMMARY = ROOT / "tmp" / "reports" / "application-architecture-testing" / "canonical-application-workspace-summary.json"
INTEGRATION_SUMMARY = ROOT / "tmp" / "reports" / "application-architecture-testing" / "runnable-template-canonical-app-summary.json"
PACKAGE_INTEGRATION_SUMMARY = ROOT / "tmp" / "reports" / "application-architecture-testing" / "package-integration-summary.json"
PUBLIC_COMMAND_CONTRACT = ROOT / "tmp" / "artifacts" / "public-command-surface" / "objc3c-public-command-contract.json"

EXPECTED_WORKFLOW_ACTIONS = [
    "materialize-project-template",
    "materialize-canonical-application-workspace",
    "validate-showcase",
    "validate-stdlib-program",
    "validate-application-architecture",
]
EXPECTED_PACKAGE_ACTIONS = [
    "materialize-project-template",
    "materialize-canonical-application-workspace",
    "validate-application-architecture",
    "validate-runnable-application-architecture",
]
EXPECTED_PACKAGE_SCRIPTS = [
    "build:objc3c:template",
    "build:objc3c:application-workspace",
    "test:objc3c:application-architecture",
    "test:objc3c:application-architecture:e2e",
]
EXPECTED_CLAIM_RULES = [
    "all milestone evidence reports publish under one generated report root",
    "all machine-owned application-architecture artifacts must be replayable from checked-in source contracts and public workflow actions",
    "template, workspace, and canonical-app evidence may reuse showcase and stdlib sources but may not create a second checked-in example taxonomy",
    "package and runnable evidence is invalid if it bypasses the public workflow runner",
]
DEMOTED_OR_OUT_OF_SCOPE_CLAIMS = [
    "hosted package manager or registry semantics are out of scope",
    "non-runner application scaffolds are not accepted as closeout evidence",
    "examples that cannot pass the packaged runnable workflow are not supported",
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
        raise RuntimeError(f"{repo_rel(path)} did not contain a JSON object")
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
    if not isinstance(package_scripts, list):
        return set()
    names: set[str] = set()
    for script in package_scripts:
        if isinstance(script, str):
            names.add(script)
        elif isinstance(script, dict):
            if script.get("package_script") is not None:
                names.add(str(script["package_script"]))
            if script.get("name") is not None:
                names.add(str(script["name"]))
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
        TESTING_SEMANTICS_SUMMARY,
        TEMPLATE_WORKSPACE_SUMMARY,
        LAYERING_SUMMARY,
        ARTIFACT_CONTRACT_SUMMARY,
        TEMPLATE_HARNESS_SUMMARY,
        CANONICAL_WORKSPACE_SUMMARY,
        INTEGRATION_SUMMARY,
        PACKAGE_INTEGRATION_SUMMARY,
        PUBLIC_COMMAND_CONTRACT,
    )
    for path in required_paths:
        expect(path.is_file(), f"missing expected artifact {repo_rel(path)}", failures)

    boundary_summary = load_json(BOUNDARY_SUMMARY) if BOUNDARY_SUMMARY.is_file() else {}
    testing_semantics_summary = load_json(TESTING_SEMANTICS_SUMMARY) if TESTING_SEMANTICS_SUMMARY.is_file() else {}
    template_workspace_summary = load_json(TEMPLATE_WORKSPACE_SUMMARY) if TEMPLATE_WORKSPACE_SUMMARY.is_file() else {}
    layering_summary = load_json(LAYERING_SUMMARY) if LAYERING_SUMMARY.is_file() else {}
    artifact_contract_summary = load_json(ARTIFACT_CONTRACT_SUMMARY) if ARTIFACT_CONTRACT_SUMMARY.is_file() else {}
    template_harness_summary = load_json(TEMPLATE_HARNESS_SUMMARY) if TEMPLATE_HARNESS_SUMMARY.is_file() else {}
    canonical_workspace_summary = load_json(CANONICAL_WORKSPACE_SUMMARY) if CANONICAL_WORKSPACE_SUMMARY.is_file() else {}
    integration_summary = load_json(INTEGRATION_SUMMARY) if INTEGRATION_SUMMARY.is_file() else {}
    package_integration_summary = load_json(PACKAGE_INTEGRATION_SUMMARY) if PACKAGE_INTEGRATION_SUMMARY.is_file() else {}
    public_command_contract = load_json(PUBLIC_COMMAND_CONTRACT) if PUBLIC_COMMAND_CONTRACT.is_file() else {}

    for name, payload in (
        ("boundary inventory", boundary_summary),
        ("first-party testing semantics", testing_semantics_summary),
        ("project template workspace semantics", template_workspace_summary),
        ("canonical application layering", layering_summary),
        ("artifact contract", artifact_contract_summary),
        ("template harness", template_harness_summary),
        ("canonical workspace", canonical_workspace_summary),
        ("application architecture integration", integration_summary),
        ("package integration", package_integration_summary),
    ):
        expect(summary_passes(payload), f"{name} summary did not report PASS", failures)

    expect(boundary_summary.get("missing_paths") == [], "boundary inventory reported missing paths", failures)
    expect(testing_semantics_summary.get("missing_paths") == [], "testing semantics reported missing paths", failures)
    expect(testing_semantics_summary.get("missing_public_scripts") == [], "testing semantics reported missing public scripts", failures)
    expect(template_workspace_summary.get("missing_paths") == [], "template workspace semantics reported missing paths", failures)
    expect(template_workspace_summary.get("missing_public_scripts") == [], "template workspace semantics reported missing public scripts", failures)
    expect(template_workspace_summary.get("missing_actions") == [], "template workspace semantics reported missing actions", failures)
    expect(layering_summary.get("architecture_layer_count") == 4, "canonical application layer count drifted", failures)
    expect(layering_summary.get("aligned_example_count") == 3, "canonical application example alignment drifted", failures)
    expect(layering_summary.get("missing_examples") == [], "canonical application layering reported missing examples", failures)
    expect(layering_summary.get("missing_actions") == [], "canonical application layering reported missing actions", failures)
    expect(layering_summary.get("missing_package_script") == [], "canonical application layering reported missing package scripts", failures)
    expect(artifact_contract_summary.get("missing_paths") == [], "artifact contract reported missing paths", failures)
    expect(artifact_contract_summary.get("missing_public_scripts") == [], "artifact contract reported missing public scripts", failures)

    artifact_claim_rules = [str(rule) for rule in artifact_contract_summary.get("artifact_claim_rules", [])]
    expect(artifact_claim_rules == EXPECTED_CLAIM_RULES, "artifact claim rules drifted", failures)
    expect(canonical_workspace_summary.get("example_count") == 3, "canonical workspace example count drifted", failures)
    expect(canonical_workspace_summary.get("architecture_layer_count") == 4, "canonical workspace layer count drifted", failures)
    expect(integration_summary.get("workflow_actions") == EXPECTED_WORKFLOW_ACTIONS, "integration workflow actions drifted", failures)
    expect(integration_summary.get("failures") == [], "integration summary reported failures", failures)
    expect(
        package_integration_summary.get("application_architecture_public_actions") == EXPECTED_PACKAGE_ACTIONS,
        "package integration public actions drifted",
        failures,
    )
    expect(
        package_integration_summary.get("application_architecture_public_scripts") == EXPECTED_PACKAGE_SCRIPTS,
        "package integration public scripts drifted",
        failures,
    )
    expect(package_integration_summary.get("failures") == [], "package integration summary reported failures", failures)

    public_actions = action_names(public_command_contract)
    public_scripts = package_script_names(public_command_contract)
    for action in EXPECTED_PACKAGE_ACTIONS:
        expect(action in public_actions, f"public command contract missing action {action}", failures)
    for script in EXPECTED_PACKAGE_SCRIPTS:
        expect(script in public_scripts, f"public command contract missing package script {script}", failures)

    claim_audit = {
        "support_state": "supported-for-current-public-runner-and-runnable-package-surfaces",
        "earned_claims": [
            "first-party testing semantics are backed by checked-in fixture rules and public actions",
            "project templates materialize through the public workflow runner",
            "canonical application workspace preserves three showcase examples across four architecture layers",
            "package/install evidence runs the same template and canonical-app checks from a staged runnable bundle",
        ],
        "demoted_or_out_of_scope_claims": DEMOTED_OR_OUT_OF_SCOPE_CLAIMS,
        "release_blockers": failures,
    }

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/check_m330_application_architecture_testing_closeout_gate.py",
        "step_count": len(steps),
        "steps": steps,
        "failures": failures,
        "expected_workflow_actions": EXPECTED_WORKFLOW_ACTIONS,
        "expected_package_actions": EXPECTED_PACKAGE_ACTIONS,
        "expected_package_scripts": EXPECTED_PACKAGE_SCRIPTS,
        "claim_audit": claim_audit,
        "artifacts": {
            "boundary_inventory": repo_rel(BOUNDARY_SUMMARY),
            "first_party_testing_semantics": repo_rel(TESTING_SEMANTICS_SUMMARY),
            "project_template_workspace_semantics": repo_rel(TEMPLATE_WORKSPACE_SUMMARY),
            "canonical_application_layering": repo_rel(LAYERING_SUMMARY),
            "artifact_contract": repo_rel(ARTIFACT_CONTRACT_SUMMARY),
            "template_harness": repo_rel(TEMPLATE_HARNESS_SUMMARY),
            "canonical_workspace": repo_rel(CANONICAL_WORKSPACE_SUMMARY),
            "application_architecture_integration": repo_rel(INTEGRATION_SUMMARY),
            "package_integration": repo_rel(PACKAGE_INTEGRATION_SUMMARY),
            "public_command_contract": repo_rel(PUBLIC_COMMAND_CONTRACT),
        },
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Application Architecture And Testing Closeout Gate\n\n"
        f"- Contract: `{payload['contract_id']}`\n"
        f"- Step count: `{payload['step_count']}`\n"
        f"- Status: `{payload['status']}`\n"
        f"- Support state: `{claim_audit['support_state']}`\n"
        f"- Release blockers: `{len(failures)}`\n"
        f"- Package scripts: `{', '.join(EXPECTED_PACKAGE_SCRIPTS)}`\n",
        encoding="utf-8",
    )
    print(f"summary_path: {repo_rel(JSON_OUT)}")
    print("m330-application-architecture-testing-closeout-gate: PASS" if not failures else "m330-application-architecture-testing-closeout-gate: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
