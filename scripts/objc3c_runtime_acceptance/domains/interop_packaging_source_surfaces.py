"""Interop packaging source contract surface builder exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.interop_packaging_source_surface_cross_module import (
    build_runtime_cross_module_package_interop_source_surface,
)
from objc3c_runtime_acceptance.domains.interop_packaging_source_surface_textual_binary import (
    build_runtime_textual_binary_interface_parity_source_surface,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_cross_module_package_interop_source_surface",
    "build_runtime_textual_binary_interface_parity_source_surface",
]


def exported_case_names() -> list[str]:
    return list(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
