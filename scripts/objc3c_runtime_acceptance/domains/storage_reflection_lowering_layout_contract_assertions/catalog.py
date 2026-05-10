"""Storage/reflection layout lowering contract assertion catalog."""

from __future__ import annotations

from .data import SurfaceValueExpectation


PROPERTY_SOURCE_SURFACE_KEY = "runtime_property_ivar_storage_accessor_source_surface"
ACCESSOR_LAYOUT_SURFACE_KEY = "executable_property_accessor_layout_lowering_surface"
IVAR_LAYOUT_SURFACE_KEY = "executable_ivar_layout_emission_surface"
SYNTHESIZED_ACCESSOR_SURFACE_KEY = (
    "executable_synthesized_accessor_property_lowering_surface"
)

ACCESSOR_LAYOUT_SURFACE_TYPE_MESSAGE = (
    "expected compile manifest to publish the executable accessor/layout lowering surface"
)
IVAR_LAYOUT_SURFACE_TYPE_MESSAGE = (
    "expected compile manifest to publish the executable ivar layout emission surface"
)
SYNTHESIZED_ACCESSOR_SURFACE_TYPE_MESSAGE = (
    "expected compile manifest to publish the synthesized accessor lowering surface"
)

PROPERTY_SOURCE_LINK_EXPECTATIONS = (
    SurfaceValueExpectation(
        "executable_property_accessor_layout_lowering_contract_id",
        "objc3c.executable.property.accessor.layout.lowering.v1",
        "expected property/ivar storage source surface to point at the executable accessor/layout lowering surface",
    ),
    SurfaceValueExpectation(
        "executable_ivar_layout_emission_contract_id",
        "objc3c.executable.ivar.layout.emission.v1",
        "expected property/ivar storage source surface to point at the executable ivar layout emission surface",
    ),
    SurfaceValueExpectation(
        "executable_synthesized_accessor_property_lowering_contract_id",
        "objc3c.executable.synthesized.accessor.property.lowering.v1",
        "expected property/ivar storage source surface to point at the synthesized accessor lowering surface",
    ),
)

ACCESSOR_LAYOUT_FIELD_EXPECTATIONS = (
    SurfaceValueExpectation(
        "contract_id",
        "objc3c.executable.property.accessor.layout.lowering.v1",
        "expected accessor/layout lowering surface to preserve contract_id",
    ),
    SurfaceValueExpectation(
        "runtime_property_ivar_storage_accessor_source_surface_contract_id",
        "objc3c.runtime.property.ivar.storage.accessor.source.surface.v1",
        "expected accessor/layout lowering surface to preserve runtime_property_ivar_storage_accessor_source_surface_contract_id",
    ),
    SurfaceValueExpectation(
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id",
        "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
        "expected accessor/layout lowering surface to preserve dispatch_and_synthesized_accessor_lowering_surface_contract_id",
    ),
    SurfaceValueExpectation(
        "property_table_model",
        "property-descriptor-bundles-carry-sema-approved-attribute-accessor-binding-and-layout-records",
        "expected accessor/layout lowering surface to preserve property_table_model",
    ),
    SurfaceValueExpectation(
        "ivar_layout_model",
        "ivar-descriptor-bundles-carry-sema-approved-layout-symbol-replay-key-slot-offset-size-alignment-padding-inheritance-owner-size-records",
        "expected accessor/layout lowering surface to preserve ivar_layout_model",
    ),
    SurfaceValueExpectation(
        "accessor_binding_model",
        "effective-accessor-selectors-and-synthesized-binding-identities-pass-through-lowering-without-body-synthesis",
        "expected accessor/layout lowering surface to preserve accessor_binding_model",
    ),
    SurfaceValueExpectation(
        "scope_model",
        "ast-sema-property-layout-handoff-ir-object-metadata-publication",
        "expected accessor/layout lowering surface to preserve scope_model",
    ),
    SurfaceValueExpectation(
        "fail_closed_model",
        "no-synthesized-accessor-bodies-no-runtime-storage-allocation-no-layout-rederivation",
        "expected accessor/layout lowering surface to preserve fail_closed_model",
    ),
    SurfaceValueExpectation(
        "compile_manifest_artifact",
        "module.manifest.json",
        "expected accessor/layout lowering surface to preserve compile_manifest_artifact",
    ),
    SurfaceValueExpectation(
        "registration_manifest_artifact",
        "module.runtime-registration-manifest.json",
        "expected accessor/layout lowering surface to preserve registration_manifest_artifact",
    ),
    SurfaceValueExpectation(
        "object_artifact",
        "module.obj",
        "expected accessor/layout lowering surface to preserve object_artifact",
    ),
    SurfaceValueExpectation(
        "backend_artifact",
        "module.ll",
        "expected accessor/layout lowering surface to preserve backend_artifact",
    ),
)

IVAR_LAYOUT_FIELD_EXPECTATIONS = (
    SurfaceValueExpectation(
        "contract_id",
        "objc3c.executable.ivar.layout.emission.v1",
        "expected ivar layout emission surface to preserve contract_id",
    ),
    SurfaceValueExpectation(
        "executable_property_accessor_layout_lowering_surface_contract_id",
        "objc3c.executable.property.accessor.layout.lowering.v1",
        "expected ivar layout emission surface to preserve executable_property_accessor_layout_lowering_surface_contract_id",
    ),
    SurfaceValueExpectation(
        "descriptor_model",
        "ivar-descriptor-records-carry-layout-symbol-replay-key-offset-global-slot-offset-size-alignment-padding-inheritance-owner-size-ordering",
        "expected ivar layout emission surface to preserve descriptor_model",
    ),
    SurfaceValueExpectation(
        "offset_global_model",
        "one-retained-i64-offset-global-per-emitted-ivar-binding",
        "expected ivar layout emission surface to preserve offset_global_model",
    ),
    SurfaceValueExpectation(
        "layout_table_model",
        "declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size",
        "expected ivar layout emission surface to preserve layout_table_model",
    ),
    SurfaceValueExpectation(
        "scope_model",
        "sema-approved-layout-shape-lowers-into-ivar-section-payloads-without-runtime-allocation",
        "expected ivar layout emission surface to preserve scope_model",
    ),
    SurfaceValueExpectation(
        "fail_closed_model",
        "no-runtime-instance-allocation-no-layout-rederivation-no-accessor-body-synthesis",
        "expected ivar layout emission surface to preserve fail_closed_model",
    ),
)

SYNTHESIZED_ACCESSOR_FIELD_EXPECTATIONS = (
    SurfaceValueExpectation(
        "contract_id",
        "objc3c.executable.synthesized.accessor.property.lowering.v1",
        "expected synthesized accessor lowering surface to preserve contract_id",
    ),
    SurfaceValueExpectation(
        "executable_property_accessor_layout_lowering_surface_contract_id",
        "objc3c.executable.property.accessor.layout.lowering.v1",
        "expected synthesized accessor lowering surface to preserve executable_property_accessor_layout_lowering_surface_contract_id",
    ),
    SurfaceValueExpectation(
        "dispatch_and_synthesized_accessor_lowering_surface_contract_id",
        "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1",
        "expected synthesized accessor lowering surface to preserve dispatch_and_synthesized_accessor_lowering_surface_contract_id",
    ),
    SurfaceValueExpectation(
        "source_model",
        "implementation-owned-properties-synthesize-missing-effective-instance-accessors-into-emitted-method-lists",
        "expected synthesized accessor lowering surface to preserve source_model",
    ),
    SurfaceValueExpectation(
        "storage_model",
        "synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals",
        "expected synthesized accessor lowering surface to preserve storage_model",
    ),
    SurfaceValueExpectation(
        "property_descriptor_model",
        "property-descriptors-carry-effective-accessor-selectors-binding-symbols-layout-symbols-and-accessor-implementation-pointers",
        "expected synthesized accessor lowering surface to preserve property_descriptor_model",
    ),
    SurfaceValueExpectation(
        "fail_closed_model",
        "no-missing-effective-accessor-bindings-no-duplicate-synthesized-owner-identities-no-storage-global-retired-routes",
        "expected synthesized accessor lowering surface to preserve fail_closed_model",
    ),
)

ACCESSOR_LAYOUT_COUNT_EXPECTATIONS = (
    SurfaceValueExpectation(
        "property_metadata_entries",
        6,
        "expected accessor/layout lowering surface to publish six property metadata entries",
    ),
    SurfaceValueExpectation(
        "ivar_metadata_entries",
        3,
        "expected accessor/layout lowering surface to publish three ivar metadata entries",
    ),
    SurfaceValueExpectation(
        "property_descriptor_entries",
        6,
        "expected accessor/layout lowering surface to publish six property descriptors",
    ),
    SurfaceValueExpectation(
        "ivar_descriptor_entries",
        3,
        "expected accessor/layout lowering surface to publish three ivar descriptors",
    ),
    SurfaceValueExpectation(
        "property_attribute_profile_entries",
        6,
        "expected accessor/layout lowering surface to publish six property attribute profiles",
    ),
    SurfaceValueExpectation(
        "accessor_ownership_profile_entries",
        6,
        "expected accessor/layout lowering surface to publish six accessor ownership profiles",
    ),
    SurfaceValueExpectation(
        "synthesized_binding_entries",
        6,
        "expected accessor/layout lowering surface to publish six synthesized binding entries",
    ),
    SurfaceValueExpectation(
        "ivar_layout_entries",
        3,
        "expected accessor/layout lowering surface to publish three ivar layout entries",
    ),
    SurfaceValueExpectation(
        "ivar_layout_owner_entries",
        1,
        "expected accessor/layout lowering surface to publish one ivar layout owner",
    ),
    SurfaceValueExpectation(
        "descriptor_counts_match_source_graph",
        True,
        "expected accessor/layout lowering surface descriptor counts to match the executable source graph",
        identity=True,
    ),
)

IVAR_LAYOUT_COUNT_EXPECTATIONS = (
    SurfaceValueExpectation(
        "offset_global_entries",
        3,
        "expected ivar layout emission surface to publish three offset globals",
    ),
    SurfaceValueExpectation(
        "layout_table_entries",
        1,
        "expected ivar layout emission surface to publish one layout table",
    ),
    SurfaceValueExpectation(
        "layout_owner_entries",
        1,
        "expected ivar layout emission surface to publish one layout owner",
    ),
    SurfaceValueExpectation(
        "ivar_descriptor_entries",
        3,
        "expected ivar layout emission surface to publish three ivar descriptors",
    ),
)

SYNTHESIZED_ACCESSOR_COUNT_EXPECTATIONS = (
    SurfaceValueExpectation(
        "implementation_owned_property_entries",
        3,
        "expected synthesized accessor lowering surface to publish three implementation-owned properties",
    ),
    SurfaceValueExpectation(
        "synthesized_getter_entries",
        3,
        "expected synthesized accessor lowering surface to publish three synthesized getters",
    ),
    SurfaceValueExpectation(
        "synthesized_setter_entries",
        3,
        "expected synthesized accessor lowering surface to publish three synthesized setters",
    ),
    SurfaceValueExpectation(
        "synthesized_accessor_entries",
        6,
        "expected synthesized accessor lowering surface to publish six synthesized accessors",
    ),
    SurfaceValueExpectation(
        "property_descriptor_entries",
        6,
        "expected synthesized accessor lowering surface to publish six property descriptors",
    ),
)
