"""Block and ARC runtime acceptance contract ownership."""

from __future__ import annotations


RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.block.arc.unified.source.surface.v1"
)
RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.ownership.transfer.capture.family.source.surface.v1"
)
RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.block.arc.lowering.helper.surface.v1"
)
RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.block.arc.runtime.abi.surface.v1"
)
RUNTIME_CROSS_MODULE_BLOCK_OWNERSHIP_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.block.ownership.artifact.preservation.v1"
)

BLOCK_OWNERSHIP_PRESERVATION_PROVIDER_FIXTURE = (
    "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3"
)
BLOCK_OWNERSHIP_PRESERVATION_CONSUMER_FIXTURE = (
    "tests/tooling/fixtures/native/runtime_packaging_consumer.objc3"
)
BLOCK_ARC_RUNTIME_ABI_PROBE = (
    "tests/tooling/runtime/block_arc_runtime_abi_probe.cpp"
)


__all__ = [name for name in globals() if name.isupper()]
