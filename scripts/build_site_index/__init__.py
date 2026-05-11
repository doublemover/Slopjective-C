"""Builder package for generated site/index.md."""

from __future__ import annotations

from .cli import build_index, build_parser, check_drift, main
from .config import load_contract_config
from .constants import (
    ALLOWED_SRC_FILES,
    CONFIG_PATH,
    POLICY_README_PATH,
    REQUIRED_POLICY_TOKENS,
    ROOT,
    SRC_DIR,
)
from .models import ContractConfig
from .rendering import digest, format_diff, render_expected, render_markdown_source
from .validation import (
    find_unknown_src_files,
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
