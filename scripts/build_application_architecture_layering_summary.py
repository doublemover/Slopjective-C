#!/usr/bin/env python3
"""Build the canonical application layering semantics summary."""

from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "application_architecture_testing" / "canonical_application_architecture_semantics.json"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "application-architecture-testing" / "canonical-application-layering-summary.json"




def main() -> int:
    contract = load_json(CONTRACT_PATH)
    package = load_json(PACKAGE_JSON)
    portfolio = load_json(ROOT / str(contract["portfolio_manifest"]))
    stdlib_program_surface = load_json(ROOT / str(contract["stdlib_program_surface"]))
    package_scripts = package.get("scripts", {})
    if not isinstance(package_scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")
    package_bridge = "objc3c"
    package_bridge_exists = package_bridge in package_scripts
    registered_actions = set(public_workflow_action_names())

    examples = portfolio.get("examples", [])
    example_ids = {
        str(entry.get("id"))
        for entry in examples
        if isinstance(entry, dict) and isinstance(entry.get("id"), str)
    }
    aligned_examples = [entry["example_id"] for entry in contract["capability_portfolio_alignment"]]
    missing_examples = [example_id for example_id in aligned_examples if example_id not in example_ids]
    missing_actions = [
        action
        for action in contract["required_evidence_actions"]
        if action not in str(stdlib_program_surface.get("workflow_surface", {}))
        and action not in str(stdlib_program_surface.get("public_actions", []))
        and action != "package-runnable-toolchain"
    ]
    missing_package_bridge = [] if package_bridge_exists else [package_bridge]
    if "package-runnable-toolchain" not in registered_actions:
        missing_actions.append("package-runnable-toolchain")

    payload = {
        "contract_id": "objc3c.application.architecture.testing.canonical_application_architecture_semantics.summary.v1",
        "status": "PASS" if not missing_examples and not missing_actions and not missing_package_bridge else "FAIL",
        "architecture_contract": repo_rel(CONTRACT_PATH),
        "runbook": str(contract["runbook"]),
        "architecture_layer_count": len(contract["architecture_layers"]),
        "aligned_example_count": len(aligned_examples),
        "required_evidence_action_count": len(contract["required_evidence_actions"]),
        "canonical_application_kind": contract["canonical_application_kind"],
        "architecture_layers": contract["architecture_layers"],
        "capability_portfolio_alignment": contract["capability_portfolio_alignment"],
        "canonical_claim_rules": contract["canonical_claim_rules"],
        "missing_examples": missing_examples,
        "missing_actions": missing_actions,
        "package_bridge": package_bridge,
        "package_bridge_count": 1 if package_bridge_exists else 0,
        "missing_package_bridge": missing_package_bridge,
        "non_goals": contract["non_goals"],
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("application-architecture-layering: PASS" if payload["status"] == "PASS" else "application-architecture-layering: FAIL")
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
