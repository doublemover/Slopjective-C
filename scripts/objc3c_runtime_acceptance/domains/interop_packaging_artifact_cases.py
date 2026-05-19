"""Interop packaging artifact-surface acceptance case exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.interop_packaging_artifact_bridge_loader import (
    check_runtime_packaging_bridge_loader_artifact_surface_case,
)

_EXPORTED_CASE_NAMES = [
    "check_runtime_packaging_bridge_loader_artifact_surface_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
