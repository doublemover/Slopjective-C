from __future__ import annotations

from typing import Any


def showcase_example_ids(examples: list[Any]) -> list[Any]:
    return [entry.get("id") for entry in examples if isinstance(entry, dict)]


def known_story_capabilities(examples: list[Any]) -> set[str]:
    return {
        capability
        for entry in examples
        if isinstance(entry, dict)
        for capability in entry.get("story_capabilities", [])
        if isinstance(capability, str)
    }


def validate_requested_ids(requested_ids: set[str], ids: list[Any]) -> str | None:
    if requested_ids:
        unknown_ids = sorted(requested_ids.difference(ids))
        if unknown_ids:
            return f"unknown showcase example ids: {', '.join(unknown_ids)}"
    return None


def validate_requested_capabilities(
    requested_capabilities: set[str],
    known_capabilities: set[str],
) -> str | None:
    if requested_capabilities:
        unknown_capabilities = sorted(requested_capabilities.difference(known_capabilities))
        if unknown_capabilities:
            return f"unknown showcase capabilities: {', '.join(unknown_capabilities)}"
    return None


def validate_showcase_entry(entry: Any) -> str | None:
    if not isinstance(entry, dict):
        return "example entry must be an object"
    example_id = entry.get("id")
    if not isinstance(example_id, str):
        return "example entry missing id/source"
    workspace_manifest = entry.get("workspace_manifest")
    if not isinstance(workspace_manifest, str):
        return f"example entry missing workspace_manifest for {example_id}"
    return None
