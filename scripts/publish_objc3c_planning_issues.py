#!/usr/bin/env python3
"""Publish checked-in Objective-C 3 planning issues to GitHub.

The publisher intentionally treats GitHub numbers as assigned state. Draft IDs
stay stable, and the durable report records the GitHub milestone/issue numbers
that were actually returned by the API.
"""

from __future__ import annotations

import sys

try:
    from objc3c_planning_issue_publication import (
        DEFAULT_PAYLOAD,
        DEFAULT_REPORT,
        ROOT,
        LabelDefinition,
        PublicationError,
        RELATIONSHIP_END,
        RELATIONSHIP_START,
        apply_relationship_section,
        assert_not_tmp_source,
        build_dependency_report,
        build_dry_run_report,
        build_report,
        collect_dependencies,
        collect_label_definitions,
        create_or_update_issues,
        dependencies_by_issue,
        ensure_labels,
        ensure_milestones,
        fetch_milestones,
        gh_json,
        is_under,
        issue_labels,
        issue_number,
        load_json,
        main,
        milestone_by_title,
        parse_args,
        render_relationship_section,
        repo_path,
        require_dict,
        run_gh,
        validate_existing_report,
        validate_payload,
        write_json,
    )
except ModuleNotFoundError:
    from scripts.objc3c_planning_issue_publication import (
        DEFAULT_PAYLOAD,
        DEFAULT_REPORT,
        ROOT,
        LabelDefinition,
        PublicationError,
        RELATIONSHIP_END,
        RELATIONSHIP_START,
        apply_relationship_section,
        assert_not_tmp_source,
        build_dependency_report,
        build_dry_run_report,
        build_report,
        collect_dependencies,
        collect_label_definitions,
        create_or_update_issues,
        dependencies_by_issue,
        ensure_labels,
        ensure_milestones,
        fetch_milestones,
        gh_json,
        is_under,
        issue_labels,
        issue_number,
        load_json,
        main,
        milestone_by_title,
        parse_args,
        render_relationship_section,
        repo_path,
        require_dict,
        run_gh,
        validate_existing_report,
        validate_payload,
        write_json,
    )

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


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
