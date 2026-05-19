"""Cross-module Block/ARC link-plan assertions."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect

from .block_arc_cross_module_artifacts import CrossModuleBlockOwnershipArtifacts
from .block_arc_cross_module_contracts import (
    IMPORTED_BLOCK_OWNERSHIP_COUNTS,
    IMPORTED_MODULE_FIELDS,
    LINK_PLAN_CONTRACT_FIELDS,
    LOCAL_BLOCK_OWNERSHIP_COUNTS,
    TRANSITIVE_BLOCK_OWNERSHIP_COUNTS,
)


def assert_cross_module_block_ownership_link_plan_contracts(
    artifacts: CrossModuleBlockOwnershipArtifacts,
) -> None:
    link_plan = artifacts.link_plan
    for field_name, expected_value in LINK_PLAN_CONTRACT_FIELDS:
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module block-ownership link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("block_ownership_cross_module_preservation_ready") is True,
        "expected cross-module link plan to mark block-ownership preservation ready",
    )


def assert_imported_module_block_ownership_surface(
    artifacts: CrossModuleBlockOwnershipArtifacts,
) -> None:
    imported_modules = artifacts.link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected block-ownership link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        imported_module.get("module_name")
        == artifacts.provider_import_payload.get("module_name")
        == "m261_byref_cell_copy_dispose_runtime_positive",
        "expected block-ownership link plan to preserve the provider module name",
    )
    for field_name, expected_value in IMPORTED_MODULE_FIELDS:
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported block-ownership module to preserve {field_name}",
        )
    expect(
        isinstance(imported_module.get("block_ownership_replay_key"), str)
        and imported_module.get("block_ownership_replay_key") != "",
        "expected imported block-ownership module to preserve a replay key",
    )


def assert_link_plan_block_ownership_totals(
    artifacts: CrossModuleBlockOwnershipArtifacts,
) -> None:
    for field_name, expected_value in LOCAL_BLOCK_OWNERSHIP_COUNTS.items():
        expect(
            artifacts.link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in IMPORTED_BLOCK_OWNERSHIP_COUNTS.items():
        expect(
            artifacts.link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in TRANSITIVE_BLOCK_OWNERSHIP_COUNTS.items():
        expect(
            artifacts.link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )


__all__ = [
    "assert_cross_module_block_ownership_link_plan_contracts",
    "assert_imported_module_block_ownership_surface",
    "assert_link_plan_block_ownership_totals",
]
