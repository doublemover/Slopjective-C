"""Bonus-experience and project-template developer-tooling actions."""

from __future__ import annotations

import json
import sys

from ..commands import run
from ..environment import ROOT
from .developer_tooling_paths import (
    BONUS_EXPERIENCE_INTEGRATION_PY,
    PROJECT_TEMPLATE_MATERIALIZER_PY,
    PUBLIC_WORKFLOW_REPORT_ROOT,
    REPO_SUPERCLEAN_SOURCE_OF_TRUTH,
    RUNNABLE_BONUS_EXPERIENCE_E2E_PY,
    SHOWCASE_PORTFOLIO_JSON,
    SHOWCASE_TUTORIAL_WALKTHROUGH_JSON,
)


def _execute_registered_action(action: str, rest: list[str]) -> int:
    from scripts.objc3c_workflow.action_dispatch import execute_registered_action

    return execute_registered_action(action, rest)


def action_validate_bonus_experiences(_: list[str]) -> int:
    return run([sys.executable, str(BONUS_EXPERIENCE_INTEGRATION_PY)])


def action_inspect_bonus_tool_integration(_: list[str]) -> int:
    native_main = ROOT / "native" / "objc3c" / "src" / "main.cpp"
    if native_main.is_file():
        rc = _execute_registered_action("build-native-contracts", [])
        if rc != 0:
            return rc
    if not REPO_SUPERCLEAN_SOURCE_OF_TRUTH.is_file():
        print(
            f"missing source-of-truth artifact: {REPO_SUPERCLEAN_SOURCE_OF_TRUTH.relative_to(ROOT).as_posix()}",
            file=sys.stderr,
        )
        return 1
    source_of_truth = json.loads(REPO_SUPERCLEAN_SOURCE_OF_TRUTH.read_text(encoding="utf-8"))
    portfolio = json.loads(SHOWCASE_PORTFOLIO_JSON.read_text(encoding="utf-8"))
    walkthrough = json.loads(SHOWCASE_TUTORIAL_WALKTHROUGH_JSON.read_text(encoding="utf-8"))
    integration_surface = source_of_truth.get("bonus_tool_integration_surface")
    if not isinstance(integration_surface, dict):
        print("repo superclean artifact missing bonus_tool_integration_surface", file=sys.stderr)
        return 1
    dump_path = PUBLIC_WORKFLOW_REPORT_ROOT / "bonus-tool-integration.json"
    dump_path.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "contract_id": "objc3c.bonus.tool.integration.surface.v1",
        "schema_version": 1,
        "source_of_truth_artifact": REPO_SUPERCLEAN_SOURCE_OF_TRUTH.relative_to(ROOT).as_posix(),
        "integration_surface": integration_surface,
        "bonus_experience_surfaces": source_of_truth.get("bonus_experience_surfaces", {}),
        "showcase_portfolio_contract_id": portfolio.get("contract_id"),
        "guided_walkthrough_contract_id": walkthrough.get("contract_id"),
    }
    dump_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {dump_path.relative_to(ROOT).as_posix()}")
    print(f"dump_path: {dump_path.relative_to(ROOT).as_posix()}")
    return 0


def action_materialize_project_template(rest: list[str]) -> int:
    return run([sys.executable, str(PROJECT_TEMPLATE_MATERIALIZER_PY), *rest])


def action_validate_runnable_bonus_experiences(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_BONUS_EXPERIENCE_E2E_PY)])
