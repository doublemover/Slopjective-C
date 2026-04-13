#!/usr/bin/env python3
"""Run the package ecosystem closeout gate over live package workflow evidence."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "tmp" / "reports" / "package-ecosystem" / "closeout-gate"
JSON_OUT = OUT_DIR / "package_ecosystem_closeout_gate.json"
MD_OUT = OUT_DIR / "package_ecosystem_closeout_gate.md"
SUMMARY_CONTRACT_ID = "objc3c.package_ecosystem.closeout.gate.v1"

STEPS = [
    ("build-boundary-inventory-summary", [sys.executable, "scripts/build_package_ecosystem_boundary_inventory_summary.py"]),
    ("build-dependency-lock-policy-summary", [sys.executable, "scripts/build_package_ecosystem_dependency_lock_policy_summary.py"]),
    ("build-local-workspace-mirror-summary", [sys.executable, "scripts/build_package_ecosystem_local_workspace_mirror_summary.py"]),
    ("build-registry-publication-summary", [sys.executable, "scripts/build_package_ecosystem_registry_publication_summary.py"]),
    ("build-artifact-contract-summary", [sys.executable, "scripts/build_package_ecosystem_artifact_contract_summary.py"]),
    ("check-package-authoring-workflow", [sys.executable, "scripts/check_objc3c_package_authoring_workflow.py"]),
    ("check-registry-mirror-reproducibility", [sys.executable, "scripts/check_objc3c_package_registry_mirror_reproducibility.py"]),
    ("check-package-ecosystem-integration", [sys.executable, "scripts/check_objc3c_package_ecosystem_integration.py"]),
    ("check-runnable-package-ecosystem-end-to-end", [sys.executable, "scripts/check_objc3c_runnable_package_ecosystem_end_to_end.py"]),
    ("render-public-command-surface", [sys.executable, "scripts/render_objc3c_public_command_surface.py"]),
    ("build-public-command-contract", [sys.executable, "scripts/build_objc3c_public_command_contract.py"]),
    ("check-public-command-budget", [sys.executable, "scripts/check_objc3c_public_command_budget.py"]),
    ("check-documentation-surface", [sys.executable, "scripts/check_documentation_surface.py"]),
    ("check-repo-superclean-surface", [sys.executable, "scripts/check_repo_superclean_surface.py"]),
    ("build-residue-authenticity-inventory", [sys.executable, "scripts/build_residue_authenticity_inventory.py"]),
    ("check-source-hygiene-authenticity", [sys.executable, "scripts/check_source_hygiene_authenticity.py"]),
]

REPORT_ROOT = ROOT / "tmp" / "reports" / "package-ecosystem"
ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "package-ecosystem"
BOUNDARY_SUMMARY = REPORT_ROOT / "boundary-inventory-summary.json"
LOCK_POLICY_SUMMARY = REPORT_ROOT / "dependency-lock-policy-summary.json"
WORKSPACE_MIRROR_SUMMARY = REPORT_ROOT / "local-workspace-mirror-semantics-summary.json"
REGISTRY_PUBLICATION_SUMMARY = REPORT_ROOT / "registry-publication-semantics-summary.json"
ARTIFACT_CONTRACT_SUMMARY = REPORT_ROOT / "artifact-contract-summary.json"
LOCK_SUMMARY = REPORT_ROOT / "package-lock-summary.json"
AUTHORING_SUMMARY = REPORT_ROOT / "package-authoring-workflow-summary.json"
MIRROR_SUMMARY = REPORT_ROOT / "package-mirror-summary.json"
REGISTRY_MIRROR_SUMMARY = REPORT_ROOT / "registry-mirror-reproducibility-summary.json"
INTEGRATION_SUMMARY = REPORT_ROOT / "integration-summary.json"
RUNNABLE_SUMMARY = REPORT_ROOT / "runnable-package-ecosystem-summary.json"
LOCK_ARTIFACT = ARTIFACT_ROOT / "locks" / "objc3c-package-lock.json"
MIRROR_ARTIFACT = ARTIFACT_ROOT / "mirrors" / "offline-mirror-index.json"
REGISTRY_ARTIFACT = ARTIFACT_ROOT / "registry" / "local-package-index.json"
PUBLICATION_ARTIFACT = ARTIFACT_ROOT / "registry" / "publication-metadata.json"
PUBLIC_COMMAND_CONTRACT = ROOT / "tmp" / "artifacts" / "public-command-surface" / "objc3c-public-command-contract.json"

EXPECTED_PACKAGE_ACTIONS = [
    "build-package-lock",
    "validate-package-authoring",
    "validate-package-mirror",
    "validate-package-ecosystem",
    "validate-runnable-package-ecosystem",
]
EXPECTED_PACKAGE_SCRIPTS = [
    "build:objc3c:package-lock",
    "test:objc3c:package-authoring",
    "test:objc3c:package-mirror",
    "test:objc3c:package-ecosystem",
    "test:objc3c:package-ecosystem:e2e",
]
DEMOTED_OR_OUT_OF_SCOPE_CLAIMS = [
    "hosted package registry service",
    "network-backed dependency resolution",
    "system package manager publication",
    "manual lockfile or registry sidecar source authority",
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
        LOCK_POLICY_SUMMARY,
        WORKSPACE_MIRROR_SUMMARY,
        REGISTRY_PUBLICATION_SUMMARY,
        ARTIFACT_CONTRACT_SUMMARY,
        LOCK_SUMMARY,
        AUTHORING_SUMMARY,
        MIRROR_SUMMARY,
        REGISTRY_MIRROR_SUMMARY,
        INTEGRATION_SUMMARY,
        RUNNABLE_SUMMARY,
        LOCK_ARTIFACT,
        MIRROR_ARTIFACT,
        REGISTRY_ARTIFACT,
        PUBLICATION_ARTIFACT,
        PUBLIC_COMMAND_CONTRACT,
    )
    for path in required_paths:
        expect(path.is_file(), f"missing expected artifact {repo_rel(path)}", failures)

    summaries = {
        "boundary": load_json(BOUNDARY_SUMMARY) if BOUNDARY_SUMMARY.is_file() else {},
        "lock_policy": load_json(LOCK_POLICY_SUMMARY) if LOCK_POLICY_SUMMARY.is_file() else {},
        "workspace_mirror": load_json(WORKSPACE_MIRROR_SUMMARY) if WORKSPACE_MIRROR_SUMMARY.is_file() else {},
        "registry_publication": load_json(REGISTRY_PUBLICATION_SUMMARY) if REGISTRY_PUBLICATION_SUMMARY.is_file() else {},
        "artifact_contract": load_json(ARTIFACT_CONTRACT_SUMMARY) if ARTIFACT_CONTRACT_SUMMARY.is_file() else {},
        "lock": load_json(LOCK_SUMMARY) if LOCK_SUMMARY.is_file() else {},
        "authoring": load_json(AUTHORING_SUMMARY) if AUTHORING_SUMMARY.is_file() else {},
        "mirror": load_json(MIRROR_SUMMARY) if MIRROR_SUMMARY.is_file() else {},
        "registry_mirror": load_json(REGISTRY_MIRROR_SUMMARY) if REGISTRY_MIRROR_SUMMARY.is_file() else {},
        "integration": load_json(INTEGRATION_SUMMARY) if INTEGRATION_SUMMARY.is_file() else {},
        "runnable": load_json(RUNNABLE_SUMMARY) if RUNNABLE_SUMMARY.is_file() else {},
    }
    for name, payload in summaries.items():
        expect(summary_passes(payload), f"{name} summary did not report PASS", failures)

    expect(summaries["boundary"].get("missing_paths") == [], "boundary inventory reported missing paths", failures)
    expect(summaries["boundary"].get("missing_public_scripts") == [], "boundary inventory reported missing public scripts", failures)
    expect(summaries["artifact_contract"].get("missing_paths") == [], "artifact contract reported missing paths", failures)
    expect(summaries["artifact_contract"].get("missing_public_scripts") == [], "artifact contract reported missing public scripts", failures)
    expect(summaries["lock"].get("package_count", 0) >= 8, "lock summary package count is too narrow", failures)
    expect(summaries["lock"].get("dependency_count", 0) >= 7, "lock summary dependency count is too narrow", failures)
    expect(summaries["registry_mirror"].get("network_policy") == "no-network-during-validation", "mirror network policy drifted", failures)
    expect(
        summaries["registry_mirror"].get("hosted_registry_support") == "deferred-release-blocking-if-claimed",
        "hosted registry support claim drifted",
        failures,
    )
    expect(summaries["runnable"].get("package_ecosystem_public_actions") == EXPECTED_PACKAGE_ACTIONS, "runnable package public actions drifted", failures)
    expect(summaries["runnable"].get("package_ecosystem_public_scripts") == EXPECTED_PACKAGE_SCRIPTS, "runnable package public scripts drifted", failures)

    public_command_contract = load_json(PUBLIC_COMMAND_CONTRACT) if PUBLIC_COMMAND_CONTRACT.is_file() else {}
    public_actions = action_names(public_command_contract)
    public_scripts = package_script_names(public_command_contract)
    for action in EXPECTED_PACKAGE_ACTIONS:
        expect(action in public_actions, f"public command contract missing action {action}", failures)
    for script in EXPECTED_PACKAGE_SCRIPTS:
        expect(script in public_scripts, f"public command contract missing script {script}", failures)

    claim_audit = {
        "support_state": "supported-for-local-lock-authoring-offline-mirror-and-generated-publication-metadata",
        "earned_claims": [
            "local package authoring emits deterministic provenance-bearing locks from checked-in package surfaces",
            "offline mirror and local registry metadata are generated from the lock with no network validation",
            "canonical application and stdlib program workflows pressure the package model through public actions",
            "staged runnable bundles can validate package authoring and offline mirror behavior from the package root",
        ],
        "demoted_or_out_of_scope_claims": DEMOTED_OR_OUT_OF_SCOPE_CLAIMS,
        "release_blockers": failures,
    }

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS" if not failures else "FAIL",
        "runner_path": "scripts/check_m329_package_ecosystem_closeout_gate.py",
        "step_count": len(steps),
        "steps": steps,
        "failures": failures,
        "expected_package_actions": EXPECTED_PACKAGE_ACTIONS,
        "expected_package_scripts": EXPECTED_PACKAGE_SCRIPTS,
        "claim_audit": claim_audit,
        "artifacts": {
            name: repo_rel(path)
            for name, path in {
                "boundary": BOUNDARY_SUMMARY,
                "lock_policy": LOCK_POLICY_SUMMARY,
                "workspace_mirror": WORKSPACE_MIRROR_SUMMARY,
                "registry_publication": REGISTRY_PUBLICATION_SUMMARY,
                "artifact_contract": ARTIFACT_CONTRACT_SUMMARY,
                "lock": LOCK_ARTIFACT,
                "mirror": MIRROR_ARTIFACT,
                "registry": REGISTRY_ARTIFACT,
                "publication_metadata": PUBLICATION_ARTIFACT,
                "integration": INTEGRATION_SUMMARY,
                "runnable": RUNNABLE_SUMMARY,
                "public_command_contract": PUBLIC_COMMAND_CONTRACT,
            }.items()
        },
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Package Ecosystem Closeout Gate\n\n"
        f"- Contract: `{payload['contract_id']}`\n"
        f"- Step count: `{payload['step_count']}`\n"
        f"- Status: `{payload['status']}`\n"
        f"- Support state: `{claim_audit['support_state']}`\n"
        f"- Release blockers: `{len(failures)}`\n"
        f"- Public scripts: `{', '.join(EXPECTED_PACKAGE_SCRIPTS)}`\n",
        encoding="utf-8",
    )
    print(f"summary_path: {repo_rel(JSON_OUT)}")
    print("m329-package-ecosystem-closeout-gate: PASS" if not failures else "m329-package-ecosystem-closeout-gate: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
