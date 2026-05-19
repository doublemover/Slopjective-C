"""Required-field helpers for runtime acceptance catalog evidence surfaces."""

from __future__ import annotations


SOURCE_CONTRACT_IDS_FIELD = "source_contract_ids"
AUTHORITATIVE_CODE_PATHS_FIELD = "authoritative_code_paths"
AUTHORITATIVE_SOURCE_FIELDS_FIELD = "authoritative_source_fields"
AUTHORITATIVE_CASE_IDS_FIELD = "authoritative_case_ids"
AUTHORITATIVE_PROBE_PATH_FIELD = "authoritative_probe_path"
AUTHORITATIVE_PROBE_PATHS_FIELD = "authoritative_probe_paths"

SOURCE_CODE_AND_CASE_FIELDS = (
    SOURCE_CONTRACT_IDS_FIELD,
    AUTHORITATIVE_CODE_PATHS_FIELD,
    AUTHORITATIVE_CASE_IDS_FIELD,
)
SOURCE_FIELD_AND_CASE_FIELDS = (
    SOURCE_CONTRACT_IDS_FIELD,
    AUTHORITATIVE_SOURCE_FIELDS_FIELD,
    AUTHORITATIVE_CASE_IDS_FIELD,
)


def source_model_case_fields(model_field: str) -> tuple[str, ...]:
    return (SOURCE_CONTRACT_IDS_FIELD, model_field, AUTHORITATIVE_CASE_IDS_FIELD)


def case_evidence_fields(*evidence_fields: str) -> tuple[str, ...]:
    return (*evidence_fields, AUTHORITATIVE_CASE_IDS_FIELD)


def probe_path_fields(*evidence_fields: str) -> tuple[str, ...]:
    return (*evidence_fields, AUTHORITATIVE_PROBE_PATHS_FIELD)


__all__ = [
    "AUTHORITATIVE_CASE_IDS_FIELD",
    "AUTHORITATIVE_CODE_PATHS_FIELD",
    "AUTHORITATIVE_PROBE_PATH_FIELD",
    "AUTHORITATIVE_PROBE_PATHS_FIELD",
    "AUTHORITATIVE_SOURCE_FIELDS_FIELD",
    "SOURCE_CODE_AND_CASE_FIELDS",
    "SOURCE_CONTRACT_IDS_FIELD",
    "SOURCE_FIELD_AND_CASE_FIELDS",
    "case_evidence_fields",
    "probe_path_fields",
    "source_model_case_fields",
]
