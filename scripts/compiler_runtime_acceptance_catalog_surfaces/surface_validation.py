"""Validation and classification helpers for catalog surface definitions."""

from __future__ import annotations

from collections.abc import Sequence

from .evidence_fields import (
    AUTHORITATIVE_CASE_IDS_FIELD,
    AUTHORITATIVE_CODE_PATHS_FIELD,
    AUTHORITATIVE_PROBE_PATH_FIELD,
    AUTHORITATIVE_PROBE_PATHS_FIELD,
    AUTHORITATIVE_SOURCE_FIELDS_FIELD,
    SOURCE_CONTRACT_IDS_FIELD,
)
from .model import SurfaceRequirement


PATH_EVIDENCE_FIELD_NAMES = frozenset(
    {
        "cross_module_link_plan_artifact",
        "dashboard_schema_path",
        "report_path",
        "runtime_import_surface_artifact",
        "semantic_surface_paths",
        "suite_path",
        AUTHORITATIVE_CODE_PATHS_FIELD,
        AUTHORITATIVE_PROBE_PATH_FIELD,
        AUTHORITATIVE_PROBE_PATHS_FIELD,
    }
)
CASE_EVIDENCE_FIELD_NAMES = frozenset({AUTHORITATIVE_CASE_IDS_FIELD})
CONTRACT_REFERENCE_FIELD_NAMES = frozenset(
    {
        "block_capture_ownership_contract_id",
        "compile_output_provenance_contract_id",
        "compile_output_truthfulness_contract_id",
        "expansion_lowering_contract_id",
        "runtime_block_arc_runtime_abi_surface_contract_id",
        "semantic_contract_ids",
        SOURCE_CONTRACT_IDS_FIELD,
    }
)
SOURCE_EVIDENCE_FIELD_NAMES = frozenset({AUTHORITATIVE_SOURCE_FIELDS_FIELD})


def classify_required_field(field_name: str) -> str:
    if field_name in PATH_EVIDENCE_FIELD_NAMES:
        return "path-evidence"
    if field_name in CASE_EVIDENCE_FIELD_NAMES:
        return "case-evidence"
    if field_name in CONTRACT_REFERENCE_FIELD_NAMES:
        return "contract-reference"
    if field_name in SOURCE_EVIDENCE_FIELD_NAMES:
        return "source-evidence"
    if field_name.endswith("_symbol") or field_name.endswith("_symbols"):
        return "runtime-symbol"
    if field_name.endswith("_type"):
        return "runtime-type"
    if field_name.endswith("_model") or field_name.endswith("_boundary"):
        return "semantic-model"
    return "payload-field"


def validate_surface_requirements(requirements: Sequence[SurfaceRequirement]) -> None:
    keys: set[str] = set()
    contract_ids: set[str] = set()
    for requirement in requirements:
        if not requirement.key:
            raise ValueError("catalog surface requirement key must be non-empty")
        if requirement.key in keys:
            raise ValueError(f"duplicate catalog surface requirement key: {requirement.key}")
        keys.add(requirement.key)

        if not requirement.contract_id:
            raise ValueError(f"catalog surface {requirement.key} has an empty contract id")
        if requirement.contract_id in contract_ids:
            raise ValueError(f"duplicate catalog surface contract id: {requirement.contract_id}")
        contract_ids.add(requirement.contract_id)

        if len(set(requirement.required_fields)) != len(requirement.required_fields):
            raise ValueError(f"catalog surface {requirement.key} has duplicate required fields")
        for field_name in requirement.required_fields:
            if not field_name:
                raise ValueError(f"catalog surface {requirement.key} has an empty required field")
            classify_required_field(field_name)


def validate_authoritative_child_contracts(
    child_contracts: Sequence[str],
    requirements: Sequence[SurfaceRequirement],
) -> None:
    if len(set(child_contracts)) != len(child_contracts):
        raise ValueError("authoritative child report contracts contain duplicates")

    known_contracts = {requirement.contract_id for requirement in requirements}
    unknown_contracts = [contract_id for contract_id in child_contracts if contract_id not in known_contracts]
    if unknown_contracts:
        raise ValueError(
            "authoritative child report contracts are missing catalog surfaces: "
            + ", ".join(unknown_contracts)
        )


__all__ = [
    "CASE_EVIDENCE_FIELD_NAMES",
    "CONTRACT_REFERENCE_FIELD_NAMES",
    "PATH_EVIDENCE_FIELD_NAMES",
    "SOURCE_EVIDENCE_FIELD_NAMES",
    "classify_required_field",
    "validate_authoritative_child_contracts",
    "validate_surface_requirements",
]
