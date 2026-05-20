"""Error-handling runtime acceptance domain."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.errors_diagnostic_cases import (
    check_bridging_filter_unwind_compatibility_diagnostics_case,
)
from objc3c_runtime_acceptance.domains.errors_async_foreign_boundary_case import (
    check_async_error_foreign_boundary_runtime_trace_case,
)
from objc3c_runtime_acceptance.domains.errors_lowering_cases import (
    check_error_lowering_unwind_bridge_helper_surface_case,
    check_executable_throw_catch_cleanup_lowering_case,
)
from objc3c_runtime_acceptance.domains.errors_replay_cases import (
    check_cross_module_error_metadata_replay_preservation_case,
)
from objc3c_runtime_acceptance.domains.errors_runtime_cases import (
    check_error_runtime_abi_cleanup_case,
    check_live_error_runtime_integration_case,
)
from objc3c_runtime_acceptance.domains.errors_semantic_cases import (
    check_error_propagation_cleanup_semantics_case,
    check_executable_try_throw_do_catch_semantics_case,
)
from objc3c_runtime_acceptance.domains.errors_source_cases import (
    check_catch_filter_finalization_source_case,
    check_error_execution_cleanup_source_case,
)
from objc3c_runtime_acceptance.domains.errors_surfaces import (
    build_runtime_bridging_filter_unwind_diagnostics_surface,
    build_runtime_catch_filter_finalization_source_surface,
    build_runtime_error_execution_cleanup_source_surface,
    build_runtime_error_lowering_unwind_bridge_helper_surface,
    build_runtime_error_propagation_catch_cleanup_runtime_implementation_surface,
    build_runtime_error_propagation_cleanup_semantics_surface,
    build_runtime_error_runtime_abi_cleanup_surface,
)

_EXPORTED_CASE_NAMES = [
    "build_runtime_error_execution_cleanup_source_surface",
    "build_runtime_catch_filter_finalization_source_surface",
    "build_runtime_error_propagation_cleanup_semantics_surface",
    "build_runtime_bridging_filter_unwind_diagnostics_surface",
    "build_runtime_error_lowering_unwind_bridge_helper_surface",
    "build_runtime_error_runtime_abi_cleanup_surface",
    "build_runtime_error_propagation_catch_cleanup_runtime_implementation_surface",
    "check_error_execution_cleanup_source_case",
    "check_catch_filter_finalization_source_case",
    "check_error_propagation_cleanup_semantics_case",
    "check_executable_try_throw_do_catch_semantics_case",
    "check_bridging_filter_unwind_compatibility_diagnostics_case",
    "check_async_error_foreign_boundary_runtime_trace_case",
    "check_error_lowering_unwind_bridge_helper_surface_case",
    "check_executable_throw_catch_cleanup_lowering_case",
    "check_cross_module_error_metadata_replay_preservation_case",
    "check_error_runtime_abi_cleanup_case",
    "check_live_error_runtime_integration_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
