"""Section assembly helpers for the objc3c workflow action catalog."""

from __future__ import annotations

from collections.abc import Iterable, Mapping

from .action_section_merge import merge_named_action_sections
from .action_spec import ActionSpec

ACTION_CATALOG_SECTION_MERGE_CONTRACT_ID = (
    "objc3c-workflow-action-catalog-section-merge-v1"
)
ACTION_CATALOG_SECTION_MERGE_OWNER_SURFACE = (
    "scripts/objc3c_workflow/action_catalog_sections.py"
)


def _section_items(
    section: Mapping[str, ActionSpec] | object,
) -> Iterable[tuple[str, ActionSpec]]:
    definitions = getattr(section, "definitions", None)
    if definitions is not None:
        return definitions.items()
    if isinstance(section, Mapping):
        return section.items()
    raise TypeError(f"unsupported action catalog section: {type(section).__name__}")


def merge_action_catalog_sections(
    *sections: Mapping[str, ActionSpec] | object,
) -> dict[str, ActionSpec]:
    return merge_named_action_sections(
        "definitions",
        *[dict(_section_items(section)) for section in sections],
    )


def action_catalog_section_merge_contract() -> dict[str, object]:
    return {
        "contract_id": ACTION_CATALOG_SECTION_MERGE_CONTRACT_ID,
        "owner_surface": ACTION_CATALOG_SECTION_MERGE_OWNER_SURFACE,
        "duplicate_action_policy": "fail-closed",
        "hidden_internal_public_split_allowed": False,
        "wrapper_only_catalog_grouping_allowed": False,
        "public_contract": True,
    }


__all__ = [
    "ACTION_CATALOG_SECTION_MERGE_CONTRACT_ID",
    "ACTION_CATALOG_SECTION_MERGE_OWNER_SURFACE",
    "action_catalog_section_merge_contract",
    "merge_action_catalog_sections",
]
