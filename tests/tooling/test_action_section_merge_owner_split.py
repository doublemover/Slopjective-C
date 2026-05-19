from __future__ import annotations

from scripts.objc3c_workflow.action_section_merge import (
    DuplicateActionSection,
    duplicate_action_message,
    duplicate_action_names,
    merge_named_action_sections,
)


def test_action_section_merge_tracks_duplicate_owners() -> None:
    duplicates = [
        DuplicateActionSection(
            action="lint",
            first_section_index=0,
            duplicate_section_index=2,
        ),
        DuplicateActionSection(
            action="lint",
            first_section_index=0,
            duplicate_section_index=3,
        ),
    ]

    assert duplicate_action_names(duplicates) == ["lint"]
    assert duplicate_action_message("handlers", duplicates) == (
        "duplicate workflow action handlers: lint"
    )


def test_action_section_merge_rejects_duplicate_actions_with_kind() -> None:
    try:
        merge_named_action_sections(
            "definitions",
            {"lint": object()},
            {"build": object()},
            {"lint": object()},
        )
    except ValueError as exc:
        assert str(exc) == "duplicate workflow action definitions: lint"
    else:
        raise AssertionError("duplicate action sections should fail closed")
