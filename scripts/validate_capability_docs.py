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
    SCHEMA_PATH,
    SUPPORT_CLAIM_RE,
    capability_truth_owner_contract,
)
from capability_docs_validator.docs import _validate_docs_reference_rows
from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.evidence_map import (
    _validate_evidence_map_projection,
    build_evidence_map_projection,
)
from capability_docs_validator.manifest import _manifest_support_claims
from capability_docs_validator.matrix import _require_matrix_shape, _validate_evidence_rows
from capability_docs_validator.support_links import (
    _row_support_claims,
    _validate_support_claim_links,
)
from capability_docs_validator.validation import validate


if __name__ == "__main__":
    raise SystemExit(main())
