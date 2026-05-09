"""Compile manifest assertions for storage/reflection lowering layout cases."""

from __future__ import annotations

from typing import Any, cast

from objc3c_runtime_acceptance.expectation_matching import expect

ManifestSurface = dict[str, Any]


def assert_property_source_surface_links(manifest: ManifestSurface) -> None:
    property_source_surface = cast(
        ManifestSurface,
        manifest.get("runtime_property_ivar_storage_accessor_source_surface", {}),
    )
    expect(
        property_source_surface.get(
            "executable_property_accessor_layout_lowering_contract_id"
        )
        == "objc3c.executable.property.accessor.layout.lowering.v1",
        "expected property/ivar storage source surface to point at the executable accessor/layout lowering surface",
    )
    expect(
        property_source_surface.get("executable_ivar_layout_emission_contract_id")
        == "objc3c.executable.ivar.layout.emission.v1",
        "expected property/ivar storage source surface to point at the executable ivar layout emission surface",
    )
    expect(
        property_source_surface.get(
            "executable_synthesized_accessor_property_lowering_contract_id"
        )
        == "objc3c.executable.synthesized.accessor.property.lowering.v1",
        "expected property/ivar storage source surface to point at the synthesized accessor lowering surface",
    )


def assert_accessor_layout_surface(manifest: ManifestSurface) -> ManifestSurface:
    accessor_layout_surface = manifest.get(
        "executable_property_accessor_layout_lowering_surface", {}
    )
    expect(
        isinstance(accessor_layout_surface, dict),
        "expected compile manifest to publish the executable accessor/layout lowering surface",
    )
    accessor_layout_surface = cast(ManifestSurface, accessor_layout_surface)
    expected_accessor_layout_fields = {
        "contract_id": "objc3c.executable.property.accessor.layout.lowering.v1",
        "runtime_property_ivar_storage_accessor_source_surface_contract_id": (
            "objc3c.runtime.property.ivar.storage.accessor.source.surface.v1"
        ),
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": (
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        ),
        "property_table_model": (
            "property-descriptor-bundles-carry-sema-approved-attribute-accessor-binding-and-layout-records"
        ),
        "ivar_layout_model": (
            "ivar-descriptor-bundles-carry-sema-approved-layout-symbol-replay-key-slot-offset-size-alignment-padding-inheritance-owner-size-records"
        ),
        "accessor_binding_model": (
            "effective-accessor-selectors-and-synthesized-binding-identities-pass-through-lowering-without-body-synthesis"
        ),
        "scope_model": "ast-sema-property-layout-handoff-ir-object-metadata-publication",
        "fail_closed_model": (
            "no-synthesized-accessor-bodies-no-runtime-storage-allocation-no-layout-rederivation"
        ),
        "compile_manifest_artifact": "module.manifest.json",
        "registration_manifest_artifact": "module.runtime-registration-manifest.json",
        "object_artifact": "module.obj",
        "backend_artifact": "module.ll",
    }
    for field, expected_value in expected_accessor_layout_fields.items():
        expect(
            accessor_layout_surface.get(field) == expected_value,
            f"expected accessor/layout lowering surface to preserve {field}",
        )
    expect(
        accessor_layout_surface.get("property_metadata_entries") == 6,
        "expected accessor/layout lowering surface to publish six property metadata entries",
    )
    expect(
        accessor_layout_surface.get("ivar_metadata_entries") == 3,
        "expected accessor/layout lowering surface to publish three ivar metadata entries",
    )
    expect(
        accessor_layout_surface.get("property_descriptor_entries") == 6,
        "expected accessor/layout lowering surface to publish six property descriptors",
    )
    expect(
        accessor_layout_surface.get("ivar_descriptor_entries") == 3,
        "expected accessor/layout lowering surface to publish three ivar descriptors",
    )
    expect(
        accessor_layout_surface.get("property_attribute_profile_entries") == 6,
        "expected accessor/layout lowering surface to publish six property attribute profiles",
    )
    expect(
        accessor_layout_surface.get("accessor_ownership_profile_entries") == 6,
        "expected accessor/layout lowering surface to publish six accessor ownership profiles",
    )
    expect(
        accessor_layout_surface.get("synthesized_binding_entries") == 6,
        "expected accessor/layout lowering surface to publish six synthesized binding entries",
    )
    expect(
        accessor_layout_surface.get("ivar_layout_entries") == 3,
        "expected accessor/layout lowering surface to publish three ivar layout entries",
    )
    expect(
        accessor_layout_surface.get("ivar_layout_owner_entries") == 1,
        "expected accessor/layout lowering surface to publish one ivar layout owner",
    )
    expect(
        accessor_layout_surface.get("descriptor_counts_match_source_graph") is True,
        "expected accessor/layout lowering surface descriptor counts to match the executable source graph",
    )
    return accessor_layout_surface


def assert_ivar_layout_surface(manifest: ManifestSurface) -> ManifestSurface:
    ivar_layout_surface = manifest.get("executable_ivar_layout_emission_surface", {})
    expect(
        isinstance(ivar_layout_surface, dict),
        "expected compile manifest to publish the executable ivar layout emission surface",
    )
    ivar_layout_surface = cast(ManifestSurface, ivar_layout_surface)
    expected_ivar_layout_fields = {
        "contract_id": "objc3c.executable.ivar.layout.emission.v1",
        "executable_property_accessor_layout_lowering_surface_contract_id": (
            "objc3c.executable.property.accessor.layout.lowering.v1"
        ),
        "descriptor_model": (
            "ivar-descriptor-records-carry-layout-symbol-replay-key-offset-global-slot-offset-size-alignment-padding-inheritance-owner-size-ordering"
        ),
        "offset_global_model": "one-retained-i64-offset-global-per-emitted-ivar-binding",
        "layout_table_model": (
            "declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size"
        ),
        "scope_model": (
            "sema-approved-layout-shape-lowers-into-ivar-section-payloads-without-runtime-allocation"
        ),
        "fail_closed_model": (
            "no-runtime-instance-allocation-no-layout-rederivation-no-accessor-body-synthesis"
        ),
    }
    for field, expected_value in expected_ivar_layout_fields.items():
        expect(
            ivar_layout_surface.get(field) == expected_value,
            f"expected ivar layout emission surface to preserve {field}",
        )
    expect(
        ivar_layout_surface.get("offset_global_entries") == 3,
        "expected ivar layout emission surface to publish three offset globals",
    )
    expect(
        ivar_layout_surface.get("layout_table_entries") == 1,
        "expected ivar layout emission surface to publish one layout table",
    )
    expect(
        ivar_layout_surface.get("layout_owner_entries") == 1,
        "expected ivar layout emission surface to publish one layout owner",
    )
    expect(
        ivar_layout_surface.get("ivar_descriptor_entries") == 3,
        "expected ivar layout emission surface to publish three ivar descriptors",
    )
    return ivar_layout_surface


def assert_executable_synthesized_accessor_surface(
    manifest: ManifestSurface,
) -> ManifestSurface:
    synthesized_accessor_surface = manifest.get(
        "executable_synthesized_accessor_property_lowering_surface", {}
    )
    expect(
        isinstance(synthesized_accessor_surface, dict),
        "expected compile manifest to publish the synthesized accessor lowering surface",
    )
    synthesized_accessor_surface = cast(ManifestSurface, synthesized_accessor_surface)
    expected_synthesized_accessor_fields = {
        "contract_id": "objc3c.executable.synthesized.accessor.property.lowering.v1",
        "executable_property_accessor_layout_lowering_surface_contract_id": (
            "objc3c.executable.property.accessor.layout.lowering.v1"
        ),
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id": (
            "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        ),
        "source_model": (
            "implementation-owned-properties-synthesize-missing-effective-instance-accessors-into-emitted-method-lists"
        ),
        "storage_model": (
            "synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals"
        ),
        "property_descriptor_model": (
            "property-descriptors-carry-effective-accessor-selectors-binding-symbols-layout-symbols-and-accessor-implementation-pointers"
        ),
        "fail_closed_model": (
            "no-missing-effective-accessor-bindings-no-duplicate-synthesized-owner-identities-no-storage-global-fallbacks"
        ),
    }
    for field, expected_value in expected_synthesized_accessor_fields.items():
        expect(
            synthesized_accessor_surface.get(field) == expected_value,
            f"expected synthesized accessor lowering surface to preserve {field}",
        )
    expect(
        synthesized_accessor_surface.get("implementation_owned_property_entries") == 3,
        "expected synthesized accessor lowering surface to publish three implementation-owned properties",
    )
    expect(
        synthesized_accessor_surface.get("synthesized_getter_entries") == 3,
        "expected synthesized accessor lowering surface to publish three synthesized getters",
    )
    expect(
        synthesized_accessor_surface.get("synthesized_setter_entries") == 3,
        "expected synthesized accessor lowering surface to publish three synthesized setters",
    )
    expect(
        synthesized_accessor_surface.get("synthesized_accessor_entries") == 6,
        "expected synthesized accessor lowering surface to publish six synthesized accessors",
    )
    expect(
        synthesized_accessor_surface.get("property_descriptor_entries") == 6,
        "expected synthesized accessor lowering surface to publish six property descriptors",
    )
    return synthesized_accessor_surface


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
    return lowering_surface


__all__ = [
    "ManifestSurface",
    "assert_accessor_layout_surface",
    "assert_executable_synthesized_accessor_surface",
    "assert_ivar_layout_surface",
    "assert_property_source_surface_links",
    "assert_registration_manifest_layout_counts",
    "assert_synthesized_accessor_codegen_manifest",
]
