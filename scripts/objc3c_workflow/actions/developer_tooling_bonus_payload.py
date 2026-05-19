"""Payload assembly for bonus-tool integration inspection."""

from __future__ import annotations

from ..environment import ROOT
from .developer_tooling_bonus_inputs import BonusSurfaceInputs
from .developer_tooling_paths import REPO_SUPERCLEAN_SOURCE_OF_TRUTH


def bonus_tool_payload(inputs: BonusSurfaceInputs) -> dict[str, object]:
    integration_surface = inputs.source_of_truth.get("bonus_tool_integration_surface")
    if not isinstance(integration_surface, dict):
        raise ValueError(
            "repo superclean artifact missing bonus_tool_integration_surface"
        )
    return {
        "contract_id": "objc3c.bonus.tool.integration.surface.v1",
        "schema_version": 1,
        "source_of_truth_artifact": REPO_SUPERCLEAN_SOURCE_OF_TRUTH.relative_to(
            ROOT
        ).as_posix(),
        "integration_surface": integration_surface,
        "bonus_experience_surfaces": inputs.source_of_truth.get(
            "bonus_experience_surfaces",
            {},
        ),
        "showcase_portfolio_contract_id": inputs.portfolio.get("contract_id"),
        "guided_walkthrough_contract_id": inputs.walkthrough.get("contract_id"),
    }
