from __future__ import annotations

from typing import Any, Mapping

from .failures import append_failure
from .markdown import render_markdown_report
from .models import JsonObject
from .snapshot import publication_snapshot


def compare_publication_references(
    *,
    payload: Mapping[str, Any],
    report: Mapping[str, Any],
    markdown_text: str | None,
    markdown_display_path: str,
    markdown_report_path: str,
    failures: list[JsonObject],
) -> None:
    expected_snapshot = publication_snapshot(report)
    if payload.get("published") != expected_snapshot:
        append_failure(
            failures,
            "payload-published-reference-drift",
            "payload.published does not match the durable JSON publication report",
        )

    expected_markdown = render_markdown_report(report)
    if markdown_text is None:
        append_failure(
            failures,
            "markdown-report-missing",
            f"missing markdown publication report: {markdown_display_path}",
        )
    elif markdown_text != expected_markdown:
        append_failure(
            failures,
            "markdown-report-drift",
            "markdown publication report does not match the durable JSON publication report",
            path=markdown_report_path,
        )


def compare_report_contract(
    *,
    payload: Mapping[str, Any],
    report: Mapping[str, Any],
    expected_dependencies: list[JsonObject],
    failures: list[JsonObject],
) -> None:
    if report.get("repository") != payload.get("repository"):
        append_failure(
            failures,
            "repository-mismatch",
            "payload and report repository values differ",
        )
    if report.get("milestoneCount") != len(payload["milestones"]):
        append_failure(
            failures,
            "milestone-count-drift",
            "report milestoneCount does not match payload milestone count",
        )
    if report.get("issueCount") != len(payload["issues"]):
        append_failure(
            failures,
            "issue-count-drift",
            "report issueCount does not match payload issue count",
        )
    if report.get("dependencyCount") != len(payload.get("dependencies", [])):
        append_failure(
            failures,
            "dependency-count-drift",
            "report dependencyCount does not match payload dependency count",
        )

    milestone_ids = {milestone["id"] for milestone in payload["milestones"]}
    issue_ids = {issue["id"] for issue in payload["issues"]}
    if set(report["milestones"]) != milestone_ids:
        append_failure(
            failures,
            "milestone-id-drift",
            "report milestone IDs do not match payload milestone IDs",
        )
    if set(report["issues"]) != issue_ids:
        append_failure(
            failures,
            "issue-id-drift",
            "report issue IDs do not match payload issue IDs",
        )

    milestone_numbers = [record.get("number") for record in report["milestones"].values()]
    issue_numbers = [record.get("number") for record in report["issues"].values()]
    if len(milestone_numbers) != len(set(milestone_numbers)):
        append_failure(
            failures,
            "duplicate-milestone-number",
            "report contains duplicate GitHub milestone numbers",
        )
    if len(issue_numbers) != len(set(issue_numbers)):
        append_failure(
            failures,
            "duplicate-issue-number",
            "report contains duplicate GitHub issue numbers",
        )

    observed_by_pair = {
        (dependency.get("blocked"), dependency.get("blocker")): dependency
        for dependency in report.get("dependencies", [])
    }
    for expected in expected_dependencies:
        key = (expected["blocked"], expected["blocker"])
        observed = observed_by_pair.get(key)
        if observed is None:
            append_failure(
                failures,
                "dependency-missing",
                "report is missing dependency mapping",
                blocked=key[0],
                blocker=key[1],
            )
            continue
        for field in ("blocked_number", "blocker_number", "resolved"):
            if observed.get(field) != expected.get(field):
                append_failure(
                    failures,
                    "dependency-number-drift",
                    f"dependency field {field} drifted",
                    blocked=key[0],
                    blocker=key[1],
                    expected=expected.get(field),
                    observed=observed.get(field),
                )
