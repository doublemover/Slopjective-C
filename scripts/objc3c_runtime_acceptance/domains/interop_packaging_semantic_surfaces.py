"""Interop packaging semantic contract surface builder exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.interop_packaging_semantic_surface_bridge import (
    build_runtime_c_cpp_swift_bridge_semantics_surface,
)
from objc3c_runtime_acceptance.domains.interop_packaging_semantic_surface_mixed_image import (
    build_runtime_mixed_image_interop_semantics_surface,
)
from objc3c_runtime_acceptance.domains.interop_packaging_semantic_surface_module_identity import (
    build_runtime_package_loading_module_identity_semantics_surface,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_mixed_image_interop_semantics_surface",
    "build_runtime_package_loading_module_identity_semantics_surface",
    "build_runtime_c_cpp_swift_bridge_semantics_surface",
]


def exported_case_names() -> list[str]:
    return list(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
