"""Runtime acceptance suite report surface."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.case_result import CaseResult


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
        result.case_id
        for result in results
        if result.claim_class == "linked-runtime-probe"
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


__all__ = ["build_acceptance_suite_surface"]
