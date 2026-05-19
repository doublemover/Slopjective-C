"""Codegen assertions for storage/reflection layout lowering."""

from __future__ import annotations

from typing import cast

from objc3c_runtime_acceptance.domains.storage_reflection_lowering_layout_contract_assertions import (
    ManifestSurface,
)
from objc3c_runtime_acceptance.expectation_matching import expect


def assert_registration_manifest_layout_counts(
    registration_manifest: ManifestSurface,
) -> None:
    expect(
        registration_manifest.get("property_descriptor_count") == 6,
        "expected registration manifest to publish six property descriptors for the synthesized accessor lowering fixture",
    )
    expect(
        registration_manifest.get("ivar_descriptor_count") == 3,
        "expected registration manifest to publish three ivar descriptors for the synthesized accessor lowering fixture",
    )


def assert_synthesized_accessor_codegen_manifest(
    manifest: ManifestSurface,
    registration_manifest: ManifestSurface,
) -> ManifestSurface:
    synthesis_summary = manifest.get("lowering_property_synthesis_ivar_binding", {})
    expect(
        isinstance(synthesis_summary, dict),
        "expected property synthesis lowering summary in compile manifest",
    )
    synthesis_summary = cast(ManifestSurface, synthesis_summary)
    expect(
        synthesis_summary.get("deterministic_handoff") is True,
        "expected property synthesis lowering summary to report deterministic handoff",
    )
    replay_key = synthesis_summary.get("replay_key", "")
    expect(
        "property_synthesis_sites=3" in replay_key,
        "expected property synthesis replay key to record the three synthesized properties",
    )
    expect(
        "property_synthesis_default_ivar_bindings=3" in replay_key,
        "expected property synthesis replay key to record the default ivar bindings",
    )
    expect(
        registration_manifest.get("property_descriptor_count", 0) >= 6,
        "expected runtime registration manifest to publish synthesized property descriptors",
    )

    lowering_surface = manifest.get(
        "dispatch_and_synthesized_accessor_lowering_surface", {}
    )
    expect(
        isinstance(lowering_surface, dict),
        "expected authoritative dispatch and synthesized-accessor lowering surface in compile manifest",
    )
    lowering_surface = cast(ManifestSurface, lowering_surface)
    _assert_codegen_lowering_surface(lowering_surface, registration_manifest)
    return lowering_surface


def _assert_codegen_lowering_surface(
    lowering_surface: ManifestSurface,
    registration_manifest: ManifestSurface,
) -> None:
    expect(
        lowering_surface.get("contract_id")
        == "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
        "expected lowering surface contract id for dispatch and synthesized accessors",
    )
    expect(
        lowering_surface.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
        "expected lowering surface to publish canonical runtime dispatch symbol",
    )
    expect(
        lowering_surface.get("runtime_dispatch_symbol_matches_lowering") is True,
        "expected lowering surface to bind lowering and runtime library dispatch symbols together",
    )
    expect(
        lowering_surface.get("property_synthesis_sites") == 3,
        "expected lowering surface to publish three synthesized properties",
    )
    expect(
        lowering_surface.get("property_synthesis_default_ivar_bindings") == 3,
        "expected lowering surface to publish three default ivar bindings",
    )
    expect(
        lowering_surface.get("property_descriptor_count")
        == registration_manifest.get("property_descriptor_count"),
        "expected lowering surface property descriptor count to match runtime registration manifest",
    )
    expect(
        lowering_surface.get("ivar_descriptor_count")
        == registration_manifest.get("ivar_descriptor_count"),
        "expected lowering surface ivar descriptor count to match runtime registration manifest",
    )
    expect(
        lowering_surface.get("deterministic_handoff") is True,
        "expected lowering surface to report deterministic handoff",
    )


__all__ = [
    "assert_registration_manifest_layout_counts",
    "assert_synthesized_accessor_codegen_manifest",
]
