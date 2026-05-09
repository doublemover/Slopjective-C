"""Interop packaging source-surface acceptance case exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.interop_packaging_source_cross_module import (
    check_cross_module_runtime_package_interop_source_surface_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_source_textual_binary import (
    check_textual_binary_interface_parity_source_surface_case,
)

_EXPORTED_CASE_NAMES = [
    "check_cross_module_runtime_package_interop_source_surface_case",
    "check_textual_binary_interface_parity_source_surface_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
