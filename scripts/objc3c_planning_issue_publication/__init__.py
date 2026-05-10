"""Support package for Objective-C 3 planning issue publication."""

from __future__ import annotations

from .cli import main, parse_args
from .constants import DEFAULT_PAYLOAD, DEFAULT_REPORT, ROOT
from .contracts import (
    LabelDefinition,
    PublicationError,
    RELATIONSHIP_END,
    RELATIONSHIP_START,
    apply_relationship_section,
    build_dependency_report,
    build_dry_run_report,
    build_report,
    collect_dependencies,
    collect_label_definitions,
    dependencies_by_issue,
    issue_labels,
    issue_number,
    milestone_by_title,
    render_relationship_section,
    require_dict,
    validate_existing_report,
    validate_payload,
)
from .github import fetch_milestones, gh_json, run_gh
from .json_loading import load_json, write_json
from .paths import assert_not_tmp_source, is_under, repo_path
from .publication import create_or_update_issues, ensure_labels, ensure_milestones

__all__ = [
    "DEFAULT_PAYLOAD",
    "DEFAULT_REPORT",
    "ROOT",
    "LabelDefinition",
    "PublicationError",
    "RELATIONSHIP_END",
    "RELATIONSHIP_START",
    "apply_relationship_section",
    "assert_not_tmp_source",
    "build_dependency_report",
    "build_dry_run_report",
    "build_report",
    "collect_dependencies",
    "collect_label_definitions",
    "create_or_update_issues",
    "dependencies_by_issue",
    "ensure_labels",
    "ensure_milestones",
    "fetch_milestones",
    "gh_json",
    "is_under",
    "issue_labels",
    "issue_number",
    "load_json",
    "main",
    "milestone_by_title",
    "parse_args",
    "render_relationship_section",
    "repo_path",
    "require_dict",
    "run_gh",
    "validate_existing_report",
    "validate_payload",
    "write_json",
]
