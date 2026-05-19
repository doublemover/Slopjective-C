"""Interop packaging linked-runtime acceptance case exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.interop_packaging_runtime_live_loading import (
    check_live_package_loading_interop_runtime_implementation_case,
)
from objc3c_runtime_acceptance.domains.interop_packaging_runtime_loader_abi import (
    check_runtime_package_loader_bridge_abi_case,
)

_EXPORTED_CASE_NAMES = [
    "check_runtime_package_loader_bridge_abi_case",
    "check_live_package_loading_interop_runtime_implementation_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
