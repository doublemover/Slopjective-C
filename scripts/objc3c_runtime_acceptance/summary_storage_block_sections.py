"""Storage-reflection and block ARC summary sections."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.cases import RuntimeAcceptanceDomains
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.summary_owner_contracts import (
    build_domain_summary_owner_payload,
)


def build_storage_block_summary_sections(
    *,
    results: list[CaseResult],
    domains: RuntimeAcceptanceDomains,
) -> dict[str, Any]:
    return {
        "runtime_storage_block_summary_owner": build_domain_summary_owner_payload(
            owner_module="summary_storage_block_sections",
            domain="storage-reflection-and-block-arc",
        ),
        "runtime_block_arc_unified_source_surface": (
            domains.block_arc.build_runtime_block_arc_unified_source_surface(results)
        ),
        "runtime_ownership_transfer_capture_family_source_surface": (
            domains.block_arc.build_runtime_ownership_transfer_capture_family_source_surface(
                results
            )
        ),
        "runtime_block_arc_lowering_helper_surface": (
            domains.block_arc.build_runtime_block_arc_lowering_helper_surface(results)
        ),
        "runtime_block_arc_runtime_abi_surface": (
            domains.block_arc.build_runtime_block_arc_runtime_abi_surface(results)
        ),
        "runtime_property_ivar_storage_accessor_source_surface": (
            domains.storage_reflection.build_runtime_property_ivar_storage_accessor_source_surface(
                results
            )
        ),
        "runtime_property_atomicity_synthesis_reflection_source_surface": (
            domains.storage_reflection.build_runtime_property_atomicity_synthesis_reflection_source_surface(
                results
            )
        ),
        "dispatch_and_synthesized_accessor_lowering_surface": (
            domains.storage_reflection.build_dispatch_and_synthesized_accessor_lowering_surface(
                results
            )
        ),
        "executable_property_accessor_layout_lowering_surface": (
            domains.storage_reflection.build_executable_property_accessor_layout_lowering_surface(
                results
            )
        ),
        "executable_ivar_layout_emission_surface": (
            domains.storage_reflection.build_executable_ivar_layout_emission_surface(
                results
            )
        ),
        "executable_synthesized_accessor_property_lowering_surface": (
            domains.storage_reflection.build_executable_synthesized_accessor_property_lowering_surface(
                results
            )
        ),
    }


__all__ = ["build_storage_block_summary_sections"]
