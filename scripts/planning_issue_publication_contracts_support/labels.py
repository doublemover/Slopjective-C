from __future__ import annotations

from typing import Any

from .dependencies import collect_dependencies
from .errors import PublicationError
from .guards import require_dict, require_list
from .models import LabelDefinition


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
        for label in require_list(
            issue_obj.get("labels"),
            f"{issue_obj.get('id', '<unknown>')}.labels",
        ):
            if not isinstance(label, str) or not label.strip():
                raise PublicationError(
                    f"{issue_obj.get('id', '<unknown>')}.labels contains a non-string label"
                )
            labels[label] = LabelDefinition(
                name=label,
                color=stable_label_color(label),
                description=f"Objective-C 3 planning label: {label}",
            )

    dependency_blocked_ids = {dep["blocked"] for dep in collect_dependencies(payload)}
    dependency_blocker_ids = {dep["blocker"] for dep in collect_dependencies(payload)}
    if dependency_blocked_ids:
        labels["blocked"] = LabelDefinition(
            "blocked",
            stable_label_color("blocked"),
            "Issue has a tracked blocker.",
        )
    if dependency_blocker_ids:
        labels["blocks"] = LabelDefinition(
            "blocks",
            stable_label_color("blocks"),
            "Issue blocks another tracked issue.",
        )
    return dict(sorted(labels.items()))
