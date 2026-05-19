"""Executable ivar layout emission surface builder."""

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
    EXECUTABLE_IVAR_LAYOUT_EMISSION_SURFACE_CONTRACT_ID,
    EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_LOWERING_SURFACE_CONTRACT_ID,
)


def build_executable_ivar_layout_emission_surface(
    results: list[CaseResult],
) -> dict[str, Any]:
    return {
        "contract_id": EXECUTABLE_IVAR_LAYOUT_EMISSION_SURFACE_CONTRACT_ID,
        "owner_contract": storage_reflection_surface_owner_payload(),
        "compile_artifact_set": [
            "<emit-prefix>.obj",
            "<emit-prefix>.ll",
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
        ],
        "source_contract_ids": [
            EXECUTABLE_PROPERTY_ACCESSOR_LAYOUT_LOWERING_SURFACE_CONTRACT_ID,
        ],
        "authoritative_code_paths": [
            "native/objc3c/src/lower/objc3_lowering_contract.h",
            "native/objc3c/src/ir/objc3_ir_emitter.cpp",
            "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp",
        ],
        "descriptor_model": (
            "ivar-descriptor-records-carry-layout-symbol-replay-key-offset-global-slot-offset-size-alignment-padding-inheritance-owner-size-ordering"
        ),
        "offset_global_model": (
            "one-retained-i64-offset-global-per-emitted-ivar-binding"
        ),
        "layout_table_model": (
            "declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size"
        ),
        "authoritative_case_ids": authoritative_case_ids(
            results,
            {
                "property-accessor-layout-lowering",
                "property-ivar-ordering-semantics",
                "property-layout",
                "instance-allocation-layout-runtime",
                "property-execution",
            },
        ),
        "authoritative_fixture_paths": [
            "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
            "tests/tooling/fixtures/native/instance_allocation_runtime_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_source_model_completion_positive.objc3",
            "tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
        ],
        "authoritative_probe_paths": [
            "tests/tooling/runtime/property_layout_runtime_probe.cpp",
            "tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
        ],
        "explicit_non_goals": [
            "no-public-runtime-abi-widening",
            "no-milestone-specific-scaffolding",
            "no-runtime-layout-rederivation",
        ],
        "requires_coupled_registration_manifest": True,
        "requires_real_compile_output": True,
        "requires_linked_runtime_probe": True,
    }


__all__ = ["build_executable_ivar_layout_emission_surface"]
