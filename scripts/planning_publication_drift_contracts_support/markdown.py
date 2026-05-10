from __future__ import annotations

from typing import Any, Mapping


def issue_ref(number: Any) -> str:
    return f"#{number}" if isinstance(number, int) else "-"


def render_markdown_report(report: Mapping[str, Any]) -> str:
    failures = report.get("failures", [])
    failure_count = len(failures) if isinstance(failures, list) else failures
    lines = [
        "# Objective-C 3.0 Publication Report",
        "",
        f"Repository: `{report['repository']}`",
        f"Started: `{report['startedAtUtc']}`",
        f"Updated: `{report['updatedAtUtc']}`",
        "",
        f"Milestones created/reused: {report['milestoneCount']}",
        f"Issues created/reused: {report['issueCount']}",
        f"Dependencies created/recorded: {report['dependencyCount']}",
        f"Failures: {failure_count}",
        "",
        "## Milestones",
        "",
        "| Draft ID | GitHub Number | Title | URL |",
        "| --- | ---: | --- | --- |",
    ]
    for draft_id in sorted(report["milestones"]):
        milestone = report["milestones"][draft_id]
        lines.append(
            f"| {draft_id} | {milestone['number']} | "
            f"{milestone['title']} | {milestone['html_url']} |"
        )

    lines.extend(
        [
            "",
            "## Issues",
            "",
            "| Draft ID | GitHub Issue | Title | URL |",
            "| --- | ---: | --- | --- |",
        ]
    )
    for draft_id in sorted(report["issues"]):
        issue = report["issues"][draft_id]
        lines.append(
            f"| {draft_id} | #{issue['number']} | "
            f"{issue['title']} | {issue['html_url']} |"
        )

    lines.extend(
        [
            "",
            "## Dependencies",
            "",
            "| Blocked Draft ID | Blocked Issue | Blocker Draft ID | Blocker Issue | Status |",
            "| --- | ---: | --- | ---: | --- |",
        ]
    )
    for dependency in report["dependencies"]:
        lines.append(
            f"| {dependency['blocked']} | "
            f"{issue_ref(dependency.get('blocked_number'))} | "
            f"{dependency['blocker']} | "
            f"{issue_ref(dependency.get('blocker_number'))} | "
            f"{dependency.get('status', '-')} |"
        )

    lines.extend(["", "## Failures", ""])
    if isinstance(failures, list) and failures:
        for failure in failures:
            lines.append(f"- {failure}")
    elif failures:
        lines.append(f"- {failures}")
    else:
        lines.append("- none")
    lines.append("")
    return "\n".join(lines)
