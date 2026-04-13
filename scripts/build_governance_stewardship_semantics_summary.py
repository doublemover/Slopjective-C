#!/usr/bin/env python3
"""Build the governance stewardship semantics summary."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability"
SEMANTICS_PATH = FIXTURE_ROOT / "stewardship_semantics.json"
MAINTAINER_REVIEW_PATH = FIXTURE_ROOT / "maintainer_review_regression_contract.json"
RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_governance_sustainability.md"
MAINTAINER_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_maintainer_workflows.md"
CONTRIBUTING_PATH = ROOT / "CONTRIBUTING.md"
OUT_DIR = ROOT / "tmp" / "reports" / "governance-sustainability" / "stewardship-semantics"
SUMMARY_PATH = OUT_DIR / "governance_stewardship_semantics_summary.json"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    semantics = read_json(SEMANTICS_PATH)
    maintainer_review = read_json(MAINTAINER_REVIEW_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    maintainer_runbook_text = MAINTAINER_RUNBOOK_PATH.read_text(encoding="utf-8")
    contributing_text = CONTRIBUTING_PATH.read_text(encoding="utf-8")
    failures: list[str] = []

    for raw_path in (semantics.get("runbook"), semantics.get("maintainer_runbook"), semantics.get("contributor_surface")):
        expect(isinstance(raw_path, str) and (ROOT / raw_path).is_file(), f"missing stewardship path {raw_path}", failures)
    for raw_path in semantics.get("depends_on", []):
        expect((ROOT / str(raw_path)).is_file(), f"missing dependency {raw_path}", failures)

    roles = semantics.get("role_surfaces", [])
    expect(isinstance(roles, list) and len(roles) >= 3, "role surface set is too narrow", failures)
    role_names = {str(role.get("role")) for role in roles if isinstance(role, dict)}
    expect({"contributor", "maintainer", "package-steward"}.issubset(role_names), "required stewardship roles missing", failures)
    for role in roles if isinstance(roles, list) else []:
        if not isinstance(role, dict):
            failures.append("role surface entry must be an object")
            continue
        role_name = str(role.get("role"))
        entry_surface = str(role.get("entry_surface"))
        expect((ROOT / entry_surface).is_file(), f"{role_name} entry surface missing", failures)
        expect(len(role.get("responsibilities", [])) >= 4, f"{role_name} responsibility set is too narrow", failures)

    maintainer_check_ids = {
        str(check.get("check_id"))
        for check in maintainer_review.get("required_review_checks", [])
        if isinstance(check, dict)
    }
    required_review_checks = [str(check) for check in semantics.get("required_review_checks", [])]
    missing_maintainer_checks = sorted(
        check for check in required_review_checks if check != "extension-review-policy" and check not in maintainer_check_ids
    )
    expect(not missing_maintainer_checks, f"missing maintainer review checks: {missing_maintainer_checks}", failures)
    expect("extension-review-policy" in required_review_checks, "extension review policy check missing", failures)
    expect(len(semantics.get("package_governance_rules", [])) >= 4, "package governance rules are too narrow", failures)
    expect(any("budget" in rule for rule in semantics.get("package_governance_rules", [])), "package governance must mention budget impact", failures)
    expect(any("hosted registry" in rule for rule in semantics.get("package_governance_rules", [])), "hosted registry demotion rule missing", failures)

    required_mentions = {
        "runbook_mentions_semantics": "tests/tooling/fixtures/governance_sustainability/stewardship_semantics.json" in runbook_text,
        "runbook_mentions_summary": "python scripts/build_governance_stewardship_semantics_summary.py" in runbook_text,
        "maintainer_runbook_mentions_semantics": "tests/tooling/fixtures/governance_sustainability/stewardship_semantics.json" in maintainer_runbook_text,
        "maintainer_runbook_mentions_summary": "python scripts/build_governance_stewardship_semantics_summary.py" in maintainer_runbook_text,
        "contributing_mentions_summary": "python scripts/build_governance_stewardship_semantics_summary.py" in contributing_text,
    }
    for key, value in required_mentions.items():
        expect(value, f"{key} is false", failures)

    summary = {
        "contract_id": "objc3c.governance.stewardship_semantics.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "semantics": repo_rel(SEMANTICS_PATH),
        "maintainer_review_contract": repo_rel(MAINTAINER_REVIEW_PATH),
        "role_count": len(roles) if isinstance(roles, list) else 0,
        "roles": sorted(role_names),
        "required_review_check_count": len(required_review_checks),
        "missing_maintainer_checks": missing_maintainer_checks,
        "package_governance_rule_count": len(semantics.get("package_governance_rules", [])),
        "fail_closed_condition_count": len(semantics.get("fail_closed_conditions", [])),
        **required_mentions,
        "failures": failures,
    }
    summary["ok"] = not failures

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
