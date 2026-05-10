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
try:
    import publish_objc3c_planning_issues as publisher
    from planning_publication_drift_contracts import (
        PlanningPublicationDriftInputs,
        PlanningPublicationDriftPaths,
        append_failure,
        build_drift_report,
        compare_publication_references,
        compare_report_contract,
        publication_snapshot,
        render_markdown_report,
        repo_relative_path,
        success_status_line,
    )
except ModuleNotFoundError:
    from scripts import publish_objc3c_planning_issues as publisher
    from scripts.planning_publication_drift_contracts import (
        PlanningPublicationDriftInputs,
        PlanningPublicationDriftPaths,
        append_failure,
        build_drift_report,
        compare_publication_references,
        compare_report_contract,
        publication_snapshot,
        render_markdown_report,
        repo_relative_path,
        success_status_line,
    )

DEFAULT_PAYLOAD = ROOT / "reports" / "planning" / "objc3c_3_next_40_github_payloads.json"
DEFAULT_PUBLICATION_REPORT = ROOT / "reports" / "planning" / "objc3c_3_next_40_publication_report.json"
DEFAULT_MARKDOWN_REPORT = ROOT / "reports" / "planning" / "objc3c_3_next_40_publication_report.md"
DEFAULT_DRIFT_REPORT = ROOT / "reports" / "planning" / "objc3c_3_next_40_publication_drift_report.json"


class DriftAuditError(RuntimeError):
    """Raised when audit inputs are malformed."""




def repo_path(path: Path) -> Path:
    return path if path.is_absolute() else ROOT / path


def require_dict(value: Any, name: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise DriftAuditError(f"{name} must be an object")
    return value


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
        compare_report_contract(
            payload=payload,
            report=report,
            expected_dependencies=publisher.build_dependency_report(payload, report["issues"]),
            failures=failures,
        )
        compare_publication_references(
            payload=payload,
            report=report,
            markdown_text=markdown_path.read_text(encoding="utf-8")
            if markdown_path.is_file()
            else None,
            markdown_display_path=str(markdown_path),
            markdown_report_path=repo_relative_path(markdown_path, ROOT),
            failures=failures,
        )
        live_summary = {"enabled": False, "checked_milestones": 0, "checked_issues": 0}
        if args.live:
            live_summary = compare_live_github(payload, report, failures, args.limit_live)
        drift_report = build_drift_report(
            PlanningPublicationDriftInputs(
                paths=PlanningPublicationDriftPaths(
                    payload_path=repo_relative_path(payload_path, ROOT),
                    publication_report_path=repo_relative_path(publication_report_path, ROOT),
                    markdown_report_path=repo_relative_path(markdown_path, ROOT),
                ),
                payload=payload,
                report=report,
                failures=failures,
                live_summary=live_summary,
            )
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
        print(success_status_line(payload, live_summary))
        return 0
    except (DriftAuditError, publisher.PublicationError, FileNotFoundError, json.JSONDecodeError) as exc:
        print(f"planning-publication-drift-audit: ERROR {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
