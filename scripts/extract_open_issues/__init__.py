"""Open-issue extraction package facade."""

from __future__ import annotations

from .cli import build_parser, main, write_parse_issues
from .config import DEFAULT_SPEC_DIR, ROOT, ExtractOpenIssuesConfig, OutputFormat, config_from_args
from .filtering import is_part_file, iter_part_files, record_sort_key, sort_records
from .github import (
    GITHUB_LAUNCH_ERROR_EXIT_CODE,
    GITHUB_MISSING_EXECUTABLE_EXIT_CODE,
    GITHUB_TIMEOUT_EXIT_CODE,
    GitHubCommand,
    GitHubCommandError,
    GitHubCommandResult,
    GitHubCommandRunner,
    SubprocessGitHubCommandRunner,
)
from .models import ExtractionResult, OpenIssueRecord, OpenIssuesSection, ParseIssue
from .orchestration import collect_open_issue_records, collect_open_issues, extract_open_issues
from .parsing import (
    LEVEL2_HEADING_RE,
    LIST_ITEM_RE,
    NON_ALNUM_RE,
    OPEN_ISSUES_HEADING_RE,
    is_none_statement,
    normalize_space,
    parse_open_issue_sections,
    parse_section_items,
)
from .rendering import render_json_report, render_markdown, render_report

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
