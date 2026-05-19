"""Publication workflow for Objective-C 3 planning issues."""

from __future__ import annotations

import time
from typing import Any

from .contracts import (
    LabelDefinition,
    apply_relationship_section,
    build_dry_run_report,
    build_report,
    collect_label_definitions,
)
from .github import fetch_milestones, gh_json, run_gh
from .models import AppliedPublicationResult, DryRunPublicationResult
from .rendering import build_issue_publication_plan, relationship_maps


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


def ensure_milestones(
    repo: str,
    payload: dict[str, Any],
    report: dict[str, Any],
    sleep_seconds: float,
) -> dict[str, Any]:
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
                {
                    "title": milestone["title"],
                    "description": milestone.get("description", ""),
                },
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
            "reused": published_prior.get(
                "reused",
                prior.get("reused", milestone["title"] in existing_by_title),
            ),
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
    blocked_by, blocks = relationship_maps(payload)
    processed = 0
    for issue in payload["issues"]:
        plan = build_issue_publication_plan(
            issue,
            payload,
            issue_report,
            milestone_report,
            blocked_by,
            blocks,
        )
        if plan.existing_number is not None:
            if update_existing:
                current_issue = gh_json(["api", f"repos/{repo}/issues/{plan.existing_number}"])
                current_labels = [
                    label["name"]
                    for label in current_issue.get("labels", [])
                    if isinstance(label, dict) and isinstance(label.get("name"), str)
                ]
                merged_labels = sorted(dict.fromkeys([*current_labels, *plan.labels]))
                current_body = (
                    current_issue.get("body")
                    if isinstance(current_issue.get("body"), str)
                    else issue["body"]
                )
                gh_json(
                    ["api", f"repos/{repo}/issues/{plan.existing_number}", "-X", "PATCH", "--input", "-"],
                    {
                        "title": issue["title"],
                        "body": apply_relationship_section(current_body, plan.relationship_section),
                        "milestone": plan.milestone_number,
                        "labels": merged_labels,
                    },
                )
                if sleep_seconds:
                    time.sleep(sleep_seconds)
            published_issue = published_issues.get(plan.draft_id)
            issue_report[plan.draft_id] = {
                **issue_report[plan.draft_id],
                "milestone_number": plan.milestone_number,
                "reused": (
                    published_issue.get("reused")
                    if isinstance(published_issue, dict)
                    else issue_report[plan.draft_id].get("reused", True)
                ),
            }
            continue
        if limit is not None and processed >= limit:
            break
        created = gh_json(
            ["api", f"repos/{repo}/issues", "-X", "POST", "--input", "-"],
            {
                "title": issue["title"],
                "body": plan.body,
                "milestone": plan.milestone_number,
                "labels": plan.labels,
            },
        )
        issue_report[plan.draft_id] = {
            "number": created["number"],
            "id": created["id"],
            "node_id": created["node_id"],
            "title": created["title"],
            "html_url": created["html_url"],
            "milestone_number": plan.milestone_number,
            "reused": False,
        }
        processed += 1
        if sleep_seconds:
            time.sleep(sleep_seconds)
    return issue_report


def apply_publication(
    repo: str,
    payload: dict[str, Any],
    existing_report: dict[str, Any],
    update_existing: bool,
    limit: int | None,
    sleep_seconds: float,
) -> AppliedPublicationResult:
    labels = collect_label_definitions(payload)
    label_summary = ensure_labels(repo, labels, sleep_seconds)
    milestone_report = ensure_milestones(repo, payload, existing_report, sleep_seconds)
    issue_report = create_or_update_issues(
        repo,
        payload,
        existing_report,
        milestone_report,
        update_existing,
        limit,
        sleep_seconds,
    )
    report = build_report(payload, existing_report, milestone_report, issue_report)
    report["labelSummary"] = label_summary
    return AppliedPublicationResult(
        repo=repo,
        report=report,
        labels=labels,
        label_summary=label_summary,
        milestone_report=milestone_report,
        issue_report=issue_report,
    )


def dry_run_publication(
    repo: str,
    payload: dict[str, Any],
    existing_report: dict[str, Any],
) -> DryRunPublicationResult:
    report = build_dry_run_report(payload, existing_report)
    unresolved = [dep for dep in report["dependencies"] if not dep["resolved"]]
    return DryRunPublicationResult(repo=repo, report=report, unresolved_dependencies=unresolved)
