from __future__ import annotations

from typing import Any

from .dependencies import collect_dependencies
from .errors import PublicationError
from .guards import require_dict, require_list, require_str


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
            raise PublicationError(
                f"{issue_id} references unknown milestone title: {milestone_title}"
            )
        require_list(issue_obj.get("labels"), f"{issue_id}.labels")
        issue_ids.add(issue_id)

    for dependency in collect_dependencies(payload):
        if dependency["blocked"] not in issue_ids:
            raise PublicationError(
                f"dependency blocked id is unknown: {dependency['blocked']}"
            )
        if dependency["blocker"] not in issue_ids:
            raise PublicationError(
                f"dependency blocker id is unknown: {dependency['blocker']}"
            )


def validate_existing_report(report: dict[str, Any], payload: dict[str, Any]) -> None:
    if not report:
        return
    repo = report.get("repository")
    if repo is not None and repo != payload["repository"]:
        raise PublicationError(
            f"publication report repository {repo!r} does not match "
            f"payload {payload['repository']!r}"
        )
    if not isinstance(report.get("milestones", {}), dict):
        raise PublicationError("publication report milestones must be an object")
    if not isinstance(report.get("issues", {}), dict):
        raise PublicationError("publication report issues must be an object")
