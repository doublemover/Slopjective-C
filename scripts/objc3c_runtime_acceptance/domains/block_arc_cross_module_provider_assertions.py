"""Cross-module Block/ARC provider surface assertions."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect

from .block_arc_cross_module_artifacts import CrossModuleBlockOwnershipArtifacts
from .block_arc_cross_module_contracts import (
    AUTORELEASEPOOL_SCOPE_LOWERING_CONTRACT_ID,
    PROVIDER_BLOCK_OWNERSHIP_COUNTS,
    PROVIDER_BLOCK_OWNERSHIP_FIELDS,
    RETAIN_RELEASE_OPERATION_LOWERING_CONTRACT_ID,
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
        and provider_block_surface.get("arc_cleanup_preservation_ready") is True
        and provider_block_surface.get("deterministic") is True,
        "expected block-ownership provider import surface to be import-ready deterministic runtime-link and ARC-cleanup ready",
    )
    expect(
        isinstance(provider_block_surface.get("replay_key"), str)
        and provider_block_surface.get("replay_key") != "",
        "expected block-ownership provider import surface to publish a replay key",
    )
    retain_release_replay_key = provider_block_surface.get(
        "retain_release_operation_lowering_replay_key"
    )
    autoreleasepool_replay_key = provider_block_surface.get(
        "autoreleasepool_scope_lowering_replay_key"
    )
    expect(
        isinstance(retain_release_replay_key, str)
        and RETAIN_RELEASE_OPERATION_LOWERING_CONTRACT_ID in retain_release_replay_key
        and "contract_violation_sites=0" in retain_release_replay_key,
        "expected block-ownership provider import surface to preserve retain/release cleanup replay",
    )
    expect(
        isinstance(autoreleasepool_replay_key, str)
        and AUTORELEASEPOOL_SCOPE_LOWERING_CONTRACT_ID in autoreleasepool_replay_key
        and "contract_violation_sites=0" in autoreleasepool_replay_key,
        "expected block-ownership provider import surface to preserve autoreleasepool cleanup replay",
    )
    autoreleasepool_sites = provider_block_surface.get(
        "local_autoreleasepool_scope_sites"
    )
    expect(
        provider_block_surface.get("local_arc_contract_violation_sites") == 0
        and provider_block_surface.get(
            "local_autoreleasepool_contract_violation_sites"
        )
        == 0
        and provider_block_surface.get(
            "local_autoreleasepool_scope_symbolized_sites"
        )
        <= autoreleasepool_sites
        and provider_block_surface.get(
            "local_autoreleasepool_scope_entry_transition_sites"
        )
        == autoreleasepool_sites
        and provider_block_surface.get(
            "local_autoreleasepool_scope_exit_transition_sites"
        )
        == autoreleasepool_sites,
        "expected block-ownership provider ARC cleanup counters to be internally consistent",
    )


__all__ = ["assert_provider_block_ownership_surface"]
