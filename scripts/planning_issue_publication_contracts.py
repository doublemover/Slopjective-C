from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime, timezone
from typing import Any

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


__all__ = [
    "LabelDefinition",
    "PublicationError",
    "apply_relationship_section",
    "build_dry_run_report",
    "build_report",
    "collect_dependencies",
    "collect_label_definitions",
    "dependencies_by_issue",
    "issue_labels",
    "issue_number",
    "milestone_by_title",
    "render_relationship_section",
    "require_dict",
    "validate_existing_report",
    "validate_payload",
]
