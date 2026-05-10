"""Assertions for cross-module storage/reflection preservation cases."""

from __future__ import annotations

from .storage_reflection_cross_module_artifacts import (
    CrossModuleStorageReflectionArtifacts,
)
from .storage_reflection_cross_module_link_plan_assertions import (
    assert_cross_module_link_plan_contracts,
    assert_imported_module_storage_reflection_surface,
    assert_link_plan_storage_reflection_totals,
)
from .storage_reflection_cross_module_provider_assertions import (
    assert_provider_storage_surface,
)


def assert_cross_module_storage_reflection_artifacts(
    artifacts: CrossModuleStorageReflectionArtifacts,
) -> None:
    assert_provider_storage_surface(artifacts)
    assert_cross_module_link_plan_contracts(artifacts)
    assert_imported_module_storage_reflection_surface(artifacts)
    assert_link_plan_storage_reflection_totals(artifacts)


__all__ = ["assert_cross_module_storage_reflection_artifacts"]
