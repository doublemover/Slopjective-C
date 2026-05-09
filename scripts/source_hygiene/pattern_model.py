from __future__ import annotations

from dataclasses import dataclass

from .owners import (
    SOURCE_HYGIENE_PATTERN_OWNER,
    SOURCE_HYGIENE_PATTERN_OWNER_SURFACE,
)


@dataclass(frozen=True)
class ForbiddenPattern:
    pattern_id: str
    description: str
    regex: str
    severity: str = "error"
    include_paths: tuple[str, ...] = ()
    exclude_paths: tuple[str, ...] = ()
    residue_class: str = "general-hard-cutover-residue"
    gate_contract: str = "source-hygiene-hard-cutover"
    owner_id: str = SOURCE_HYGIENE_PATTERN_OWNER
    owner_surface: str = SOURCE_HYGIENE_PATTERN_OWNER_SURFACE


@dataclass(frozen=True)
class ForbiddenPatternGroup:
    group_id: str
    description: str
    owner_surface: str
    patterns: tuple[ForbiddenPattern, ...]


def flatten_pattern_groups(
    groups: tuple[ForbiddenPatternGroup, ...],
) -> tuple[ForbiddenPattern, ...]:
    return tuple(pattern for group in groups for pattern in group.patterns)


def pattern_group_report_records(
    groups: tuple[ForbiddenPatternGroup, ...],
) -> list[dict[str, object]]:
    return [
        {
            "group_id": group.group_id,
            "description": group.description,
            "owner_surface": group.owner_surface,
            "pattern_count": len(group.patterns),
            "pattern_ids": [pattern.pattern_id for pattern in group.patterns],
            "residue_classes": sorted(
                {pattern.residue_class for pattern in group.patterns}
            ),
        }
        for group in groups
    ]
