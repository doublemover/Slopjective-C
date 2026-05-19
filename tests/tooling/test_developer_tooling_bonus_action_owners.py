from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.actions.developer_tooling_bonus_inputs import (
    BonusSurfaceInputs,
)
from scripts.objc3c_workflow.actions.developer_tooling_bonus_payload import (
    bonus_tool_payload,
)

ROOT = Path(__file__).resolve().parents[2]
ACTION_ROOT = ROOT / "scripts" / "objc3c_workflow" / "actions"

BONUS_OWNER_MODULES = (
    "developer_tooling_bonus_artifacts",
    "developer_tooling_bonus_inputs",
    "developer_tooling_bonus_payload",
    "developer_tooling_bonus_inspection",
    "developer_tooling_bonus_templates",
)


def test_bonus_action_owner_modules_are_explicit() -> None:
    for module_name in BONUS_OWNER_MODULES:
        assert importlib.import_module(
            f"scripts.objc3c_workflow.actions.{module_name}"
        )


def test_bonus_inspection_action_delegates_owned_details() -> None:
    source = (ACTION_ROOT / "developer_tooling_bonus_inspection.py").read_text(
        encoding="utf-8"
    )

    assert "from .developer_tooling_bonus_artifacts import" in source
    assert "from .developer_tooling_bonus_inputs import" in source
    assert "from .developer_tooling_bonus_payload import" in source
    assert "SHOWCASE_PORTFOLIO_JSON" not in source
    assert "SHOWCASE_TUTORIAL_WALKTHROUGH_JSON" not in source
    assert "build-native-contracts" not in source


def test_bonus_payload_preserves_inspection_contract() -> None:
    payload = bonus_tool_payload(
        BonusSurfaceInputs(
            source_of_truth={
                "bonus_tool_integration_surface": {"tooling": "present"},
                "bonus_experience_surfaces": {"template": "derived"},
            },
            portfolio={"contract_id": "objc3c.showcase.portfolio.v1"},
            walkthrough={"contract_id": "objc3c.guided.walkthrough.v1"},
        )
    )

    assert payload["contract_id"] == "objc3c.bonus.tool.integration.surface.v1"
    assert payload["integration_surface"] == {"tooling": "present"}
    assert payload["bonus_experience_surfaces"] == {"template": "derived"}
    assert payload["showcase_portfolio_contract_id"] == (
        "objc3c.showcase.portfolio.v1"
    )
    assert payload["guided_walkthrough_contract_id"] == (
        "objc3c.guided.walkthrough.v1"
    )


def test_bonus_payload_requires_source_of_truth_integration_surface() -> None:
    try:
        bonus_tool_payload(
            BonusSurfaceInputs(
                source_of_truth={},
                portfolio={"contract_id": "objc3c.showcase.portfolio.v1"},
                walkthrough={"contract_id": "objc3c.guided.walkthrough.v1"},
            )
        )
    except ValueError as exc:
        assert str(exc) == (
            "repo superclean artifact missing bonus_tool_integration_surface"
        )
    else:
        raise AssertionError("missing bonus surface did not fail closed")
