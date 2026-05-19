"""Error-runtime summary sections."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.cases import RuntimeAcceptanceDomains
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.summary_owner_contracts import (
    build_domain_summary_owner_payload,
)


def build_error_summary_sections(
    *,
    results: list[CaseResult],
    domains: RuntimeAcceptanceDomains,
) -> dict[str, Any]:
    return {
        "runtime_error_summary_owner": build_domain_summary_owner_payload(
            owner_module="summary_error_sections",
            domain="errors",
        ),
        "runtime_error_execution_cleanup_source_surface": (
            domains.errors.build_runtime_error_execution_cleanup_source_surface(results)
        ),
        "runtime_catch_filter_finalization_source_surface": (
            domains.errors.build_runtime_catch_filter_finalization_source_surface(
                results
            )
        ),
        "runtime_error_propagation_cleanup_semantics_surface": (
            domains.errors.build_runtime_error_propagation_cleanup_semantics_surface(
                results
            )
        ),
        "runtime_bridging_filter_unwind_diagnostics_surface": (
            domains.errors.build_runtime_bridging_filter_unwind_diagnostics_surface(
                results
            )
        ),
        "runtime_error_lowering_unwind_bridge_helper_surface": (
            domains.errors.build_runtime_error_lowering_unwind_bridge_helper_surface(
                results
            )
        ),
        "runtime_error_runtime_abi_cleanup_surface": (
            domains.errors.build_runtime_error_runtime_abi_cleanup_surface(results)
        ),
        "runtime_error_propagation_catch_cleanup_runtime_implementation_surface": (
            domains.errors.build_runtime_error_propagation_catch_cleanup_runtime_implementation_surface(
                results
            )
        ),
    }


__all__ = ["build_error_summary_sections"]
