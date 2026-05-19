"""Interop packaging semantic acceptance case exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.interop_packaging_semantic_bridge import (
    check_c_cpp_swift_bridge_semantics_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_semantic_mixed_image import (
    check_mixed_image_interop_semantics_case,
)

_EXPORTED_CASE_NAMES = [
    "check_mixed_image_interop_semantics_case",
    "check_c_cpp_swift_bridge_semantics_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
