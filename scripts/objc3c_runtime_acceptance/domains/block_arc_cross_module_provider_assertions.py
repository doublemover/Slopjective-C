"""Cross-module Block/ARC provider surface assertions."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect

from .block_arc_cross_module_artifacts import CrossModuleBlockOwnershipArtifacts
from .block_arc_cross_module_contracts import (
    PROVIDER_BLOCK_OWNERSHIP_COUNTS,
    PROVIDER_BLOCK_OWNERSHIP_FIELDS,
)


def assert_provider_block_ownership_surface(
    artifacts: CrossModuleBlockOwnershipArtifacts,
) -> None:
    provider_block_surface = artifacts.provider_block_surface
    expect(
        isinstance(provider_block_surface, dict),
        "expected block-ownership provider import surface to publish the preservation packet",
    )
    for field_name, expected_value in PROVIDER_BLOCK_OWNERSHIP_FIELDS.items():
        expect(
            provider_block_surface.get(field_name) == expected_value,
            f"expected block-ownership provider import surface to preserve {field_name}",
        )

    for field_name, expected_value in PROVIDER_BLOCK_OWNERSHIP_COUNTS:
        expect(
            provider_block_surface.get(field_name) == expected_value,
            f"expected block-ownership provider import surface to preserve {field_name}",
        )
    expect(
        provider_block_surface.get("runtime_import_artifact_ready") is True
        and provider_block_surface.get("separate_compilation_preservation_ready")
        is True
        and provider_block_surface.get("runtime_support_library_link_wiring_ready")
        is True
        and provider_block_surface.get("deterministic") is True,
        "expected block-ownership provider import surface to be import-ready deterministic and runtime-link ready",
    )
    expect(
        isinstance(provider_block_surface.get("replay_key"), str)
        and provider_block_surface.get("replay_key") != "",
        "expected block-ownership provider import surface to publish a replay key",
    )


__all__ = ["assert_provider_block_ownership_surface"]
