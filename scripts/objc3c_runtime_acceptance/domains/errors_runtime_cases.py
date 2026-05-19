"""Error-handling linked-runtime acceptance case exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.errors_runtime_abi_case import (
    check_error_runtime_abi_cleanup_case,
)
from objc3c_runtime_acceptance.domains.errors_runtime_live_case import (
    check_live_error_runtime_integration_case,
)


_EXPORTED_CASE_NAMES = [
    "check_error_runtime_abi_cleanup_case",
    "check_live_error_runtime_integration_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
