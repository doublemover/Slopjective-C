from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SRC_DIR = ROOT / "docs" / "objc3c-native" / "src"
README_PATH = SRC_DIR / "README.md"
OUTPUT_PATH = ROOT / "docs" / "objc3c-native.md"

FRAGMENT_ORDER: tuple[str, ...] = (
    "10-cli.md",
    "20-grammar.md",
    "30-semantics.md",
    "35-runtime-architecture.md",
    "40-diagnostics.md",
    "50-artifacts.md",
    "60-tests.md",
    "library-api.md",
)
REQUIRED_HEADINGS: tuple[str, ...] = (
    "## Ownership Policy",
    "## Canonical Fragment Taxonomy",
    "## Deterministic Stitch Order",
    "## Include Rules",
    "## Contract Validation",
)
ORDER_LINE_RE = re.compile(r"^\d+\.\s+`([^`]+\.md)`\s*$")
LINT_DISABLE_LINE = b"<!-- markdownlint-disable-file MD041 -->"
