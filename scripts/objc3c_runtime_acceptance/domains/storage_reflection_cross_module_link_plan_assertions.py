"""Cross-module storage/reflection link-plan assertions."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect

from .storage_reflection_cross_module_artifacts import (
    CrossModuleStorageReflectionArtifacts,
)
from .storage_reflection_cross_module_contracts import (
    IMPORTED_MODULE_FIELDS,
    IMPORTED_STORAGE_REFLECTION_COUNTS,
    LINK_PLAN_CONTRACT_FIELDS,
    LOCAL_STORAGE_REFLECTION_COUNTS,
    TRANSITIVE_STORAGE_REFLECTION_COUNTS,
)


def assert_cross_module_link_plan_contracts(
    artifacts: CrossModuleStorageReflectionArtifacts,
) -> None:
    link_plan = artifacts.link_plan
    for field_name, expected_value in LINK_PLAN_CONTRACT_FIELDS:
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("storage_reflection_cross_module_preservation_ready") is True,
        "expected cross-module link plan to mark storage/reflection preservation ready",
    )


def assert_imported_module_storage_reflection_surface(
    artifacts: CrossModuleStorageReflectionArtifacts,
) -> None:
    imported_modules = artifacts.link_plan.get("imported_modules")
    expect(
        isinstance(imported_modules, list) and len(imported_modules) == 1,
        "expected storage-reflection link plan to publish exactly one imported module",
    )
    imported_module = imported_modules[0]
    expect(
        imported_module.get("module_name")
        == artifacts.provider_import_payload.get("module_name")
        == "synthesizedAccessorPropertyLowering",
        "expected storage-reflection link plan to preserve the provider module name",
    )
    for field_name, expected_value in IMPORTED_MODULE_FIELDS:
        expect(
            imported_module.get(field_name) == expected_value,
            f"expected imported storage-reflection module to preserve {field_name}",
        )
    expect(
        isinstance(imported_module.get("storage_reflection_replay_key"), str)
        and imported_module.get("storage_reflection_replay_key") != "",
        "expected imported storage-reflection module to preserve a replay key",
    )


def assert_link_plan_storage_reflection_totals(
    artifacts: CrossModuleStorageReflectionArtifacts,
) -> None:
    for field_name, expected_value in LOCAL_STORAGE_REFLECTION_COUNTS.items():
        expect(
            artifacts.link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in IMPORTED_STORAGE_REFLECTION_COUNTS.items():
        expect(
            artifacts.link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in TRANSITIVE_STORAGE_REFLECTION_COUNTS.items():
        expect(
            artifacts.link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )


__all__ = [
    "assert_cross_module_link_plan_contracts",
    "assert_imported_module_storage_reflection_surface",
    "assert_link_plan_storage_reflection_totals",
]
