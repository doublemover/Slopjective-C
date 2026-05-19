"""Interop packaging linked-runtime contract surface builder exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.interop_packaging_runtime_surface_live_loading import (
    build_runtime_package_loading_interop_implementation_surface,
)
from objc3c_runtime_acceptance.domains.interop_packaging_runtime_surface_loader_abi import (
    build_runtime_package_loader_bridge_abi_surface,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_package_loader_bridge_abi_surface",
    "build_runtime_package_loading_interop_implementation_surface",
]


def exported_case_names() -> list[str]:
    return list(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
