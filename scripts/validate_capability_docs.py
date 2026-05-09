#!/usr/bin/env python3
"""Stable entrypoint for capability docs and evidence-link validation."""

from __future__ import annotations

from capability_docs_validator.cli import build_parser, main
from capability_docs_validator.constants import (
    BEHAVIOR_MATRIX_COMMAND,
    CANONICAL_MANIFEST_PATH,
    EVIDENCE_DOC,
    MATRIX_DOC,
    MATRIX_PATH,
    SCHEMA_PATH,
    SUPPORT_CLAIM_RE,
)
from capability_docs_validator.docs import _validate_docs_reference_rows
from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.manifest import _manifest_support_claims
from capability_docs_validator.matrix import _require_matrix_shape, _validate_evidence_rows
from capability_docs_validator.support_links import (
    _row_support_claims,
    _validate_support_claim_links,
)
from capability_docs_validator.validation import validate


if __name__ == "__main__":
    raise SystemExit(main())
