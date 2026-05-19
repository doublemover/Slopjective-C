"""Error handling runtime acceptance contract ownership."""

from __future__ import annotations


RUNTIME_ERROR_EXECUTION_CLEANUP_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.error.execution.cleanup.source.surface.v1"
)
RUNTIME_CATCH_FILTER_FINALIZATION_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.catch.filter.finalization.source.surface.v1"
)
RUNTIME_ERROR_PROPAGATION_CLEANUP_SEMANTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.error.propagation.cleanup.semantics.surface.v1"
)
RUNTIME_BRIDGING_FILTER_UNWIND_DIAGNOSTICS_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.bridging.filter.unwind.diagnostics.surface.v1"
)
RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.error.lowering.unwind.bridge.helper.surface.v1"
)
RUNTIME_CROSS_MODULE_ERROR_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.cross.module.error.metadata.replay.preservation.surface.v1"
)
RUNTIME_ERROR_RUNTIME_ABI_CLEANUP_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.error.runtime.abi.cleanup.surface.v1"
)
RUNTIME_ERROR_PROPAGATION_CATCH_CLEANUP_RUNTIME_IMPLEMENTATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.error.propagation.catch.cleanup.runtime.implementation.surface.v1"
)


__all__ = [name for name in globals() if name.isupper()]
