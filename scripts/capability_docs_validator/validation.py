from __future__ import annotations

from dataclasses import dataclass
from typing import Any

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
    PHASE_OWNER_CONTRACT_PATH,
    SCHEMA_PATH,
    SUPPORT_CLAIM_RUNNABLE_EVIDENCE_CATALOG_PATH,
    UMBRELLA_READINESS_PATH,
    UMBRELLA_READINESS_SCHEMA_PATH,
)
from capability_docs_validator.conformance import _validate_conformance_manifest_links
from capability_docs_validator.docs import _validate_docs_reference_rows
from capability_docs_validator.evidence_map import _validate_evidence_map_projection
from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.matrix import (
    _require_matrix_shape,
    _validate_evidence_rows,
    _validate_object_model_scope,
)
from capability_docs_validator.rendering import (
    _validate_generated_docs,
    render_support_docs,
)
from capability_docs_validator.runnable_evidence import (
    _validate_support_claim_runnable_evidence_catalog,
)
from capability_docs_validator.support_links import _validate_support_claim_links
from capability_docs_validator.type_protocol_claims import (
    _validate_type_protocol_capability_rows,
)
from capability_docs_validator.umbrella_readiness import validate_umbrella_readiness


@dataclass(frozen=True)
class CapabilityDocsInputs:
    matrix: dict[str, Any]
    evidence_map: dict[str, Any]
    manifest: dict[str, Any]
    phase_owner_contracts: dict[str, Any]
    runnable_evidence_catalog: dict[str, Any]
    umbrella_readiness: dict[str, Any]
    rows: list[dict[str, Any]]


def _load_validated_inputs() -> CapabilityDocsInputs:
    matrix = load_json_object(MATRIX_PATH)
    schema = load_json_object(SCHEMA_PATH)
    evidence_map = load_json_object(EVIDENCE_MAP_PATH)
    evidence_map_schema = load_json_object(EVIDENCE_MAP_SCHEMA_PATH)
    manifest = load_json_object(CANONICAL_MANIFEST_PATH)
    phase_owner_contracts = load_json_object(PHASE_OWNER_CONTRACT_PATH)
    runnable_evidence_catalog = load_json_object(SUPPORT_CLAIM_RUNNABLE_EVIDENCE_CATALOG_PATH)
    umbrella_readiness = load_json_object(UMBRELLA_READINESS_PATH)
    umbrella_readiness_schema = load_json_object(UMBRELLA_READINESS_SCHEMA_PATH)
    try:
        validate_json_schema(matrix, schema, label=display_path(MATRIX_PATH))
        validate_json_schema(evidence_map, evidence_map_schema, label=display_path(EVIDENCE_MAP_PATH))
        validate_json_schema(
            umbrella_readiness,
            umbrella_readiness_schema,
            label=display_path(UMBRELLA_READINESS_PATH),
        )
    except JsonSchemaValidationError as exc:
        raise CapabilityDocsError(str(exc)) from exc
    rows = _require_matrix_shape(matrix)
    _validate_evidence_rows(rows)
    _validate_object_model_scope(rows)
    _validate_evidence_map_projection(rows, evidence_map)
    _validate_support_claim_links(rows, manifest)
    _validate_conformance_manifest_links(manifest, phase_owner_contracts)
    _validate_support_claim_runnable_evidence_catalog(
        rows,
        manifest,
        runnable_evidence_catalog,
    )
    _validate_type_protocol_capability_rows(rows, runnable_evidence_catalog)
    validate_umbrella_readiness(
        umbrella_readiness,
        rows=rows,
        evidence_map=evidence_map,
    )
    return CapabilityDocsInputs(
        matrix=matrix,
        evidence_map=evidence_map,
        manifest=manifest,
        phase_owner_contracts=phase_owner_contracts,
        runnable_evidence_catalog=runnable_evidence_catalog,
        umbrella_readiness=umbrella_readiness,
        rows=rows,
    )


def validate() -> None:
    inputs = _load_validated_inputs()
    _validate_generated_docs(
        render_support_docs(
            matrix=inputs.matrix,
            rows=inputs.rows,
            evidence_map=inputs.evidence_map,
            manifest=inputs.manifest,
            phase_owner_contracts=inputs.phase_owner_contracts,
            umbrella_readiness=inputs.umbrella_readiness,
        )
    )
    _validate_docs_reference_rows(inputs.rows)
