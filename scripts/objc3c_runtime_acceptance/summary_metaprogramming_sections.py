"""Metaprogramming summary sections."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_catalog import RuntimeAcceptanceDomains
from objc3c_runtime_acceptance.case_result import CaseResult


def build_metaprogramming_summary_sections(
    *,
    results: list[CaseResult],
    domains: RuntimeAcceptanceDomains,
) -> dict[str, Any]:
    return {
        "runtime_metaprogramming_source_surface": (
            domains.metaprogramming.build_runtime_metaprogramming_source_surface(results)
        ),
        "runtime_metaprogramming_package_provenance_source_surface": (
            domains.metaprogramming.build_runtime_metaprogramming_package_provenance_source_surface(
                results
            )
        ),
        "runtime_metaprogramming_semantics_surface": (
            domains.metaprogramming.build_runtime_metaprogramming_semantics_surface(
                results
            )
        ),
        "runtime_metaprogramming_lowering_host_cache_surface": (
            domains.metaprogramming.build_runtime_metaprogramming_lowering_host_cache_surface(
                results
            )
        ),
        "runtime_cross_module_metaprogramming_artifact_preservation_surface": (
            domains.metaprogramming.build_runtime_cross_module_metaprogramming_artifact_preservation_surface(
                results
            )
        ),
        "runtime_metaprogramming_runtime_abi_cache_surface": (
            domains.metaprogramming.build_runtime_metaprogramming_runtime_abi_cache_surface(
                results
            )
        ),
        "runtime_metaprogramming_cache_runtime_integration_implementation_surface": (
            domains.metaprogramming.build_runtime_metaprogramming_cache_runtime_integration_implementation_surface(
                results
            )
        ),
    }


__all__ = ["build_metaprogramming_summary_sections"]
