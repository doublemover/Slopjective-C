from __future__ import annotations

from typing import Any

from .constants import RELATIONSHIP_END, RELATIONSHIP_START
from .dependencies import dependencies_by_issue
from .records import issue_number


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
