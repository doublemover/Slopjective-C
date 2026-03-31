#!/usr/bin/env python3
"""Enforce the live governance budget and waiver contract."""

from __future__ import annotations

import json
import re
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability"
BUDGET_INVENTORY_PATH = FIXTURE_ROOT / "budget_inventory.json"
POLICY_PATH = FIXTURE_ROOT / "sustainable_progress_policy.json"
WAIVER_REGISTRY_PATH = FIXTURE_ROOT / "waiver_registry.json"
SCHEMA_PATH = ROOT / "schemas" / "objc3c-governance-budget-summary-v1.schema.json"
TASK_HYGIENE_PATH = ROOT / "scripts" / "ci" / "check_task_hygiene.py"
RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_governance_sustainability.md"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "governance-sustainability" / "budget-enforcement" / "governance_budget_enforcement_summary.json"

LEGACY_ALIAS_RE = re.compile(
    r"npm run (check:objc3c:m|test:tooling:m|check:compiler-closeout:m|run:objc3c:|plan:compiler-dispatch:|refresh:compiler-dispatch:|dev:objc3c:)"
)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {path}")
    return payload


def count_package_script_categories(package_json: dict[str, Any]) -> dict[str, int]:
    categories: dict[str, int] = {}
    for name in package_json.get("scripts", {}):
        key = name.split(":", 1)[0] if ":" in name else name
        categories[key] = categories.get(key, 0) + 1
    return dict(sorted(categories.items()))


def count_action_definitions(path: Path) -> int:
    return sum(1 for line in path.read_text(encoding="utf-8").splitlines() if line.startswith("def action_"))


def iter_live_files(roots: list[Path]):
    for root in roots:
        if not root.exists():
            continue
        if root.is_file():
            yield root
            continue
        for path in root.rglob("*"):
            if path.is_file() and "tmp" not in path.parts and "node_modules" not in path.parts:
                yield path


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def main() -> int:
    budget_inventory = read_json(BUDGET_INVENTORY_PATH)
    policy = read_json(POLICY_PATH)
    waiver_registry = read_json(WAIVER_REGISTRY_PATH)
    schema = read_json(SCHEMA_PATH)
    package_json = read_json(ROOT / "package.json")
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    task_hygiene_text = TASK_HYGIENE_PATH.read_text(encoding="utf-8")

    package_scripts = package_json.get("scripts", {})
    script_categories = count_package_script_categories(package_json)
    live_roots = [
        ROOT / ".github",
        ROOT / "docs",
        ROOT / "showcase",
        ROOT / "scripts",
        ROOT / "tests",
        ROOT / "native",
        ROOT / "README.md",
        ROOT / "CONTRIBUTING.md",
        ROOT / "package.json",
    ]

    legacy_alias_hits: list[str] = []
    for path in iter_live_files(live_roots):
        try:
            text = path.read_text(encoding="utf-8", errors="ignore")
        except Exception:
            continue
        if LEGACY_ALIAS_RE.search(text):
            legacy_alias_hits.append(repo_rel(path))

    waivers = waiver_registry.get("waivers", [])
    if not isinstance(waivers, list):
        waivers = []
    active_waivers = [waiver for waiver in waivers if isinstance(waiver, dict) and waiver.get("status") == "active"]
    expired_waivers = [waiver for waiver in waivers if isinstance(waiver, dict) and waiver.get("status") == "expired"]

    measured = {
        "package_script_count": len(package_scripts),
        "package_script_budget": int(re.search(r"PACKAGE_SCRIPT_BUDGET = (\d+)", task_hygiene_text).group(1)),
        "package_script_category_count": len(script_categories),
        "package_script_categories": script_categories,
        "public_workflow_action_count": count_action_definitions(ROOT / "scripts" / "objc3c_public_workflow_runner.py"),
        "runbook_count": len(list((ROOT / "docs" / "runbooks").glob("*.md"))),
        "schema_count": len(list((ROOT / "schemas").glob("*.json"))),
        "live_check_script_count": len(list((ROOT / "scripts").rglob("check_*.py"))),
        "must_remain_absent": {
            "docs_contracts_exists": (ROOT / "docs" / "contracts").exists(),
            "spec_planning_exists": (ROOT / "spec" / "planning").exists(),
            "compiler_tree_exists": (ROOT / "compiler").exists(),
            "milestone_workflow_count": len(list((ROOT / ".github" / "workflows").glob("m*.yml"))),
            "checker_wrapper_test_count": len(list((ROOT / "tests" / "tooling").glob("test_check_*.py"))),
            "lane_readiness_runner_count": len(list((ROOT / "scripts").rglob("run_*_lane_*_readiness.py"))),
            "legacy_alias_reference_count": len(legacy_alias_hits),
            "legacy_alias_reference_examples": legacy_alias_hits[:5],
        },
        "known_live_governance_drifts": {
            "milestone_checker_count": len(list((ROOT / "scripts").glob("check_m*.py"))),
            "milestone_checker_examples": [repo_rel(path) for path in sorted((ROOT / "scripts").glob("check_m*.py"))[:5]],
            "live_pyc_count": len([path for path in ROOT.rglob("*.pyc") if "tmp" not in path.parts]),
            "live_pycache_dir_count": len([path for path in ROOT.rglob("__pycache__") if "tmp" not in path.parts]),
        },
    }

    failures: list[str] = []
    if measured["package_script_count"] > measured["package_script_budget"]:
        failures.append("package script count exceeds budget")
    must_absent = measured["must_remain_absent"]
    if must_absent["docs_contracts_exists"]:
        failures.append("docs/contracts must remain absent")
    if must_absent["spec_planning_exists"]:
        failures.append("spec/planning must remain absent")
    if must_absent["compiler_tree_exists"]:
        failures.append("compiler must remain absent")
    if must_absent["milestone_workflow_count"]:
        failures.append("milestone-named workflows must remain absent")
    if must_absent["checker_wrapper_test_count"]:
        failures.append("checker-wrapper tests must remain absent")
    if must_absent["lane_readiness_runner_count"]:
        failures.append("lane readiness runners must remain absent")
    if must_absent["legacy_alias_reference_count"]:
        failures.append("legacy npm alias references must remain absent")
    live_drift = measured["known_live_governance_drifts"]
    if live_drift["milestone_checker_count"]:
        failures.append("milestone-scoped checkers must not be live without a waiver")
    if live_drift["live_pyc_count"]:
        failures.append("live .pyc files must not be present")
    if live_drift["live_pycache_dir_count"]:
        failures.append("live __pycache__ directories must not be present")
    if expired_waivers:
        failures.append("expired waivers are present")

    summary = {
        "contract_id": "objc3c.governance.sustainability.budget.summary.v1",
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "budget_inventory_contract": budget_inventory["contract_id"],
        "policy_contract": policy["contract_id"],
        "waiver_registry_contract": waiver_registry["contract_id"],
        "measured": measured,
        "waiver_status_summary": {
            "waiver_count": len(waivers),
            "active_waiver_count": len(active_waivers),
            "expired_waiver_count": len(expired_waivers),
        },
        "schema_path": schema.get("$id"),
        "runbook_mentions_enforcement": "python scripts/check_governance_sustainability_budget_enforcement.py" in runbook_text,
        "failures": failures,
    }

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print(f"governance-sustainability-budget-enforcement: {summary['status']}")
    if failures:
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
