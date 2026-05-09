"""Bonus-tool integration inspection workflow action."""

from __future__ import annotations

import json
import sys

from ..environment import ROOT
from .developer_tooling_paths import (
    PUBLIC_WORKFLOW_REPORT_ROOT,
    REPO_SUPERCLEAN_SOURCE_OF_TRUTH,
    SHOWCASE_PORTFOLIO_JSON,
    SHOWCASE_TUTORIAL_WALKTHROUGH_JSON,
)


def execute_registered_action(action: str, rest: list[str]) -> int:
    from scripts.objc3c_workflow.action_dispatch import execute_registered_action as execute

    return execute(action, rest)


def _ensure_bonus_artifact_source() -> int:
    native_main = ROOT / "native" / "objc3c" / "src" / "main.cpp"
    if native_main.is_file():
        return execute_registered_action("build-native-contracts", [])
    return 0


def _load_bonus_surface_inputs() -> tuple[dict[str, object], dict[str, object], dict[str, object]]:
    source_of_truth = json.loads(REPO_SUPERCLEAN_SOURCE_OF_TRUTH.read_text(encoding="utf-8"))
    portfolio = json.loads(SHOWCASE_PORTFOLIO_JSON.read_text(encoding="utf-8"))
    walkthrough = json.loads(SHOWCASE_TUTORIAL_WALKTHROUGH_JSON.read_text(encoding="utf-8"))
    return source_of_truth, portfolio, walkthrough


def _bonus_tool_payload(
    source_of_truth: dict[str, object],
    portfolio: dict[str, object],
    walkthrough: dict[str, object],
) -> dict[str, object]:
    integration_surface = source_of_truth.get("bonus_tool_integration_surface")
    if not isinstance(integration_surface, dict):
        raise ValueError("repo superclean artifact missing bonus_tool_integration_surface")
    return {
        "contract_id": "objc3c.bonus.tool.integration.surface.v1",
        "schema_version": 1,
        "source_of_truth_artifact": REPO_SUPERCLEAN_SOURCE_OF_TRUTH.relative_to(
            ROOT
        ).as_posix(),
        "integration_surface": integration_surface,
        "bonus_experience_surfaces": source_of_truth.get("bonus_experience_surfaces", {}),
        "showcase_portfolio_contract_id": portfolio.get("contract_id"),
        "guided_walkthrough_contract_id": walkthrough.get("contract_id"),
    }


def action_inspect_bonus_tool_integration(_: list[str]) -> int:
    rc = _ensure_bonus_artifact_source()
    if rc != 0:
        return rc
    if not REPO_SUPERCLEAN_SOURCE_OF_TRUTH.is_file():
        print(
            f"missing source-of-truth artifact: {REPO_SUPERCLEAN_SOURCE_OF_TRUTH.relative_to(ROOT).as_posix()}",
            file=sys.stderr,
        )
        return 1

    try:
        payload = _bonus_tool_payload(*_load_bonus_surface_inputs())
    except ValueError as exc:
        print(str(exc), file=sys.stderr)
        return 1

    dump_path = PUBLIC_WORKFLOW_REPORT_ROOT / "bonus-tool-integration.json"
    dump_path.parent.mkdir(parents=True, exist_ok=True)
    dump_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {dump_path.relative_to(ROOT).as_posix()}")
    print(f"dump_path: {dump_path.relative_to(ROOT).as_posix()}")
    return 0
