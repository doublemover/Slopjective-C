"""Builder package for the Objective-C 3 native implementation docs."""

from __future__ import annotations

from .cli import build_docs, build_parser, check_contract, check_drift, main
from .constants import (
    FRAGMENT_ORDER,
    LINT_DISABLE_LINE,
    ORDER_LINE_RE,
    OUTPUT_PATH,
    README_PATH,
    REQUIRED_HEADINGS,
    ROOT,
    SRC_DIR,
)
from .models import ContractCheckResult
from .output_writing import output_digest
from .section_rendering import print_diff_preview
from .source_loading import (
    find_unknown_fragments,
    parse_order_from_readme,
    read_fragment_bytes,
    required_fragment_paths,
    stitch_fragments,
    validate_readme_contract,
    validate_source_contract,
)

__all__ = [
    "FRAGMENT_ORDER",
    "LINT_DISABLE_LINE",
    "ORDER_LINE_RE",
    "OUTPUT_PATH",
    "README_PATH",
    "REQUIRED_HEADINGS",
    "ROOT",
    "SRC_DIR",
    "ContractCheckResult",
    "build_docs",
    "build_parser",
    "check_contract",
    "check_drift",
    "find_unknown_fragments",
    "main",
    "output_digest",
    "parse_order_from_readme",
    "print_diff_preview",
    "read_fragment_bytes",
    "required_fragment_paths",
    "stitch_fragments",
    "validate_readme_contract",
    "validate_source_contract",
]
