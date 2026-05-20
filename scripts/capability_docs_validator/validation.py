from __future__ import annotations

from objc3c_shared.json_io import (
    JsonSchemaValidationError,
    load_json_object,
    validate_json_schema,
)
from objc3c_tooling.paths import display_path

from capability_docs_validator.constants import (
    CANONICAL_MANIFEST_PATH,
    EVIDENCE_MAP_PATH,
    EVIDENCE_MAP_SCHEMA_PATH,
    MATRIX_PATH,
    SCHEMA_PATH,
)
from capability_docs_validator.docs import _validate_docs_reference_rows
from capability_docs_validator.evidence_map import _validate_evidence_map_projection
from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.matrix import (
    _require_matrix_shape,
    _validate_evidence_rows,
    _validate_object_model_scope,
)
from capability_docs_validator.support_links import _validate_support_claim_links


def validate() -> None:
    matrix = load_json_object(MATRIX_PATH)
    schema = load_json_object(SCHEMA_PATH)
    evidence_map = load_json_object(EVIDENCE_MAP_PATH)
    evidence_map_schema = load_json_object(EVIDENCE_MAP_SCHEMA_PATH)
    try:
        validate_json_schema(matrix, schema, label=display_path(MATRIX_PATH))
        validate_json_schema(evidence_map, evidence_map_schema, label=display_path(EVIDENCE_MAP_PATH))
    except JsonSchemaValidationError as exc:
        raise CapabilityDocsError(str(exc)) from exc
    rows = _require_matrix_shape(matrix)
    _validate_evidence_rows(rows)
    _validate_object_model_scope(rows)
    _validate_evidence_map_projection(rows, evidence_map)
    _validate_support_claim_links(rows, load_json_object(CANONICAL_MANIFEST_PATH))
    _validate_docs_reference_rows(rows)
