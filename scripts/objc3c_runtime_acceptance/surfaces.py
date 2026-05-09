"""Surface builder facade for runtime acceptance reports."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from .case_result import CaseResult


def __getattr__(name: str):
    from . import core as _core

    if name.startswith("build_") or name.endswith("_SURFACE_CONTRACT_ID"):
        return getattr(_core, name)
    raise AttributeError(name)


def exported_surface_names() -> list[str]:
    from . import core as _core

    return sorted(
        name
        for name in dir(_core)
        if name.startswith("build_") or name.endswith("_SURFACE_CONTRACT_ID")
    )


def build_claim_boundary(public_runtime_abi_boundary: list[str]) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.runtime.execution.claim.boundary.v1",
        "authoritative_claim_classes": {
            "linked-runtime-probe": {
                "requires_runtime_library_or_emitted_object": True,
                "requires_executable_probe": True,
                "requires_runtime_backed_execution_or_snapshot": True,
            },
            "compile-coupled-inspection": {
                "requires_real_compile": True,
                "requires_compile_output_truthfulness": True,
                "requires_coupled_registration_manifest": True,
            },
        },
        "non_authoritative_inputs": [
            "hand-authored llvm ir without matching compile output",
            "sidecar-only manifests or reports with no coupled object/probe path",
            "non-authoritative test surfaces without a coupled emitted object and runtime probe",
            "comment-only or placeholder-only capability claims",
        ],
        "public_runtime_abi_boundary": public_runtime_abi_boundary,
    }


def build_acceptance_suite_surface(
    results: list[CaseResult],
    report_path: Path,
    *,
    root: Path,
    runtime_acceptance_suite_surface_contract_id: str,
    runtime_state_publication_surface_contract_id: str,
    compile_provenance_contract_id: str,
    compile_output_truthfulness_contract_id: str,
) -> dict[str, Any]:
    compile_coupled_case_ids = [
        result.case_id for result in results if result.fixture is not None
    ]
    linked_runtime_probe_case_ids = [
        result.case_id for result in results if result.claim_class == "linked-runtime-probe"
    ]
    compile_coupled_inspection_case_ids = [
        result.case_id
        for result in results
        if result.claim_class == "compile-coupled-inspection"
    ]
    return {
        "contract_id": runtime_acceptance_suite_surface_contract_id,
        "suite_path": "scripts/check_objc3c_runtime_acceptance.py",
        "report_path": str(report_path.relative_to(root)).replace("\\", "/"),
        "consumes_runtime_state_publication_surface_contract_id": (
            runtime_state_publication_surface_contract_id
        ),
        "authoritative_claim_classes": [
            "linked-runtime-probe",
            "compile-coupled-inspection",
        ],
        "linked_runtime_probe_case_ids": linked_runtime_probe_case_ids,
        "compile_coupled_case_ids": compile_coupled_case_ids,
        "compile_coupled_inspection_case_ids": compile_coupled_inspection_case_ids,
        "compile_output_provenance_contract_id": compile_provenance_contract_id,
        "compile_output_truthfulness_contract_id": compile_output_truthfulness_contract_id,
        "coupled_artifact_requirements": [
            "<emit-prefix>.manifest.json",
            "<emit-prefix>.runtime-registration-manifest.json",
            "<emit-prefix>.compile-provenance.json",
        ],
    }


__all__ = [
    "build_acceptance_suite_surface",
    "build_claim_boundary",
    "exported_surface_names",
]
