"""Error-handling semantic acceptance case exports."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.errors_semantic_propagation_case import (
    check_error_propagation_cleanup_semantics_case,
)
from objc3c_runtime_acceptance.domains.errors_semantic_try_catch_case import (
    check_executable_try_throw_do_catch_semantics_case,
)


_EXPORTED_CASE_NAMES = [
    "check_error_propagation_cleanup_semantics_case",
    "check_executable_try_throw_do_catch_semantics_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
