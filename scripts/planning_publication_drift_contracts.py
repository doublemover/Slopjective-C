"""Planning publication drift report contracts."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any, Mapping


JsonObject = dict[str, Any]
DRIFT_CONTRACT_ID = "objc3c.planning.github-publication-drift-report.v1"


@dataclass(frozen=True)
class PlanningPublicationDriftPaths:
    payload_path: str
    publication_report_path: str
    markdown_report_path: str


@dataclass(frozen=True)
class PlanningPublicationDriftInputs:
    paths: PlanningPublicationDriftPaths
    payload: Mapping[str, Any]
    report: Mapping[str, Any]
    failures: list[JsonObject]
    live_summary: Mapping[str, Any]


def repo_relative_path(path: Path, root: Path) -> str:
    try:
        return path.relative_to(root).as_posix()
    except ValueError:
        return str(path)


def issue_ref(number: Any) -> str:
    return f"#{number}" if isinstance(number, int) else "-"


def append_failure(
    failures: list[JsonObject],
    code: str,
    detail: str,
    **extra: Any,
) -> None:
    failures.append({"code": code, "detail": detail, **extra})


def publication_snapshot(report: Mapping[str, Any]) -> JsonObject:
    failures = report.get("failures", [])
    failure_count = len(failures) if isinstance(failures, list) else failures
    return {
        "repository": report["repository"],
        "updatedAtUtc": report["updatedAtUtc"],
        "milestoneCount": report["milestoneCount"],
        "issueCount": report["issueCount"],
        "dependencyCount": report["dependencyCount"],
        "failures": failure_count,
        "milestones": report["milestones"],
        "issues": report["issues"],
        "dependencies": report["dependencies"],
    }


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

    milestone_numbers = [
        record.get("number") for record in report["milestones"].values()
    ]
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


def build_drift_report(inputs: PlanningPublicationDriftInputs) -> JsonObject:
    payload = inputs.payload
    report = inputs.report
    return {
        "contract_id": DRIFT_CONTRACT_ID,
        "repository": payload["repository"],
        "source_updated_at_utc": report["updatedAtUtc"],
        "payload_path": inputs.paths.payload_path,
        "publication_report_path": inputs.paths.publication_report_path,
        "markdown_report_path": inputs.paths.markdown_report_path,
        "milestone_count": len(payload["milestones"]),
        "issue_count": len(payload["issues"]),
        "dependency_count": len(payload.get("dependencies", [])),
        "live": inputs.live_summary,
        "failure_count": len(inputs.failures),
        "failures": inputs.failures,
    }


def success_status_line(
    payload: Mapping[str, Any],
    live_summary: Mapping[str, Any],
) -> str:
    return (
        "planning-publication-drift-audit: OK "
        f"milestones={len(payload['milestones'])} issues={len(payload['issues'])} "
        f"dependencies={len(payload.get('dependencies', []))} "
        f"live_issues={live_summary['checked_issues']}"
    )


__all__ = [
    "PlanningPublicationDriftInputs",
    "PlanningPublicationDriftPaths",
    "append_failure",
    "build_drift_report",
    "compare_publication_references",
    "compare_report_contract",
    "publication_snapshot",
    "render_markdown_report",
    "repo_relative_path",
    "success_status_line",
]
