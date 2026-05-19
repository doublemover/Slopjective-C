"""Storage-reflection case factories."""

from __future__ import annotations

from objc3c_runtime_acceptance.case_factory_types import CaseFactoryContext
from objc3c_runtime_acceptance.case_factory_types import LabeledCaseFactories
from objc3c_runtime_acceptance.domains.storage_reflection_owner_contracts import (
    STORAGE_REFLECTION_DIRECT_FACTORY_CASE_IDS,
    assert_storage_reflection_direct_factory_case_ids,
)


def build_storage_reflection_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    factories = [
        (
            "storage-ownership-reflection",
            lambda: domains.storage_reflection.check_storage_ownership_reflection_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "property-ivar-ordering-semantics",
            lambda: domains.storage_reflection.check_property_ivar_ordering_semantics_case(
                run_dir
            ),
        ),
        (
            "accessor-storage-lowering-metadata-surface",
            lambda: domains.storage_reflection.check_accessor_storage_lowering_metadata_surface_case(
                run_dir
            ),
        ),
        (
            "property-accessor-layout-lowering",
            lambda: domains.storage_reflection.check_property_accessor_layout_lowering_case(
                run_dir
            ),
        ),
        (
            "property-reflection-accessor-compatibility-diagnostics",
            lambda: domains.storage_reflection.check_property_reflection_accessor_compatibility_diagnostics_case(
                run_dir
            ),
        ),
        (
            "property-synthesis-storage-binding-semantics",
            lambda: domains.storage_reflection.check_property_synthesis_storage_binding_semantics_case(
                run_dir
            ),
        ),
        (
            "storage-legality-semantics",
            lambda: domains.storage_reflection.check_storage_legality_semantics_case(
                run_dir
            ),
        ),
        (
            "synthesized-accessor-codegen",
            lambda: domains.storage_reflection.check_synthesized_accessor_codegen_case(
                run_dir
            ),
        ),
        (
            "synthesized-accessor-runtime",
            lambda: domains.storage_reflection.check_synthesized_accessor_runtime_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "property-layout",
            lambda: domains.storage_reflection.check_property_layout_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "instance-allocation-layout-runtime",
            lambda: domains.storage_reflection.check_instance_allocation_layout_runtime_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "property-execution",
            lambda: domains.storage_reflection.check_property_execution_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "property-reflection",
            lambda: domains.storage_reflection.check_property_reflection_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "property-ivar-invalid-layout-runtime",
            lambda: domains.storage_reflection.check_property_invalid_layout_runtime_case(
                clangxx,
                run_dir,
            ),
        ),
    ]
    assert_storage_reflection_direct_factory_case_ids(
        tuple(case_id for case_id, _ in factories)
    )
    assert_storage_reflection_direct_factory_case_ids(
        STORAGE_REFLECTION_DIRECT_FACTORY_CASE_IDS
    )
    return factories


__all__ = ["build_storage_reflection_case_factories"]
