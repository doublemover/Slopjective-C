from __future__ import annotations

from typing import Any

from .errors import PublicationError
from .guards import require_dict, require_list, require_str
from .records import issue_number


def collect_dependencies(payload: dict[str, Any]) -> list[dict[str, str]]:
    dependencies: list[dict[str, str]] = []
    for index, dependency in enumerate(
        require_list(payload.get("dependencies", []), "dependencies")
    ):
        dep_obj = require_dict(dependency, f"dependencies[{index}]")
        blocked = require_str(dep_obj, "blocked", f"dependencies[{index}]")
        blocker = require_str(dep_obj, "blocker", f"dependencies[{index}]")
        reason = dep_obj.get("reason", "")
        if reason is not None and not isinstance(reason, str):
            raise PublicationError(
                f"dependencies[{index}].reason must be a string when present"
            )
        dependencies.append({"blocked": blocked, "blocker": blocker, "reason": reason or ""})
    return dependencies


def dependencies_by_issue(
    payload: dict[str, Any],
) -> tuple[dict[str, list[dict[str, str]]], dict[str, list[dict[str, str]]]]:
    blocked_by: dict[str, list[dict[str, str]]] = {}
    blocks: dict[str, list[dict[str, str]]] = {}
    for dependency in collect_dependencies(payload):
        blocked_by.setdefault(dependency["blocked"], []).append(dependency)
        blocks.setdefault(dependency["blocker"], []).append(dependency)
    return blocked_by, blocks


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


def build_dependency_report(
    payload: dict[str, Any],
    issue_report: dict[str, Any],
) -> list[dict[str, Any]]:
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
