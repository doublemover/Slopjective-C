#!/usr/bin/env python3
from __future__ import annotations

import json
import re
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/governance_sustainability/budget_inventory.json"
OUT_DIR = ROOT / "tmp/reports/m318/M318-A001"
SUMMARY_PATH = OUT_DIR / "governance_budget_inventory_summary.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_governance_sustainability.md"
TASK_HYGIENE_PATH = ROOT / "scripts/ci/check_task_hygiene.py"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def count_action_definitions(path: Path) -> int:
    return sum(1 for line in path.read_text(encoding="utf-8").splitlines() if line.startswith("def action_"))


def count_package_script_categories(package_json: dict[str, Any]) -> dict[str, int]:
    categories: dict[str, int] = {}
    for name in package_json.get("scripts", {}):
        key = name.split(":", 1)[0] if ":" in name else name
        categories[key] = categories.get(key, 0) + 1
    return dict(sorted(categories.items()))


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


def main() -> int:
    contract = read_json(CONTRACT_PATH)
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

    legacy_alias_re = re.compile(
        r"npm run (check:objc3c:m|test:tooling:m|check:compiler-closeout:m|run:objc3c:|plan:compiler-dispatch:|refresh:compiler-dispatch:|dev:objc3c:)"
    )
    legacy_alias_hits = []
    for path in iter_live_files(live_roots):
        try:
            text = path.read_text(encoding="utf-8", errors="ignore")
        except Exception:
            continue
        if legacy_alias_re.search(text):
            legacy_alias_hits.append(path.relative_to(ROOT).as_posix())

    measured = {
        "package_script_count": len(package_scripts),
        "package_script_budget": int(re.search(r"PACKAGE_SCRIPT_BUDGET = (\d+)", task_hygiene_text).group(1)),
        "package_script_category_count": len(script_categories),
        "package_script_categories": script_categories,
        "public_workflow_action_count": count_action_definitions(ROOT / "scripts/objc3c_public_workflow_runner.py"),
        "runbook_count": len(list((ROOT / "docs/runbooks").glob("*.md"))),
        "schema_count": len(list((ROOT / "schemas").glob("*.json"))),
        "live_check_script_count": len(list((ROOT / "scripts").rglob("check_*.py"))),
        "must_remain_absent": {
            "docs_contracts_exists": (ROOT / "docs/contracts").exists(),
            "spec_planning_exists": (ROOT / "spec/planning").exists(),
            "compiler_tree_exists": (ROOT / "compiler").exists(),
            "milestone_workflow_count": len(list((ROOT / ".github/workflows").glob("m*.yml"))),
            "checker_wrapper_test_count": len(list((ROOT / "tests/tooling").glob("test_check_*.py"))),
            "lane_readiness_runner_count": len(list((ROOT / "scripts").rglob("run_*_lane_*_readiness.py"))),
            "legacy_alias_reference_count": len(legacy_alias_hits),
            "legacy_alias_reference_examples": legacy_alias_hits[:5],
        },
        "known_live_governance_drifts": {
            "milestone_checker_count": len(list((ROOT / "scripts").glob("check_m*.py"))),
            "milestone_checker_examples": [path.relative_to(ROOT).as_posix() for path in sorted((ROOT / "scripts").glob("check_m*.py"))[:5]],
            "live_pyc_count": len([path for path in ROOT.rglob("*.pyc") if "tmp" not in path.parts]),
            "live_pycache_dir_count": len([path for path in ROOT.rglob("__pycache__") if "tmp" not in path.parts]),
        },
        "governance_entry_surface_count": sum(1 for path in contract["governance_entry_surfaces"] if (ROOT / path).exists()),
    }

    summary = {
        "issue": "M318-A001",
        "contract_id": contract["contract_id"],
        "runbook_mentions_contract": "tests/tooling/fixtures/governance_sustainability/budget_inventory.json" in runbook_text,
        "summary_script_matches_runbook": "python scripts/build_governance_budget_inventory_summary.py" in runbook_text,
        "measured": measured,
        "explicit_non_goal_count": len(contract["explicit_non_goals"]),
        "successor_issue_count": len(contract["successor_issues"]),
    }
    summary["ok"] = all(
        [
            summary["runbook_mentions_contract"],
            summary["summary_script_matches_runbook"],
            measured["package_script_count"] <= measured["package_script_budget"],
            measured["must_remain_absent"]["docs_contracts_exists"] is False,
            measured["must_remain_absent"]["spec_planning_exists"] is False,
            measured["must_remain_absent"]["compiler_tree_exists"] is False,
            measured["must_remain_absent"]["milestone_workflow_count"] == 0,
            measured["must_remain_absent"]["checker_wrapper_test_count"] == 0,
            measured["must_remain_absent"]["lane_readiness_runner_count"] == 0,
            measured["must_remain_absent"]["legacy_alias_reference_count"] == 0,
        ]
    )

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
