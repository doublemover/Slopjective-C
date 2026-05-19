"""Grouping and classification for runbook documentation sources."""

from __future__ import annotations

from dataclasses import dataclass

from .runbook_path_normalization import normalize_runbook_source_path
from .runbook_source_catalog import RunbookSourceSpec


RUNBOOK_GROUP_LABELS: dict[str, str] = {
    "site": "Site entrypoints",
    "native-fragments": "Native documentation fragments",
    "maintainer-workflow": "Maintainer workflow runbook",
    "boundary-runbooks": "Boundary runbooks",
    "public-command-surface": "Public command appendix",
}
RUNBOOK_GROUP_ORDER: tuple[str, ...] = tuple(RUNBOOK_GROUP_LABELS)


@dataclass(frozen=True)
class RunbookSourceGroup:
    group_id: str
    label: str
    sources: tuple[RunbookSourceSpec, ...]

    @property
    def source_count(self) -> int:
        return len(self.sources)


def classify_runbook_source(spec: RunbookSourceSpec) -> str:
    normalized_path = normalize_runbook_source_path(spec.path)
    if normalized_path.startswith("site/"):
        return "site"
    if normalized_path.startswith("docs/objc3c-native/"):
        return "native-fragments"
    if normalized_path == "docs/runbooks/objc3c_maintainer_workflows.md":
        return "maintainer-workflow"
    if normalized_path == "docs/runbooks/objc3c_public_command_surface.md":
        return "public-command-surface"
    return "boundary-runbooks"


def group_runbook_source_specs(
    specs: tuple[RunbookSourceSpec, ...],
) -> tuple[RunbookSourceGroup, ...]:
    grouped_specs: dict[str, list[RunbookSourceSpec]] = {
        group_id: [] for group_id in RUNBOOK_GROUP_ORDER
    }
    for spec in specs:
        grouped_specs[classify_runbook_source(spec)].append(spec)

    return tuple(
        RunbookSourceGroup(
            group_id=group_id,
            label=RUNBOOK_GROUP_LABELS[group_id],
            sources=tuple(grouped_specs[group_id]),
        )
        for group_id in RUNBOOK_GROUP_ORDER
        if grouped_specs[group_id]
    )


__all__ = (
    "RUNBOOK_GROUP_LABELS",
    "RUNBOOK_GROUP_ORDER",
    "RunbookSourceGroup",
    "classify_runbook_source",
    "group_runbook_source_specs",
)
