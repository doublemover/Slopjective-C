#!/usr/bin/env python3
"""Facade for the modular open-issue extraction utility."""

from __future__ import annotations

import sys
from pathlib import Path

_REPO_ROOT = Path(__file__).resolve().parents[1]
if str(_REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(_REPO_ROOT))

from scripts.extract_open_issues import (  # noqa: E402
    DEFAULT_SPEC_DIR,
    LEVEL2_HEADING_RE,
    LIST_ITEM_RE,
    NON_ALNUM_RE,
    OPEN_ISSUES_HEADING_RE,
    ROOT,
    ExtractOpenIssuesConfig,
    ExtractionResult,
    GitHubCommand,
    GitHubCommandError,
    GitHubCommandResult,
    GitHubCommandRunner,
    GITHUB_LAUNCH_ERROR_EXIT_CODE,
    GITHUB_MISSING_EXECUTABLE_EXIT_CODE,
    GITHUB_TIMEOUT_EXIT_CODE,
    OpenIssueRecord,
    OpenIssuesSection,
    OutputFormat,
    ParseIssue,
    SubprocessGitHubCommandRunner,
    build_parser,
    collect_open_issue_records,
    collect_open_issues,
    config_from_args,
    extract_open_issues,
    is_none_statement,
    is_part_file,
    iter_part_files,
    main,
    normalize_space,
    parse_open_issue_sections,
    parse_section_items,
    record_sort_key,
    render_json_report,
    render_markdown,
    render_report,
    sort_records,
    write_parse_issues,
)

__all__ = (
    "DEFAULT_SPEC_DIR",
    "LEVEL2_HEADING_RE",
    "LIST_ITEM_RE",
    "NON_ALNUM_RE",
    "OPEN_ISSUES_HEADING_RE",
    "ROOT",
    "ExtractOpenIssuesConfig",
    "ExtractionResult",
    "GitHubCommand",
    "GitHubCommandError",
    "GitHubCommandResult",
    "GitHubCommandRunner",
    "GITHUB_LAUNCH_ERROR_EXIT_CODE",
    "GITHUB_MISSING_EXECUTABLE_EXIT_CODE",
    "GITHUB_TIMEOUT_EXIT_CODE",
    "OpenIssueRecord",
    "OpenIssuesSection",
    "OutputFormat",
    "ParseIssue",
    "SubprocessGitHubCommandRunner",
    "build_parser",
    "collect_open_issue_records",
    "collect_open_issues",
    "config_from_args",
    "extract_open_issues",
    "is_none_statement",
    "is_part_file",
    "iter_part_files",
    "main",
    "normalize_space",
    "parse_open_issue_sections",
    "parse_section_items",
    "record_sort_key",
    "render_json_report",
    "render_markdown",
    "render_report",
    "sort_records",
    "write_parse_issues",
)


if __name__ == "__main__":
    raise SystemExit(main())
