"""Storage ownership reflection payload shaping."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.domains.storage_reflection_runtime_ownership_sources import (
    StorageOwnershipReflectionArtifacts,
)

from .catalog import MANIFEST_IMPLEMENTATION_SURFACE_KEY
from .data import StorageOwnershipReflectionFacts


def capture_storage_ownership_reflection_facts(
    payload: dict[str, Any],
    artifacts: StorageOwnershipReflectionArtifacts,
) -> StorageOwnershipReflectionFacts:
    return StorageOwnershipReflectionFacts(
        payload=payload,
        box_entry=payload.get("box_entry", {}),
        implementation_surface=payload.get("implementation_surface", {}),
        manifest_implementation_surface=artifacts.manifest.get(
            MANIFEST_IMPLEMENTATION_SURFACE_KEY,
            {},
        ),
    )


__all__ = ["capture_storage_ownership_reflection_facts"]
