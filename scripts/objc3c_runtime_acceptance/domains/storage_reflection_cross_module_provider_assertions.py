"""Cross-module storage/reflection provider surface assertions."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect

from .storage_reflection_cross_module_artifacts import (
    CrossModuleStorageReflectionArtifacts,
)
from .storage_reflection_cross_module_contracts import (
    PROVIDER_STORAGE_FIELDS,
    STORAGE_REFLECTION_ENTRY_COUNTS,
)


def assert_provider_storage_surface(
    artifacts: CrossModuleStorageReflectionArtifacts,
) -> None:
    provider_storage_surface = artifacts.provider_storage_surface
    expect(
        isinstance(provider_storage_surface, dict),
        "expected storage-reflection provider import surface to publish the preservation packet",
    )
    for field_name, expected_value in PROVIDER_STORAGE_FIELDS.items():
        expect(
            provider_storage_surface.get(field_name) == expected_value,
            f"expected storage-reflection provider import surface to preserve {field_name}",
        )

    expect(
        provider_storage_surface.get("local_property_descriptor_count")
        == artifacts.provider_registration_manifest.get("property_descriptor_count")
        == 6,
        "expected storage-reflection provider import surface to preserve six property descriptors",
    )
    expect(
        provider_storage_surface.get("local_ivar_descriptor_count")
        == artifacts.provider_registration_manifest.get("ivar_descriptor_count")
        == 3,
        "expected storage-reflection provider import surface to preserve three ivar descriptors",
    )
    for field_name, expected_value in STORAGE_REFLECTION_ENTRY_COUNTS:
        expect(
            provider_storage_surface.get(field_name) == expected_value,
            f"expected storage-reflection provider import surface to preserve {field_name}",
        )
    expect(
        provider_storage_surface.get("runtime_import_artifact_ready") is True
        and provider_storage_surface.get("separate_compilation_preservation_ready")
        is True
        and provider_storage_surface.get("deterministic") is True,
        "expected storage-reflection provider import surface to be import-ready deterministic and separate-compilation ready",
    )
    expect(
        isinstance(provider_storage_surface.get("replay_key"), str)
        and provider_storage_surface.get("replay_key") != "",
        "expected storage-reflection provider import surface to publish a replay key",
    )


__all__ = ["assert_provider_storage_surface"]
