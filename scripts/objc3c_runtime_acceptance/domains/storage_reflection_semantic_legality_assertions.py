"""Storage/reflection semantic legality assertions."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect

from .storage_reflection_semantic_legality_catalog import STORAGE_LEGALITY_IR_EVIDENCE
from .storage_reflection_semantic_legality_predicates import (
    has_property_atomicity_synthesis_reflection_source_contract,
    has_property_ivar_storage_accessor_source_contract,
    has_storage_legality_ivar_descriptor_count,
    has_storage_legality_property_descriptor_count,
    ll_text_contains_storage_legality_evidence,
    runtime_export_boundary_is_ready,
    runtime_export_counter_is_zero,
)


def expect_registration_manifest_published(registration_manifest_path: Path) -> None:
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"compiled fixture did not publish {registration_manifest_path}"
        )


def expect_storage_legality_positive_evidence(
    registration_manifest: dict[str, Any],
    manifest: dict[str, Any],
    sema_pass_manager_manifest: dict[str, Any],
    ll_text: str,
) -> None:
    expect(
        has_storage_legality_property_descriptor_count(registration_manifest),
        "expected storage legality positive fixture to publish ten property descriptors",
    )
    expect(
        has_storage_legality_ivar_descriptor_count(registration_manifest),
        "expected storage legality positive fixture to publish five ivar descriptors",
    )
    expect(
        has_property_ivar_storage_accessor_source_contract(manifest),
        "expected storage legality positive fixture to publish the property/ivar/storage/accessor source surface",
    )
    expect(
        has_property_atomicity_synthesis_reflection_source_contract(manifest),
        "expected storage legality positive fixture to publish the property atomicity/synthesis/reflection source surface",
    )
    expect(
        runtime_export_boundary_is_ready(sema_pass_manager_manifest),
        "expected storage legality positive fixture to publish a ready runtime export legality boundary",
    )
    expect(
        runtime_export_counter_is_zero(
            sema_pass_manager_manifest,
            "runtime_export_property_attribute_invalid_entries",
        ),
        "expected storage legality positive fixture to publish zero invalid property-attribute entries",
    )
    expect(
        runtime_export_counter_is_zero(
            sema_pass_manager_manifest,
            "runtime_export_property_attribute_contract_violations",
        ),
        "expected storage legality positive fixture to publish zero property contract violations",
    )
    expect(
        runtime_export_counter_is_zero(
            sema_pass_manager_manifest,
            "runtime_export_property_ivar_binding_missing",
        ),
        "expected storage legality positive fixture to publish zero missing property ivar bindings",
    )
    expect(
        runtime_export_counter_is_zero(
            sema_pass_manager_manifest,
            "runtime_export_property_ivar_binding_conflicts",
        ),
        "expected storage legality positive fixture to publish zero conflicting property ivar bindings",
    )
    for needle, label in STORAGE_LEGALITY_IR_EVIDENCE:
        expect(
            ll_text_contains_storage_legality_evidence(ll_text, needle),
            f"expected storage legality positive fixture to publish {label} in LLVM IR",
        )
