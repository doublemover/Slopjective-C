"""Storage reflection runtime acceptance contract ownership."""

from __future__ import annotations


RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.property.ivar.storage.accessor.source.surface.v1"
)
DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID = (
    "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
)
EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_LOWERING_SURFACE_CONTRACT_ID = (
    "objc3c.executable.property.accessor.layout.lowering.v1"
)
EXECUTABLE_IVAR_LAYOUT_EMISSION_SURFACE_CONTRACT_ID = (
    "objc3c.executable.ivar.layout.emission.v1"
)
EXECUTABLE_SYNTHESIZED_ACCESSOR_PROPERTY_LOWERING_SURFACE_CONTRACT_ID = (
    "objc3c.executable.synthesized.accessor.property.lowering.v1"
)
RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.storage.accessor.abi.surface.v1"
)
RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.property.ivar.accessor.reflection.implementation.surface.v1"
)
RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.property.atomicity.synthesis.reflection.source.surface.v1"
)
RUNTIME_CROSS_MODULE_STORAGE_REFLECTION_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.storage.reflection.artifact.preservation.v1"
)

STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE = (
    "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3"
)
STORAGE_REFLECTION_PRESERVATION_CONSUMER_FIXTURE = (
    "tests/tooling/fixtures/native/runtime_packaging_consumer.objc3"
)


__all__ = [name for name in globals() if name.isupper()]
