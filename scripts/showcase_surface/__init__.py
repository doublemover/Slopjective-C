"""Helper modules for showcase surface validation."""

from __future__ import annotations

from .cli import fail, main, parse_args
from .commands import run
from .fixtures import MODULE_DECL_RE
from .paths import GUIDED_WALKTHROUGH, PORTFOLIO, ROOT, repo_relative
from .validation import (
    GUIDED_WALKTHROUGH_CONTRACT_ID,
    SHOWCASE_SUMMARY_CONTRACT_ID,
    WORKSPACE_CONTRACT_ID,
)

__all__ = [
    "GUIDED_WALKTHROUGH",
    "GUIDED_WALKTHROUGH_CONTRACT_ID",
    "MODULE_DECL_RE",
    "PORTFOLIO",
    "ROOT",
    "SHOWCASE_SUMMARY_CONTRACT_ID",
    "WORKSPACE_CONTRACT_ID",
    "fail",
    "main",
    "parse_args",
    "repo_relative",
    "run",
]
