#!/usr/bin/env python3
"""Validate the live showcase example surface through the public compiler path."""

from __future__ import annotations

from showcase_surface import (
    GUIDED_WALKTHROUGH,
    GUIDED_WALKTHROUGH_CONTRACT_ID,
    MODULE_DECL_RE,
    PORTFOLIO,
    ROOT,
    SHOWCASE_SUMMARY_CONTRACT_ID,
    WORKSPACE_CONTRACT_ID,
    fail,
    main,
    parse_args,
    repo_relative,
    run,
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


if __name__ == "__main__":
    raise SystemExit(main())
