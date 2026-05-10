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
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.json_io import write_json_file as write_json

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_PAYLOAD = ROOT / "reports" / "planning" / "objc3c_3_next_40_github_payloads.json"
DEFAULT_REPORT = ROOT / "reports" / "planning" / "objc3c_3_next_40_publication_report.json"
REPORT_CONTRACT_ID = "objc3c.planning.github-publication-report.v1"
RELATIONSHIP_START = "<!-- objc3c-publisher:relationships:start -->"
RELATIONSHIP_END = "<!-- objc3c-publisher:relationships:end -->"


class PublicationError(RuntimeError):
    """Raised when the planning payload cannot be safely published."""


@dataclass(frozen=True)
class LabelDefinition:
    name: str
    color: str
    description: str


def utc_now() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


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



def require_dict(value: Any, name: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise PublicationError(f"{name} must be an object")
    return value


def require_list(value: Any, name: str) -> list[Any]:
    if not isinstance(value, list):
        raise PublicationError(f"{name} must be an array")
    return value


def require_str(record: dict[str, Any], key: str, context: str) -> str:
    value = record.get(key)
    if not isinstance(value, str) or not value.strip():
        raise PublicationError(f"{context}.{key} must be a non-empty string")
    return value


def stable_label_color(name: str) -> str:
    if name.startswith("priority:"):
        return "b60205"
    if name.startswith("area:"):
        return "1d76db"
    if name.startswith("kind:"):
        return "0e8a16"
    if name.startswith("type:"):
        return "5319e7"
    if name.startswith("source:"):
        return "fbca04"
    if name.startswith("publication:"):
        return "c2e0c6"
    if name in {"blocked", "blocks"}:
        return "d93f0b"
    return "ededed"


def collect_label_definitions(payload: dict[str, Any]) -> dict[str, LabelDefinition]:
    labels: dict[str, LabelDefinition] = {}
    for issue in require_list(payload.get("issues"), "issues"):
        issue_obj = require_dict(issue, "issues[]")
        for label in require_list(issue_obj.get("labels"), f"{issue_obj.get('id', '<unknown>')}.labels"):
            if not isinstance(label, str) or not label.strip():
                raise PublicationError(f"{issue_obj.get('id', '<unknown>')}.labels contains a non-string label")
            labels[label] = LabelDefinition(
                name=label,
                color=stable_label_color(label),
                description=f"Objective-C 3 planning label: {label}",
            )

    dependency_blocked_ids = {dep["blocked"] for dep in collect_dependencies(payload)}
    dependency_blocker_ids = {dep["blocker"] for dep in collect_dependencies(payload)}
    if dependency_blocked_ids:
        labels["blocked"] = LabelDefinition("blocked", stable_label_color("blocked"), "Issue has a tracked blocker.")
    if dependency_blocker_ids:
        labels["blocks"] = LabelDefinition("blocks", stable_label_color("blocks"), "Issue blocks another tracked issue.")
    return dict(sorted(labels.items()))


def collect_dependencies(payload: dict[str, Any]) -> list[dict[str, str]]:
    dependencies: list[dict[str, str]] = []
    for index, dependency in enumerate(require_list(payload.get("dependencies", []), "dependencies")):
        dep_obj = require_dict(dependency, f"dependencies[{index}]")
        blocked = require_str(dep_obj, "blocked", f"dependencies[{index}]")
        blocker = require_str(dep_obj, "blocker", f"dependencies[{index}]")
        reason = dep_obj.get("reason", "")
        if reason is not None and not isinstance(reason, str):
            raise PublicationError(f"dependencies[{index}].reason must be a string when present")
        dependencies.append({"blocked": blocked, "blocker": blocker, "reason": reason or ""})
    return dependencies


def validate_payload(payload: dict[str, Any]) -> None:
    require_str(payload, "repository", "root")
    rules = require_dict(payload.get("rules", {}), "rules")
    if rules.get("doNotUseTmpAsSourceOfTruth") is not True:
        raise PublicationError("rules.doNotUseTmpAsSourceOfTruth must be true")
    if rules.get("acceptGithubAssignedNumbers") is not True:
        raise PublicationError("rules.acceptGithubAssignedNumbers must be true")

    milestones = require_list(payload.get("milestones"), "milestones")
    issues = require_list(payload.get("issues"), "issues")
    if not milestones:
        raise PublicationError("milestones must not be empty")
    if not issues:
        raise PublicationError("issues must not be empty")

    milestone_ids: set[str] = set()
    milestone_titles: set[str] = set()
    for index, milestone in enumerate(milestones):
        milestone_obj = require_dict(milestone, f"milestones[{index}]")
        milestone_id = require_str(milestone_obj, "id", f"milestones[{index}]")
        title = require_str(milestone_obj, "title", f"milestones[{index}]")
        if milestone_id in milestone_ids:
            raise PublicationError(f"duplicate milestone id: {milestone_id}")
        if title in milestone_titles:
            raise PublicationError(f"duplicate milestone title: {title}")
        milestone_ids.add(milestone_id)
        milestone_titles.add(title)

    issue_ids: set[str] = set()
    for index, issue in enumerate(issues):
        issue_obj = require_dict(issue, f"issues[{index}]")
        issue_id = require_str(issue_obj, "id", f"issues[{index}]")
        title = require_str(issue_obj, "title", f"issues[{index}]")
        body = require_str(issue_obj, "body", f"issues[{index}]")
        milestone_title = require_str(issue_obj, "milestoneTitle", f"issues[{index}]")
        if issue_id in issue_ids:
            raise PublicationError(f"duplicate issue id: {issue_id}")
        if f"<!-- draft-id: {issue_id} -->" not in body:
            raise PublicationError(f"{issue_id} body is missing its stable draft-id marker")
        if issue_id not in title:
            raise PublicationError(f"{issue_id} title must include its stable draft id")
        if milestone_title not in milestone_titles:
            raise PublicationError(f"{issue_id} references unknown milestone title: {milestone_title}")
        require_list(issue_obj.get("labels"), f"{issue_id}.labels")
        issue_ids.add(issue_id)

    for dependency in collect_dependencies(payload):
        if dependency["blocked"] not in issue_ids:
            raise PublicationError(f"dependency blocked id is unknown: {dependency['blocked']}")
        if dependency["blocker"] not in issue_ids:
            raise PublicationError(f"dependency blocker id is unknown: {dependency['blocker']}")


def validate_existing_report(report: dict[str, Any], payload: dict[str, Any]) -> None:
    if not report:
        return
    repo = report.get("repository")
    if repo is not None and repo != payload["repository"]:
        raise PublicationError(f"publication report repository {repo!r} does not match payload {payload['repository']!r}")
    if not isinstance(report.get("milestones", {}), dict):
        raise PublicationError("publication report milestones must be an object")
    if not isinstance(report.get("issues", {}), dict):
        raise PublicationError("publication report issues must be an object")


def milestone_by_title(payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {milestone["title"]: milestone for milestone in payload["milestones"]}


def dependencies_by_issue(payload: dict[str, Any]) -> tuple[dict[str, list[dict[str, str]]], dict[str, list[dict[str, str]]]]:
    blocked_by: dict[str, list[dict[str, str]]] = {}
    blocks: dict[str, list[dict[str, str]]] = {}
    for dependency in collect_dependencies(payload):
        blocked_by.setdefault(dependency["blocked"], []).append(dependency)
        blocks.setdefault(dependency["blocker"], []).append(dependency)
    return blocked_by, blocks


def issue_number(mapping: dict[str, Any], draft_id: str) -> int | None:
    record = mapping.get(draft_id)
    if isinstance(record, dict) and isinstance(record.get("number"), int):
        return record["number"]
    return None


def render_relationship_section(
    issue_id: str,
    existing_issue_mapping: dict[str, Any],
    blocked_by: dict[str, list[dict[str, str]]],
    blocks: dict[str, list[dict[str, str]]],
) -> str:
    lines = [RELATIONSHIP_START, "## GitHub Relationships"]
    if issue_id in blocked_by:
        refs = []
        for dependency in blocked_by[issue_id]:
            number = issue_number(existing_issue_mapping, dependency["blocker"])
            ref = f"#{number}" if number is not None else dependency["blocker"]
            reason = f" ({dependency['reason']})" if dependency.get("reason") else ""
            refs.append(f"{ref}{reason}")
        lines.append(f"- Blocked by: {', '.join(refs)}")
    if issue_id in blocks:
        refs = []
        for dependency in blocks[issue_id]:
            number = issue_number(existing_issue_mapping, dependency["blocked"])
            ref = f"#{number}" if number is not None else dependency["blocked"]
            reason = f" ({dependency['reason']})" if dependency.get("reason") else ""
            refs.append(f"{ref}{reason}")
        lines.append(f"- Blocks: {', '.join(refs)}")
    if len(lines) == 2:
        lines.append("- No tracked blockers.")
    lines.append(RELATIONSHIP_END)
    return "\n".join(lines)


def apply_relationship_section(body: str, section: str) -> str:
    if RELATIONSHIP_START in body and RELATIONSHIP_END in body:
        prefix, rest = body.split(RELATIONSHIP_START, 1)
        _, suffix = rest.split(RELATIONSHIP_END, 1)
        return prefix.rstrip() + "\n\n" + section + suffix
    return body.rstrip() + "\n\n" + section + "\n"


def issue_labels(issue: dict[str, Any], issue_id: str, payload: dict[str, Any]) -> list[str]:
    labels = list(issue.get("labels", []))
    blocked_by, blocks = dependencies_by_issue(payload)
    if issue_id in blocked_by:
        labels.append("blocked")
    if issue_id in blocks:
        labels.append("blocks")
    return sorted(dict.fromkeys(labels))


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


def prior_dependency_statuses(payload: dict[str, Any]) -> dict[tuple[str, str], str]:
    published = payload.get("published", {})
    if not isinstance(published, dict):
        return {}
    statuses: dict[tuple[str, str], str] = {}
    for dependency in published.get("dependencies", []):
        if not isinstance(dependency, dict):
            continue
        blocked = dependency.get("blocked")
        blocker = dependency.get("blocker")
        status = dependency.get("status")
        if isinstance(blocked, str) and isinstance(blocker, str) and isinstance(status, str):
            statuses[(blocked, blocker)] = status
    return statuses


def build_dependency_report(payload: dict[str, Any], issue_report: dict[str, Any]) -> list[dict[str, Any]]:
    records: list[dict[str, Any]] = []
    prior_status_by_pair = prior_dependency_statuses(payload)
    for dependency in collect_dependencies(payload):
        blocked_number = issue_number(issue_report, dependency["blocked"])
        blocker_number = issue_number(issue_report, dependency["blocker"])
        resolved = blocked_number is not None and blocker_number is not None
        records.append(
            {
                "blocked": dependency["blocked"],
                "blocked_number": blocked_number,
                "blocker": dependency["blocker"],
                "blocker_number": blocker_number,
                "reason": dependency.get("reason", ""),
                "status": prior_status_by_pair.get(
                    (dependency["blocked"], dependency["blocker"]),
                    "resolved" if resolved else "unresolved",
                ),
                "blocked_issue_number": blocked_number,
                "blocker_issue_number": blocker_number,
                "resolved": resolved,
                "native_relationship": "body-reference-and-label",
            }
        )
    return records


def build_report(
    payload: dict[str, Any],
    existing_report: dict[str, Any],
    milestone_report: dict[str, Any],
    issue_report: dict[str, Any],
) -> dict[str, Any]:
    dependencies = build_dependency_report(payload, issue_report)
    return {
        "contract_id": REPORT_CONTRACT_ID,
        "repository": payload["repository"],
        "startedAtUtc": existing_report.get("startedAtUtc", utc_now()),
        "updatedAtUtc": utc_now(),
        "milestoneCount": len(payload["milestones"]),
        "issueCount": len(payload["issues"]),
        "dependencyCount": len(payload.get("dependencies", [])),
        "milestones": milestone_report,
        "issues": issue_report,
        "dependencies": dependencies,
        "failures": [],
    }


def build_dry_run_report(payload: dict[str, Any], existing_report: dict[str, Any]) -> dict[str, Any]:
    milestone_report = dict(existing_report.get("milestones", {}))
    issue_report = dict(existing_report.get("issues", {}))
    return build_report(payload, existing_report, milestone_report, issue_report)


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
