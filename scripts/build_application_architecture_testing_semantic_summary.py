#!/usr/bin/env python3
"""Build the first-party testing semantics summary."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "application_architecture_testing" / "first_party_testing_semantics.json"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "application-architecture-testing" / "first-party-testing-semantics-summary.json"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    package = load_json(PACKAGE_JSON)
    package_scripts = package.get("scripts", {})
    if not isinstance(package_scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")

    required_paths = [str(contract["workflow_runner"]), *[str(path) for path in contract["checked_in_fixture_roots"]]]
    missing_paths = []
    for raw in required_paths:
      path = ROOT / raw
      if not path.exists():
        missing_paths.append(raw)

    public_scripts = [str(name) for name in contract["public_scripts"]]
    public_actions = [str(name) for name in contract["public_actions"]]
    missing_public_scripts = [name for name in public_scripts if name not in package_scripts]

    tooling_test_count = len(list((ROOT / "tests" / "tooling").glob("test_*.py")))
    showcase_workspace_count = len(list((ROOT / "showcase").glob("*/workspace.json")))
    tutorial_doc_count = len(list((ROOT / "docs" / "tutorials").glob("*.md")))

    payload = {
        "contract_id": "objc3c.application.architecture.testing.first_party_testing_semantics.summary.v1",
        "status": "PASS" if not missing_paths and not missing_public_scripts else "FAIL",
        "testing_contract": repo_rel(CONTRACT_PATH),
        "runbook": str(contract["runbook"]),
        "testing_layer_count": len(contract["testing_layers"]),
        "checked_in_fixture_root_count": len(contract["checked_in_fixture_roots"]),
        "generated_output_root_count": len(contract["generated_output_roots"]),
        "public_action_count": len(public_actions),
        "public_script_count": len(public_scripts),
        "tooling_test_count": tooling_test_count,
        "showcase_workspace_count": showcase_workspace_count,
        "tutorial_doc_count": tutorial_doc_count,
        "public_actions": public_actions,
        "public_scripts": public_scripts,
        "testing_layers": contract["testing_layers"],
        "fixture_rules": contract["fixture_rules"],
        "missing_paths": missing_paths,
        "missing_public_scripts": missing_public_scripts,
        "non_goals": contract["non_goals"],
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("application-architecture-testing-semantics: PASS" if payload["status"] == "PASS" else "application-architecture-testing-semantics: FAIL")
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
