#!/usr/bin/env python3
"""Stable entrypoint for capability docs and evidence-link validation."""

from __future__ import annotations

from capability_docs_validator.cli import build_parser, main
from capability_docs_validator.constants import (
    BEHAVIOR_MATRIX_COMMAND,
    CANONICAL_MANIFEST_PATH,
    CAPABILITY_DOCS_SURFACE_OWNER,
    CAPABILITY_DOCS_SURFACE_OWNER_SURFACE,
    CAPABILITY_EVIDENCE_OWNER,
    CAPABILITY_EVIDENCE_OWNER_SURFACE,
    CAPABILITY_MANIFEST_OWNER,
    CAPABILITY_MANIFEST_OWNER_SURFACE,
    CAPABILITY_MATRIX_OWNER,
    CAPABILITY_MATRIX_OWNER_SURFACE,
    CAPABILITY_TRUTH_BLOCKER_METADATA,
    CAPABILITY_TRUTH_OWNER,
    CAPABILITY_TRUTH_OWNER_SURFACE,
    EVIDENCE_DOC,
    MATRIX_DOC,
    MATRIX_PATH,
    PHASE_OWNER_CONTRACT_PATH,
    SCHEMA_PATH,
    SUPPORT_DOCS_DIR,
    SUPPORT_CLAIM_RUNNABLE_EVIDENCE_CATALOG_PATH,
    SUPPORT_CLAIM_RE,
    capability_truth_owner_contract,
)
from capability_docs_validator.conformance import _validate_conformance_manifest_links
from capability_docs_validator.docs import _validate_docs_reference_rows
from capability_docs_validator.docs import _validate_public_doc_claim_tokens
from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.evidence_map import (
    _validate_evidence_map_projection,
    build_evidence_map_projection,
)
from capability_docs_validator.manifest import _manifest_fixture_paths, _manifest_support_claims
from capability_docs_validator.matrix import (
    _require_matrix_shape,
    _validate_evidence_rows,
    _validate_object_model_scope,
)
from capability_docs_validator.rendering import render_support_docs
from capability_docs_validator.runnable_evidence import _validate_support_claim_runnable_evidence_catalog
from capability_docs_validator.support_links import (
    _row_support_claims,
    _validate_support_claim_links,
)
from capability_docs_validator.type_protocol_claims import (
    _validate_type_protocol_capability_rows,
)
from capability_docs_validator.validation import _load_validated_inputs, validate


if __name__ == "__main__":
    raise SystemExit(main())
