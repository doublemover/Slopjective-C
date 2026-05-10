"""Assertions for cross-module Block/ARC preservation cases."""

from __future__ import annotations

from .block_arc_cross_module_artifacts import CrossModuleBlockOwnershipArtifacts
from .block_arc_cross_module_link_plan_assertions import (
    assert_cross_module_block_ownership_link_plan_contracts,
    assert_imported_module_block_ownership_surface,
    assert_link_plan_block_ownership_totals,
)
from .block_arc_cross_module_provider_assertions import (
    assert_provider_block_ownership_surface,
)


def assert_cross_module_block_ownership_artifacts(
    artifacts: CrossModuleBlockOwnershipArtifacts,
) -> None:
    assert_provider_block_ownership_surface(artifacts)
    assert_cross_module_block_ownership_link_plan_contracts(artifacts)
    assert_imported_module_block_ownership_surface(artifacts)
    assert_link_plan_block_ownership_totals(artifacts)


__all__ = ["assert_cross_module_block_ownership_artifacts"]
