"""Object-model summary sections."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_catalog import RuntimeAcceptanceDomains
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.summary_owner_contracts import (
    build_domain_summary_owner_payload,
)


def build_object_model_summary_sections(
    *,
    results: list[CaseResult],
    domains: RuntimeAcceptanceDomains,
) -> dict[str, Any]:
    return {
        "runtime_object_model_summary_owner": build_domain_summary_owner_payload(
            owner_module="summary_object_model_sections",
            domain="object-model",
        ),
        "runtime_object_model_realization_source_surface": (
            domains.object_model.build_runtime_object_model_realization_source_surface(
                results
            )
        ),
        "runtime_realization_lowering_reflection_artifact_surface": (
            domains.object_model.build_runtime_realization_lowering_reflection_artifact_surface(
                results
            )
        ),
        "runtime_dispatch_table_reflection_record_lowering_surface": (
            domains.object_model.build_runtime_dispatch_table_reflection_record_lowering_surface(
                results
            )
        ),
        "runtime_cross_module_realized_metadata_replay_preservation_surface": (
            domains.object_model.build_runtime_cross_module_realized_metadata_replay_preservation_surface(
                results
            )
        ),
        "runtime_object_model_abi_query_surface": (
            domains.object_model.build_runtime_object_model_abi_query_surface(results)
        ),
        "runtime_realization_lookup_reflection_implementation_surface": (
            domains.object_model.build_runtime_realization_lookup_reflection_implementation_surface(
                results
            )
        ),
        "runtime_reflection_query_surface": (
            domains.object_model.build_runtime_reflection_query_surface(results)
        ),
        "runtime_realization_lookup_semantics_surface": (
            domains.object_model.build_runtime_realization_lookup_semantics_surface(
                results
            )
        ),
        "runtime_class_metaclass_protocol_realization_surface": (
            domains.object_model.build_runtime_class_metaclass_protocol_realization_surface(
                results
            )
        ),
        "runtime_category_attachment_merged_dispatch_surface": (
            domains.object_model.build_runtime_category_attachment_merged_dispatch_surface(
                results
            )
        ),
        "runtime_reflection_visibility_coherence_diagnostics_surface": (
            domains.object_model.build_runtime_reflection_visibility_coherence_diagnostics_surface(
                results
            )
        ),
    }


__all__ = ["build_object_model_summary_sections"]
