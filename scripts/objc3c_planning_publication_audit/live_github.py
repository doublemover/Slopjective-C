"""Live GitHub comparison for durable planning publication references."""

from __future__ import annotations

from typing import Any

from .shared import append_failure, publisher, require_dict


def expected_issue_labels(issue: dict[str, Any], issue_id: str, payload: dict[str, Any]) -> set[str]:
    return set(publisher.issue_labels(issue, issue_id, payload))


def fetch_live_issue(repo: str, number: int) -> dict[str, Any]:
    return require_dict(publisher.gh_json(["api", f"repos/{repo}/issues/{number}"]), f"issue #{number}")


def compare_live_github(
    payload: dict[str, Any],
    report: dict[str, Any],
    failures: list[dict[str, Any]],
    limit_live: int | None,
) -> dict[str, Any]:
    repo = payload["repository"]
    live_milestones = publisher.fetch_milestones(repo)
    checked_milestones = 0
    checked_issues = 0

    for draft_id, milestone in report["milestones"].items():
        live = live_milestones.get(milestone["title"])
        checked_milestones += 1
        if live is None:
            append_failure(failures, "live-milestone-missing", "GitHub milestone title is missing", draft_id=draft_id)
            continue
        if live.get("number") != milestone.get("number"):
            append_failure(
                failures,
                "live-milestone-number-drift",
                "GitHub milestone number differs from durable report",
                draft_id=draft_id,
                expected=milestone.get("number"),
                observed=live.get("number"),
            )

    issues_by_id = {issue["id"]: issue for issue in payload["issues"]}
    title_to_milestone = publisher.milestone_by_title(payload)
    forward_dependencies, reverse_dependencies = publisher.dependencies_by_issue(payload)
    for index, (draft_id, report_issue) in enumerate(report["issues"].items()):
        if limit_live is not None and index >= limit_live:
            break
        issue_number = report_issue.get("number")
        if not isinstance(issue_number, int):
            append_failure(failures, "issue-number-missing", "report issue number is missing", draft_id=draft_id)
            continue
        live_issue = fetch_live_issue(repo, issue_number)
        checked_issues += 1
        payload_issue = issues_by_id[draft_id]
        if live_issue.get("title") != payload_issue["title"]:
            append_failure(failures, "live-issue-title-drift", "GitHub issue title differs from payload", draft_id=draft_id)
        live_milestone = live_issue.get("milestone") or {}
        expected_milestone_id = title_to_milestone[payload_issue["milestoneTitle"]]["id"]
        expected_milestone_number = report["milestones"][expected_milestone_id]["number"]
        if live_milestone.get("number") != expected_milestone_number:
            append_failure(failures, "live-issue-milestone-drift", "GitHub issue milestone differs from report", draft_id=draft_id)
        live_labels = {label.get("name") for label in live_issue.get("labels", []) if isinstance(label, dict)}
        missing_labels = sorted(expected_issue_labels(payload_issue, draft_id, payload) - live_labels)
        if missing_labels:
            append_failure(
                failures,
                "live-issue-label-drift",
                "GitHub issue is missing expected planning labels",
                draft_id=draft_id,
                missing_labels=missing_labels,
            )
        body = live_issue.get("body") or ""
        if f"<!-- draft-id: {draft_id} -->" not in body:
            append_failure(failures, "live-issue-draft-marker-missing", "GitHub issue body lost its draft-id marker", draft_id=draft_id)
        has_relationship = draft_id in forward_dependencies or draft_id in reverse_dependencies
        if has_relationship and publisher.RELATIONSHIP_START not in body:
            append_failure(
                failures,
                "live-issue-relationship-block-missing",
                "GitHub issue body is missing the managed relationship block",
                draft_id=draft_id,
            )

    return {"enabled": True, "checked_milestones": checked_milestones, "checked_issues": checked_issues}
