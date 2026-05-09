from __future__ import annotations

import re

from objc3c_tooling.paths import ROOT

MATRIX_PATH = ROOT / "docs" / "support" / "capability_matrix.json"
SCHEMA_PATH = ROOT / "docs" / "support" / "capability_matrix.schema.json"
MATRIX_DOC = ROOT / "docs" / "support" / "capability_matrix.md"
EVIDENCE_DOC = ROOT / "docs" / "support" / "evidence_map.md"
CANONICAL_MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
BEHAVIOR_MATRIX_COMMAND = "npm run objc3c -- test-behavior-matrix"
SUPPORT_CLAIM_RE = re.compile(r"\bobjc3c\.behavior\.[a-z0-9._-]+\b")
