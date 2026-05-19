"""Object model runtime acceptance contract ownership."""

from __future__ import annotations


RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.object.model.realization.source.surface.v1"
)
RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.realization.lowering.reflection.artifact.surface.v1"
)
RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.dispatch.table.reflection.record.lowering.surface.v1"
)
RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.cross.module.realized.metadata.replay.preservation.surface.v1"
)
RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.object.model.abi.query.surface.v1"
)
RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.realization.lookup.reflection.implementation.surface.v1"
)
RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.reflection.query.surface.v1"
)
RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.realization.lookup.semantics.v1"
)
RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.class.metaclass.protocol.realization.v1"
)
RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.category.attachment.merged.dispatch.surface.v1"
)
RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.reflection.visibility.coherence.diagnostics.surface.v1"
)

REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE = (
    "tests/tooling/runtime/object_model_lookup_reflection_runtime_probe.cpp"
)


__all__ = [name for name in globals() if name.isupper()]
