"""Error-handling lowering acceptance case exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.errors_lowering_throw_catch_case import (
    check_executable_throw_catch_cleanup_lowering_case,
)
from objc3c_runtime_acceptance.domains.errors_lowering_unwind_bridge_case import (
    check_error_lowering_unwind_bridge_helper_surface_case,
)


_EXPORTED_CASE_NAMES = [
    "check_error_lowering_unwind_bridge_helper_surface_case",
    "check_executable_throw_catch_cleanup_lowering_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
