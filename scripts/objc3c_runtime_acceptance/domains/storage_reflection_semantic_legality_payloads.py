"""Storage/reflection semantic legality evidence and report payloads."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult

from .storage_reflection_owner_contracts import storage_reflection_case_summary
from .storage_reflection_semantic_legality_catalog import (
    STORAGE_LEGALITY_POSITIVE_FIXTURE_LABEL,
    STORAGE_LEGALITY_SEMANTICS_CASE_ID,
)


def storage_legality_summary_payload(
    registration_manifest: dict[str, Any],
    sema_pass_manager_manifest: dict[str, Any],
    storage_negative_results: dict[str, dict[str, Any]],
    negative_batch: dict[str, Any],
) -> dict[str, Any]:
    return {
        "property_descriptor_count": registration_manifest.get(
            "property_descriptor_count"
        ),
        "ivar_descriptor_count": registration_manifest.get("ivar_descriptor_count"),
        "runtime_export_property_attribute_invalid_entries": sema_pass_manager_manifest.get(
            "runtime_export_property_attribute_invalid_entries"
        ),
        "runtime_export_property_attribute_contract_violations": sema_pass_manager_manifest.get(
            "runtime_export_property_attribute_contract_violations"
        ),
        "atomic_negative_diagnostic_count": storage_negative_results[
            "negative-atomic-ownership"
        ]["diagnostic_count"],
        "weak_mismatch_diagnostic_count": storage_negative_results[
            "negative-weak-mismatch"
        ]["diagnostic_count"],
        "unowned_mismatch_diagnostic_count": storage_negative_results[
            "negative-unowned-mismatch"
        ]["diagnostic_count"],
        "scalar_ownership_negative_diagnostic_count": storage_negative_results[
            "negative-scalar-ownership"
        ]["diagnostic_count"],
        "duplicate_getter_negative_diagnostic_count": storage_negative_results[
            "negative-duplicate-getter"
        ]["diagnostic_count"],
        "duplicate_setter_negative_diagnostic_count": storage_negative_results[
            "negative-duplicate-setter"
        ]["diagnostic_count"],
        "readonly_setter_negative_diagnostic_count": storage_negative_results[
            "negative-readonly-setter"
        ]["diagnostic_count"],
        "negative_diagnostics_batch": negative_batch,
    }


def storage_legality_case_result(
    registration_manifest: dict[str, Any],
    sema_pass_manager_manifest: dict[str, Any],
    storage_negative_results: dict[str, dict[str, Any]],
    negative_batch: dict[str, Any],
) -> CaseResult:
    return CaseResult(
        case_id=STORAGE_LEGALITY_SEMANTICS_CASE_ID,
        probe="compile-manifest-and-diagnostics",
        fixture=STORAGE_LEGALITY_POSITIVE_FIXTURE_LABEL,
        claim_class="compile-coupled-inspection",
        passed=True,
        summary=storage_reflection_case_summary(
            STORAGE_LEGALITY_SEMANTICS_CASE_ID,
            storage_legality_summary_payload(
                registration_manifest,
                sema_pass_manager_manifest,
                storage_negative_results,
                negative_batch,
            ),
        ),
    )
