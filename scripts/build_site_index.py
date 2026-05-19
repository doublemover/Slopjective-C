#!/usr/bin/env python3
"""Stable entrypoint for generated site/index.md build and drift checks."""

from __future__ import annotations

import sys as _sys
from pathlib import Path as _Path

_SCRIPTS_DIR = _Path(__file__).resolve().parent
if str(_SCRIPTS_DIR) not in _sys.path:
    _sys.path.insert(0, str(_SCRIPTS_DIR))

from build_site_index import (  # noqa: E402
    ALLOWED_SRC_FILES,
    CONFIG_PATH,
    POLICY_README_PATH,
    REQUIRED_POLICY_TOKENS,
    ROOT,
    SRC_DIR,
    ContractConfig,
    build_index,
    build_parser,
    check_drift,
    digest,
    find_unknown_src_files,
    format_diff,
    load_contract_config,
    main,
    render_expected,
    render_markdown_source,
    validate_contract_inputs,
    validate_policy_readme,
)

__all__ = [
    "ALLOWED_SRC_FILES",
    "CONFIG_PATH",
    "POLICY_README_PATH",
    "REQUIRED_POLICY_TOKENS",
    "ROOT",
    "SRC_DIR",
    "ContractConfig",
    "build_index",
    "build_parser",
    "check_drift",
    "digest",
    "find_unknown_src_files",
    "format_diff",
    "load_contract_config",
    "main",
    "render_expected",
    "render_markdown_source",
    "validate_contract_inputs",
    "validate_policy_readme",
]


if __name__ == "__main__":
    raise SystemExit(main())
