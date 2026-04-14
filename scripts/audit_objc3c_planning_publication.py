#!/usr/bin/env python3
"""Audit Objective-C 3 planning publication drift and refresh durable references."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.json_io import load_json_any as load_json, write_json_file as write_json

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

import publish_objc3c_planning_issues as publisher  # noqa: E402

DEFAULT_PAYLOAD = ROOT / "reports" / "planning" / "objc3c_3_next_40_github_payloads.json"
DEFAULT_PUBLICATION_REPORT = ROOT / "reports" / "planning" / "objc3c_3_next_40_publication_report.json"
DEFAULT_MARKDOWN_REPORT = ROOT / "reports" / "planning" / "objc3c_3_next_40_publication_report.md"
DEFAULT_DRIFT_REPORT = ROOT / "reports" / "planning" / "objc3c_3_next_40_publication_drift_report.json"
DRIFT_CONTRACT_ID = "objc3c.planning.github-publication-drift-report.v1"


class DriftAuditError(RuntimeError):
    """Raised when audit inputs are malformed."""




def repo_path(path: Path) -> Path:
    return path if path.is_absolute() else ROOT / path


def require_dict(value: Any, name: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise DriftAuditError(f"{name} must be an object")
    return value


def issue_ref(number: Any) -> str:
    return f"#{number}" if isinstance(number, int) else "-"


def publication_snapshot(report: dict[str, Any]) -> dict[str, Any]:
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


def render_markdown_report(report: dict[str, Any]) -> str:
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
            f"| {draft_id} | {milestone['number']} | {milestone['title']} | {milestone['html_url']} |"
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
            f"| {draft_id} | #{issue['number']} | {issue['title']} | {issue['html_url']} |"
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
            f"| {dependency['blocked']} | {issue_ref(dependency.get('blocked_number'))} | "
            f"{dependency['blocker']} | {issue_ref(dependency.get('blocker_number'))} | "
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


def expected_issue_labels(issue: dict[str, Any], issue_id: str, payload: dict[str, Any]) -> set[str]:
    return set(publisher.issue_labels(issue, issue_id, payload))


def append_failure(failures: list[dict[str, Any]], code: str, detail: str, **extra: Any) -> None:
    failures.append({"code": code, "detail": detail, **extra})


def compare_publication_references(
    payload: dict[str, Any],
    report: dict[str, Any],
    markdown_path: Path,
    failures: list[dict[str, Any]],
) -> None:
    expected_snapshot = publication_snapshot(report)
    if payload.get("published") != expected_snapshot:
        append_failure(
            failures,
            "payload-published-reference-drift",
            "payload.published does not match the durable JSON publication report",
        )

    expected_markdown = render_markdown_report(report)
    if not markdown_path.is_file():
        append_failure(failures, "markdown-report-missing", f"missing markdown publication report: {markdown_path}")
    elif markdown_path.read_text(encoding="utf-8") != expected_markdown:
        append_failure(
            failures,
            "markdown-report-drift",
            "markdown publication report does not match the durable JSON publication report",
            path=markdown_path.relative_to(ROOT).as_posix() if publisher.is_under(markdown_path, ROOT) else str(markdown_path),
        )


def compare_report_contract(payload: dict[str, Any], report: dict[str, Any], failures: list[dict[str, Any]]) -> None:
    if report.get("repository") != payload.get("repository"):
        append_failure(failures, "repository-mismatch", "payload and report repository values differ")
    if report.get("milestoneCount") != len(payload["milestones"]):
        append_failure(failures, "milestone-count-drift", "report milestoneCount does not match payload milestone count")
    if report.get("issueCount") != len(payload["issues"]):
        append_failure(failures, "issue-count-drift", "report issueCount does not match payload issue count")
    if report.get("dependencyCount") != len(payload.get("dependencies", [])):
        append_failure(failures, "dependency-count-drift", "report dependencyCount does not match payload dependency count")

    milestone_ids = {milestone["id"] for milestone in payload["milestones"]}
    issue_ids = {issue["id"] for issue in payload["issues"]}
    if set(report["milestones"]) != milestone_ids:
        append_failure(failures, "milestone-id-drift", "report milestone IDs do not match payload milestone IDs")
    if set(report["issues"]) != issue_ids:
        append_failure(failures, "issue-id-drift", "report issue IDs do not match payload issue IDs")

    milestone_numbers = [record.get("number") for record in report["milestones"].values()]
    issue_numbers = [record.get("number") for record in report["issues"].values()]
    if len(milestone_numbers) != len(set(milestone_numbers)):
        append_failure(failures, "duplicate-milestone-number", "report contains duplicate GitHub milestone numbers")
    if len(issue_numbers) != len(set(issue_numbers)):
        append_failure(failures, "duplicate-issue-number", "report contains duplicate GitHub issue numbers")

    expected_dependencies = publisher.build_dependency_report(payload, report["issues"])
    observed_by_pair = {
        (dependency.get("blocked"), dependency.get("blocker")): dependency
        for dependency in report.get("dependencies", [])
    }
    for expected in expected_dependencies:
        key = (expected["blocked"], expected["blocker"])
        observed = observed_by_pair.get(key)
        if observed is None:
            append_failure(failures, "dependency-missing", "report is missing dependency mapping", blocked=key[0], blocker=key[1])
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
        has_relationship = draft_id in publisher.dependencies_by_issue(payload)[0] or draft_id in publisher.dependencies_by_issue(payload)[1]
        if has_relationship and publisher.RELATIONSHIP_START not in body:
            append_failure(
                failures,
                "live-issue-relationship-block-missing",
                "GitHub issue body is missing the managed relationship block",
                draft_id=draft_id,
            )

    return {"enabled": True, "checked_milestones": checked_milestones, "checked_issues": checked_issues}


def build_drift_report(
    payload_path: Path,
    publication_report_path: Path,
    markdown_path: Path,
    payload: dict[str, Any],
    report: dict[str, Any],
    failures: list[dict[str, Any]],
    live_summary: dict[str, Any],
) -> dict[str, Any]:
    return {
        "contract_id": DRIFT_CONTRACT_ID,
        "repository": payload["repository"],
        "source_updated_at_utc": report["updatedAtUtc"],
        "payload_path": payload_path.relative_to(ROOT).as_posix() if publisher.is_under(payload_path, ROOT) else str(payload_path),
        "publication_report_path": publication_report_path.relative_to(ROOT).as_posix()
        if publisher.is_under(publication_report_path, ROOT)
        else str(publication_report_path),
        "markdown_report_path": markdown_path.relative_to(ROOT).as_posix() if publisher.is_under(markdown_path, ROOT) else str(markdown_path),
        "milestone_count": len(payload["milestones"]),
        "issue_count": len(payload["issues"]),
        "dependency_count": len(payload.get("dependencies", [])),
        "live": live_summary,
        "failure_count": len(failures),
        "failures": failures,
    }


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--payload", type=Path, default=DEFAULT_PAYLOAD)
    parser.add_argument("--publication-report", type=Path, default=DEFAULT_PUBLICATION_REPORT)
    parser.add_argument("--markdown-report", type=Path, default=DEFAULT_MARKDOWN_REPORT)
    parser.add_argument("--output", type=Path, default=DEFAULT_DRIFT_REPORT)
    parser.add_argument("--write", action="store_true", help="Write the drift report.")
    parser.add_argument("--check", action="store_true", help="Fail if checked-in generated references are out of sync.")
    parser.add_argument("--update-payload-published", action="store_true", help="Replace payload.published from the JSON publication report.")
    parser.add_argument("--update-markdown-report", action="store_true", help="Rewrite the markdown publication report from JSON.")
    parser.add_argument("--live", action="store_true", help="Compare the durable mapping against live GitHub issues and milestones.")
    parser.add_argument("--limit-live", type=int, default=None, help="Limit live issue checks; milestones are always fully checked.")
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    try:
        payload_path = repo_path(args.payload)
        publication_report_path = repo_path(args.publication_report)
        markdown_path = repo_path(args.markdown_report)
        output_path = repo_path(args.output)
        publisher.assert_not_tmp_source(payload_path)
        payload = require_dict(load_json(payload_path), "payload")
        report = require_dict(load_json(publication_report_path), "publication report")
        publisher.validate_payload(payload)
        publisher.validate_existing_report(report, payload)

        if args.update_payload_published:
            payload["published"] = publication_snapshot(report)
            write_json(payload_path, payload)
        if args.update_markdown_report:
            markdown_path.parent.mkdir(parents=True, exist_ok=True)
            markdown_path.write_text(render_markdown_report(report), encoding="utf-8", newline="\n")

        failures: list[dict[str, Any]] = []
        compare_report_contract(payload, report, failures)
        compare_publication_references(payload, report, markdown_path, failures)
        live_summary = {"enabled": False, "checked_milestones": 0, "checked_issues": 0}
        if args.live:
            live_summary = compare_live_github(payload, report, failures, args.limit_live)
        drift_report = build_drift_report(
            payload_path,
            publication_report_path,
            markdown_path,
            payload,
            report,
            failures,
            live_summary,
        )

        rendered = json.dumps(drift_report, indent=2) + "\n"
        if args.write:
            output_path.parent.mkdir(parents=True, exist_ok=True)
            output_path.write_text(rendered, encoding="utf-8", newline="\n")
        if args.check:
            if not output_path.is_file():
                append_failure(failures, "drift-report-missing", f"missing drift report: {output_path}")
            elif output_path.read_text(encoding="utf-8") != rendered:
                append_failure(failures, "drift-report-stale", "checked-in publication drift report is stale")

        if failures:
            print(f"planning-publication-drift-audit: FAIL failures={len(failures)}", file=sys.stderr)
            for failure in failures[:10]:
                print(f"- {failure['code']}: {failure['detail']}", file=sys.stderr)
            return 1
        print(
            "planning-publication-drift-audit: OK "
            f"milestones={len(payload['milestones'])} issues={len(payload['issues'])} "
            f"dependencies={len(payload.get('dependencies', []))} live_issues={live_summary['checked_issues']}"
        )
        return 0
    except (DriftAuditError, publisher.PublicationError, FileNotFoundError, json.JSONDecodeError) as exc:
        print(f"planning-publication-drift-audit: ERROR {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
