"""Executable property accessor/layout lowering surface builder."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.storage_reflection_lowering_surface_support import (
    authoritative_case_ids,
)
from objc3c_runtime_acceptance.domains.storage_reflection_owner_contracts import (
    storage_reflection_surface_owner_payload,
)

from ..runtime_contract_storage_reflection import (
    DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID,
    EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
)


def build_executable_property_accessor_layout_lowering_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    return {
        "contract_id": EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_LOWERING_SURFACE_CONTRACT_ID,
        "owner_contract": storage_reflection_surface_owner_payload(),
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
            DISPATCH_AND_SYNTHESIZED_ACCESSOR_LOWERING_SURFACE_CONTRACT_ID,
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
        ],
        "property_table_model": (
            "property-descriptor-bundles-carry-sema-approved-attribute-accessor-binding-and-layout-records"
        ),
        "ivar_layout_model": (
            "ivar-descriptor-bundles-carry-sema-approved-layout-symbol-replay-key-slot-offset-size-alignment-padding-inheritance-owner-size-records"
        ),
        "accessor_binding_model": (
            "effective-accessor-selectors-and-synthesized-binding-identities-pass-through-lowering-without-body-synthesis"
        ),
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {
                "property-accessor-layout-lowering",
                "property-synthesis-storage-binding-semantics",
                "property-layout",
                "instance-allocation-layout-runtime",
                "property-execution",
                "storage-ownership-reflection",
                "arc-property-helper-abi",
            },
        ),
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/instance_allocation_runtime_positive.objc3",
            "tests/tooling/fixtures/native/property_synthesis_default_ivar_binding_no_redeclaration.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
            "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
            "tests/tooling/fixtures/native/arc_property_interaction_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/synthesized_accessor_probe.cpp",
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
            "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-layout-or-accessor-body-rederivation-outside-the-live-lowering-path",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


__all__ = ["build_executable_property_accessor_layout_lowering_surface"]
