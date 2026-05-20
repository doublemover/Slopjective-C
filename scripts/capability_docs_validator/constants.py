from __future__ import annotations

import re

from objc3c_shared.schema_registry import schema_path
from objc3c_tooling.paths import ROOT

CAPABILITY_MATRIX_SCHEMA_ID = "objc3c-capability-matrix-v1"
CAPABILITY_EVIDENCE_MAP_SCHEMA_ID = "objc3c-capability-evidence-map-v1"

MATRIX_PATH = ROOT / "docs" / "support" / "capability_matrix.json"
EVIDENCE_MAP_PATH = ROOT / "docs" / "support" / "evidence_map.json"
SCHEMA_PATH = schema_path(CAPABILITY_MATRIX_SCHEMA_ID)
EVIDENCE_MAP_SCHEMA_PATH = schema_path(CAPABILITY_EVIDENCE_MAP_SCHEMA_ID)
SUPPORT_DOCS_DIR = ROOT / "docs" / "support"
MATRIX_DOC = ROOT / "docs" / "support" / "capability_matrix.md"
EVIDENCE_DOC = ROOT / "docs" / "support" / "evidence_map.md"
CANONICAL_MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
PHASE_OWNER_CONTRACT_PATH = (
    ROOT / "tests" / "conformance" / "hard_cutover_behavior_phase_owner_contracts.json"
)
BEHAVIOR_MATRIX_COMMAND = "npm run objc3c -- test-behavior-matrix"
SUPPORT_CLAIM_RE = re.compile(r"\bobjc3c\.behavior\.[a-z0-9._-]+\b")

CAPABILITY_TRUTH_OWNER = "capability-docs.truth-owner"
CAPABILITY_MATRIX_OWNER = "capability-docs.matrix-owner"
CAPABILITY_EVIDENCE_OWNER = "capability-docs.evidence-owner"
CAPABILITY_MANIFEST_OWNER = "capability-docs.manifest-owner"
CAPABILITY_DOCS_SURFACE_OWNER = "capability-docs.docs-surface-owner"

CAPABILITY_TRUTH_OWNER_SURFACE = "scripts/capability_docs_validator/validation.py"
CAPABILITY_MATRIX_OWNER_SURFACE = "scripts/capability_docs_validator/matrix.py"
CAPABILITY_EVIDENCE_OWNER_SURFACE = "scripts/capability_docs_validator/evidence_map.py"
CAPABILITY_MANIFEST_OWNER_SURFACE = "scripts/capability_docs_validator/manifest.py"
CAPABILITY_DOCS_SURFACE_OWNER_SURFACE = "scripts/capability_docs_validator/docs.py"

CAPABILITY_TRUTH_BLOCKER_METADATA = {
    "blocker_contract": "hard-cutover-capability-truth-fail-closed",
    "blocker_scope": "capability-matrix-manifest-evidence-docs",
    "blocker_owner": CAPABILITY_TRUTH_OWNER,
    "blocker_owner_surface": CAPABILITY_TRUTH_OWNER_SURFACE,
}


def capability_truth_owner_contract() -> dict[str, object]:
    return {
        "truth_owner": {
            "owner_id": CAPABILITY_TRUTH_OWNER,
            "owner_surface": CAPABILITY_TRUTH_OWNER_SURFACE,
        },
        "matrix_owner": {
            "owner_id": CAPABILITY_MATRIX_OWNER,
            "owner_surface": CAPABILITY_MATRIX_OWNER_SURFACE,
            "matrix_path": MATRIX_PATH.relative_to(ROOT).as_posix(),
            "schema_path": SCHEMA_PATH.relative_to(ROOT).as_posix(),
        },
        "evidence_owner": {
            "owner_id": CAPABILITY_EVIDENCE_OWNER,
            "owner_surface": CAPABILITY_EVIDENCE_OWNER_SURFACE,
            "evidence_map_path": EVIDENCE_MAP_PATH.relative_to(ROOT).as_posix(),
            "evidence_map_schema_path": EVIDENCE_MAP_SCHEMA_PATH.relative_to(ROOT).as_posix(),
            "evidence_doc": EVIDENCE_DOC.relative_to(ROOT).as_posix(),
        },
        "manifest_owner": {
            "owner_id": CAPABILITY_MANIFEST_OWNER,
            "owner_surface": CAPABILITY_MANIFEST_OWNER_SURFACE,
            "manifest_path": CANONICAL_MANIFEST_PATH.relative_to(ROOT).as_posix(),
        },
        "docs_surface_owner": {
            "owner_id": CAPABILITY_DOCS_SURFACE_OWNER,
            "owner_surface": CAPABILITY_DOCS_SURFACE_OWNER_SURFACE,
            "matrix_doc": MATRIX_DOC.relative_to(ROOT).as_posix(),
        },
        "blocker_metadata": dict(CAPABILITY_TRUTH_BLOCKER_METADATA),
    }
