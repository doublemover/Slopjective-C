"""Summary helpers for cross-module Block/ARC preservation cases."""

from __future__ import annotations

from typing import Any

from ..runtime_contract_block_arc import (
    BLOCK_OWNERSHIP_PRESERVATION_CONSUMER_FIXTURE,
    BLOCK_OWNERSHIP_PRESERVATION_PROVIDER_FIXTURE,
)
from .block_arc_cross_module_artifacts import CrossModuleBlockOwnershipArtifacts


CROSS_MODULE_BLOCK_OWNERSHIP_CASE_ID = (
    "cross-module-block-ownership-artifact-preservation"
)


def build_cross_module_block_ownership_summary(
    artifacts: CrossModuleBlockOwnershipArtifacts,
    case_total_ms: int,
) -> dict[str, Any]:
    return {
        "provider_fixture": BLOCK_OWNERSHIP_PRESERVATION_PROVIDER_FIXTURE,
        "consumer_fixture": BLOCK_OWNERSHIP_PRESERVATION_CONSUMER_FIXTURE,
        "provider_compile_ms": artifacts.provider_compile_ms,
        "consumer_compile_ms": artifacts.consumer_compile_ms,
        "case_total_ms": case_total_ms,
        "provider_module_name": artifacts.provider_import_payload.get("module_name"),
        "consumer_module_name": artifacts.link_plan.get("local_module", {}).get(
            "module_name"
        ),
        "imported_block_literal_sites": artifacts.link_plan.get(
            "imported_block_ownership_block_literal_sites"
        ),
        "imported_copy_helper_required_sites": artifacts.link_plan.get(
            "imported_block_ownership_copy_helper_required_sites"
        ),
        "imported_byref_layout_symbolized_sites": artifacts.link_plan.get(
            "imported_block_ownership_byref_layout_symbolized_sites"
        ),
    }


__all__ = [
    "CROSS_MODULE_BLOCK_OWNERSHIP_CASE_ID",
    "build_cross_module_block_ownership_summary",
]
