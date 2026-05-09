#!/usr/bin/env python3
"""Build the aging-regression and release-cadence criteria summary."""

from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.public_runner import public_workflow_action_names


ROOT = Path(__file__).resolve().parents[1]
CRITERIA_PATH = ROOT / "tests" / "tooling" / "fixtures" / "long_horizon_operations" / "aging_regression_release_cadence_criteria.json"
PERFORMANCE_BUDGET = ROOT / "tests" / "tooling" / "fixtures" / "performance_governance" / "budget_model.json"
SOAK_CONTRACT = ROOT / "tests" / "tooling" / "fixtures" / "full_envelope_claimability" / "soak_external_validation_contract.json"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "long-horizon-operations" / "aging-regression-release-cadence-summary.json"




def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    criteria = load_json(CRITERIA_PATH)
    performance_budget = load_json(PERFORMANCE_BUDGET)
    soak_contract = load_json(SOAK_CONTRACT)
    package = load_json(PACKAGE_JSON)
    scripts = package.get("scripts", {})
    if not isinstance(scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")
    package_bridge = str(criteria["package_bridge"])
    package_bridge_exists = package_bridge in scripts
    registered_actions = set(public_workflow_action_names())

    failures: list[str] = []
    for raw_path in criteria.get("depends_on", []):
        expect((ROOT / str(raw_path)).is_file(), f"missing dependency {raw_path}", failures)

    required_actions = [str(name) for name in criteria.get("required_actions", [])]
    missing_actions = [name for name in required_actions if name not in registered_actions]
    expect(package_bridge_exists, f"missing package bridge: {package_bridge}", failures)
    expect(not missing_actions, f"missing workflow actions: {missing_actions}", failures)

    budget_families = performance_budget.get("budget_families", [])
    publication_freshness = [
        family for family in budget_families
        if isinstance(family, dict) and family.get("budget_id") == "publication-freshness"
    ]
    expect(bool(publication_freshness), "performance governance missing publication-freshness budget", failures)
    freshness_metrics = publication_freshness[0].get("metric_definitions", []) if publication_freshness else []
    blocking_freshness_metrics = [
        metric for metric in freshness_metrics
        if isinstance(metric, dict) and metric.get("comparison") == "max" and metric.get("blocking_value") is not None
    ]
    expect(len(blocking_freshness_metrics) >= 3, "publication freshness blocking metrics are too narrow", failures)

    soak_families = soak_contract.get("required_acceptance_matrix_families", [])
    expect(isinstance(soak_families, list) and len(soak_families) >= 4, "soak acceptance family coverage is too narrow", failures)

    cadence_classes = criteria.get("cadence_classes", [])
    expect(isinstance(cadence_classes, list) and len(cadence_classes) >= 3, "cadence class coverage is too narrow", failures)
    for cadence in cadence_classes if isinstance(cadence_classes, list) else []:
        if not isinstance(cadence, dict):
            failures.append("cadence class entry must be an object")
            continue
        minimum_families = cadence.get("minimum_evidence_families", [])
        expect(
            isinstance(minimum_families, list) and "release-operations" in minimum_families,
            f"{cadence.get('cadence_id')} missing release operations evidence",
            failures,
        )
        expect(cadence.get("revert_required") is True, f"{cadence.get('cadence_id')} must require revert evidence", failures)

    payload = {
        "contract_id": "objc3c.long_horizon_operations.aging_regression_release_cadence.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "criteria": repo_rel(CRITERIA_PATH),
        "performance_budget": repo_rel(PERFORMANCE_BUDGET),
        "soak_contract": repo_rel(SOAK_CONTRACT),
        "cadence_class_count": len(cadence_classes) if isinstance(cadence_classes, list) else 0,
        "publication_freshness_metric_count": len(blocking_freshness_metrics),
        "soak_acceptance_family_count": len(soak_families) if isinstance(soak_families, list) else 0,
        "required_action_count": len(required_actions),
        "package_bridge_count": 1 if package_bridge_exists else 0,
        "package_bridge": package_bridge,
        "missing_package_bridge": [] if package_bridge_exists else [package_bridge],
        "missing_actions": missing_actions,
        "aging_regression_rules": criteria.get("aging_regression_rules", []),
        "release_blocking_conditions": criteria.get("release_blocking_conditions", []),
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("long-horizon-aging-cadence: PASS" if not failures else "long-horizon-aging-cadence: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
