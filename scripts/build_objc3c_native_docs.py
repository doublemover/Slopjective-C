#!/usr/bin/env python3
"""Stable entrypoint for docs/objc3c-native fragment stitching."""

from __future__ import annotations

import sys as _sys
from pathlib import Path as _Path

_SCRIPTS_DIR = _Path(__file__).resolve().parent
if str(_SCRIPTS_DIR) not in _sys.path:
    _sys.path.insert(0, str(_SCRIPTS_DIR))

from objc3c_native_docs_builder import (  # noqa: E402
    FRAGMENT_ORDER,
    LINT_DISABLE_LINE,
    ORDER_LINE_RE,
    OUTPUT_PATH,
    README_PATH,
    REQUIRED_HEADINGS,
    ROOT,
    SRC_DIR,
    ContractCheckResult,
    build_docs,
    build_parser,
    check_contract,
    check_drift,
    find_unknown_fragments,
    main,
    output_digest,
    parse_order_from_readme,
    print_diff_preview,
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


if __name__ == "__main__":
    raise SystemExit(main())
