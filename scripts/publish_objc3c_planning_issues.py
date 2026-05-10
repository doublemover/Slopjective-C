#!/usr/bin/env python3
"""Publish checked-in Objective-C 3 planning issues to GitHub.

The publisher intentionally treats GitHub numbers as assigned state. Draft IDs
stay stable, and the durable report records the GitHub milestone/issue numbers
that were actually returned by the API.
"""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
import time
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.json_io import write_json_file as write_json
try:
    from planning_issue_publication_contracts import (
        LabelDefinition,
        PublicationError,
        apply_relationship_section,
        build_dry_run_report,
        build_report,
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
except ModuleNotFoundError:
    from scripts.planning_issue_publication_contracts import (
        LabelDefinition,
        PublicationError,
        apply_relationship_section,
        build_dry_run_report,
        build_report,
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

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_PAYLOAD = ROOT / "reports" / "planning" / "objc3c_3_next_40_github_payloads.json"
DEFAULT_REPORT = ROOT / "reports" / "planning" / "objc3c_3_next_40_publication_report.json"


def repo_path(path: Path) -> Path:
    return path if path.is_absolute() else ROOT / path


def is_under(child: Path, parent: Path) -> bool:
    try:
        child.resolve().relative_to(parent.resolve())
        return True
    except ValueError:
        return False


def assert_not_tmp_source(path: Path) -> None:
    if is_under(path, ROOT / "tmp"):
        raise PublicationError(
            f"refusing transient planning input: {path.relative_to(ROOT).as_posix()}"
        )


def load_json(path: Path) -> Any:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise PublicationError(f"missing JSON file: {path}") from exc
    except json.JSONDecodeError as exc:
        raise PublicationError(f"invalid JSON in {path}: {exc}") from exc


def run_gh(args: Sequence[str], input_payload: Any | None = None, retries: int = 3) -> subprocess.CompletedProcess[str]:
    command = ["gh", *args]
    stdin = None if input_payload is None else json.dumps(input_payload)
    for attempt in range(1, retries + 1):
        result = subprocess.run(
            command,
            cwd=ROOT,
            input=stdin,
            text=True,
            capture_output=True,
            check=False,
        )
        if result.returncode == 0:
            return result
        stderr = result.stderr.lower()
        retryable = any(token in stderr for token in ("secondary rate limit", "abuse detection", "502", "503", "504", "timeout"))
        if not retryable or attempt == retries:
            raise PublicationError(
                f"GitHub command failed ({' '.join(command)}): {result.stderr.strip() or result.stdout.strip()}"
            )
        time.sleep(60 if "secondary rate limit" in stderr or "abuse detection" in stderr else 5 * attempt)
    raise AssertionError("unreachable")


def gh_json(args: Sequence[str], input_payload: Any | None = None) -> Any:
    result = run_gh(args, input_payload=input_payload)
    if not result.stdout.strip():
        return None
    try:
        return json.loads(result.stdout)
    except json.JSONDecodeError as exc:
        raise PublicationError(f"GitHub command did not return JSON: {' '.join(['gh', *args])}") from exc


def ensure_labels(repo: str, labels: dict[str, LabelDefinition], sleep_seconds: float) -> dict[str, Any]:
    existing_payload = gh_json(["label", "list", "--repo", repo, "--limit", "1000", "--json", "name"])
    existing = {record["name"] for record in existing_payload}
    created: list[str] = []
    reused: list[str] = []
    for label in labels.values():
        if label.name in existing:
            reused.append(label.name)
            continue
        run_gh(
            [
                "label",
                "create",
                label.name,
                "--repo",
                repo,
                "--color",
                label.color,
                "--description",
                label.description,
            ]
        )
        created.append(label.name)
        if sleep_seconds:
            time.sleep(sleep_seconds)
    return {"created": created, "reused": reused}


def fetch_milestones(repo: str) -> dict[str, dict[str, Any]]:
    records: list[dict[str, Any]] = []
    page = 1
    while True:
        payload = gh_json(["api", f"repos/{repo}/milestones?state=all&per_page=100&page={page}"])
        if not isinstance(payload, list):
            raise PublicationError("GitHub milestones response must be an array")
        records.extend(payload)
        if len(payload) < 100:
            break
        page += 1
    return {record["title"]: record for record in records}


def ensure_milestones(repo: str, payload: dict[str, Any], report: dict[str, Any], sleep_seconds: float) -> dict[str, Any]:
    existing_by_title = fetch_milestones(repo)
    milestone_report = dict(report.get("milestones", {}))
    published_milestones = payload.get("published", {}).get("milestones", {})
    if not isinstance(published_milestones, dict):
        published_milestones = {}
    for milestone in payload["milestones"]:
        existing = existing_by_title.get(milestone["title"])
        if existing is None:
            existing = gh_json(
                ["api", f"repos/{repo}/milestones", "-X", "POST", "--input", "-"],
                {"title": milestone["title"], "description": milestone.get("description", "")},
            )
            if sleep_seconds:
                time.sleep(sleep_seconds)
        prior = milestone_report.get(milestone["id"], {})
        published_prior = published_milestones.get(milestone["id"], {})
        if not isinstance(published_prior, dict):
            published_prior = {}
        milestone_report[milestone["id"]] = {
            "number": existing["number"],
            "id": existing["id"],
            "title": existing["title"],
            "html_url": existing["html_url"],
            "reused": published_prior.get("reused", prior.get("reused", milestone["title"] in existing_by_title)),
        }
    return milestone_report


def create_or_update_issues(
    repo: str,
    payload: dict[str, Any],
    report: dict[str, Any],
    milestone_report: dict[str, Any],
    update_existing: bool,
    limit: int | None,
    sleep_seconds: float,
) -> dict[str, Any]:
    issue_report = dict(report.get("issues", {}))
    published_issues = payload.get("published", {}).get("issues", {})
    if not isinstance(published_issues, dict):
        published_issues = {}
    title_to_milestone = milestone_by_title(payload)
    blocked_by, blocks = dependencies_by_issue(payload)
    processed = 0
    for issue in payload["issues"]:
        issue_id = issue["id"]
        milestone_id = title_to_milestone[issue["milestoneTitle"]]["id"]
        milestone_number = milestone_report[milestone_id]["number"]
        section = render_relationship_section(issue_id, issue_report, blocked_by, blocks)
        body = apply_relationship_section(issue["body"], section)
        labels = issue_labels(issue, issue_id, payload)
        existing_number = issue_number(issue_report, issue_id)
        if existing_number is not None:
            if update_existing:
                current_issue = gh_json(["api", f"repos/{repo}/issues/{existing_number}"])
                current_labels = [
                    label["name"]
                    for label in current_issue.get("labels", [])
                    if isinstance(label, dict) and isinstance(label.get("name"), str)
                ]
                merged_labels = sorted(dict.fromkeys([*current_labels, *labels]))
                current_body = current_issue.get("body") if isinstance(current_issue.get("body"), str) else issue["body"]
                gh_json(
                    ["api", f"repos/{repo}/issues/{existing_number}", "-X", "PATCH", "--input", "-"],
                    {
                        "title": issue["title"],
                        "body": apply_relationship_section(current_body, section),
                        "milestone": milestone_number,
                        "labels": merged_labels,
                    },
                )
                if sleep_seconds:
                    time.sleep(sleep_seconds)
            issue_report[issue_id] = {
                **issue_report[issue_id],
                "milestone_number": milestone_number,
                "reused": (
                    published_issues.get(issue_id, {}).get("reused")
                    if isinstance(published_issues.get(issue_id), dict)
                    else issue_report[issue_id].get("reused", True)
                ),
            }
            continue
        if limit is not None and processed >= limit:
            break
        created = gh_json(
            ["api", f"repos/{repo}/issues", "-X", "POST", "--input", "-"],
            {
                "title": issue["title"],
                "body": body,
                "milestone": milestone_number,
                "labels": labels,
            },
        )
        issue_report[issue_id] = {
            "number": created["number"],
            "id": created["id"],
            "node_id": created["node_id"],
            "title": created["title"],
            "html_url": created["html_url"],
            "milestone_number": milestone_number,
            "reused": False,
        }
        processed += 1
        if sleep_seconds:
            time.sleep(sleep_seconds)
    return issue_report


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--payload", type=Path, default=DEFAULT_PAYLOAD, help="Checked-in planning GitHub payload JSON.")
    parser.add_argument("--report", type=Path, default=DEFAULT_REPORT, help="Durable publication mapping report JSON.")
    parser.add_argument("--repo", default="", help="Override repository owner/name. Defaults to payload.repository.")
    parser.add_argument("--apply", action="store_true", help="Create/update GitHub labels, milestones, and issues.")
    parser.add_argument("--update-existing", action="store_true", help="Patch mapped existing issues with current title/body/labels/milestone.")
    parser.add_argument("--limit", type=int, default=None, help="Maximum number of new issues to create in this run.")
    parser.add_argument("--sleep-seconds", type=float, default=1.0, help="Pause between GitHub write operations.")
    parser.add_argument("--write-report", action="store_true", help="Write the durable report during dry-run validation.")
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    try:
        payload_path = repo_path(args.payload)
        report_path = repo_path(args.report)
        assert_not_tmp_source(payload_path)
        payload = require_dict(load_json(payload_path), "payload")
        validate_payload(payload)
        existing_report = load_json(report_path) if report_path.is_file() else {}
        existing_report = require_dict(existing_report, "publication report")
        validate_existing_report(existing_report, payload)
        repo = args.repo or payload["repository"]
        payload["repository"] = repo

        if args.apply:
            labels = collect_label_definitions(payload)
            label_summary = ensure_labels(repo, labels, args.sleep_seconds)
            milestone_report = ensure_milestones(repo, payload, existing_report, args.sleep_seconds)
            issue_report = create_or_update_issues(
                repo,
                payload,
                existing_report,
                milestone_report,
                args.update_existing,
                args.limit,
                args.sleep_seconds,
            )
            report = build_report(payload, existing_report, milestone_report, issue_report)
            report["labelSummary"] = label_summary
            write_json(report_path, report)
            print(
                "planning-issue-publisher: OK "
                f"repo={repo} labels={len(labels)} milestones={len(milestone_report)} "
                f"issues={len(issue_report)} dependencies={len(report['dependencies'])} report={report_path.relative_to(ROOT).as_posix()}"
            )
            return 0

        report = build_dry_run_report(payload, existing_report)
        unresolved = [dep for dep in report["dependencies"] if not dep["resolved"]]
        if args.write_report:
            write_json(report_path, report)
        print(
            "planning-issue-publisher: OK dry-run "
            f"repo={repo} milestones={len(payload['milestones'])} issues={len(payload['issues'])} "
            f"labels={len(collect_label_definitions(payload))} dependencies={len(report['dependencies'])} "
            f"unresolved_dependencies={len(unresolved)}"
        )
        return 0
    except PublicationError as exc:
        print(f"planning-issue-publisher: ERROR {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
