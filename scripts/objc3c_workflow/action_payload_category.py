"""Action category fields for workflow payloads."""

from __future__ import annotations

ACTION_CATEGORY_CONTRACT_ID = "objc3c-workflow-action-category-v1"
ACTION_CATEGORY_OWNER_SURFACE = "scripts/objc3c_workflow/action_payload_category.py"
CANONICAL_ACTION_CATEGORIES: tuple[str, ...] = (
    "benchmark",
    "build",
    "check",
    "compile",
    "format",
    "inspect",
    "lint",
    "materialize",
    "package",
    "proof",
    "publish",
    "test",
    "validate",
)
CATEGORY_ALIASES: dict[str, str] = {}
RETIRED_PUBLIC_COMMAND_ALIASES: tuple[str, ...] = (
    "lint-default",
)


def action_category(action: str) -> str:
    if action in RETIRED_PUBLIC_COMMAND_ALIASES:
        raise ValueError(f"retired public workflow action alias: {action}")
    category = action.split("-", 1)[0]
    if category not in CANONICAL_ACTION_CATEGORIES:
        raise ValueError(f"unknown workflow action category: {category}")
    return category


def action_category_contract_payload() -> dict[str, object]:
    return {
        "contract_id": ACTION_CATEGORY_CONTRACT_ID,
        "owner_surface": ACTION_CATEGORY_OWNER_SURFACE,
        "canonical_categories": list(CANONICAL_ACTION_CATEGORIES),
        "category_aliases": dict(CATEGORY_ALIASES),
        "retired_public_command_aliases": list(RETIRED_PUBLIC_COMMAND_ALIASES),
        "stale_category_aliases_allowed": False,
        "unknown_category_fallback_allowed": False,
        "public_command_aliases_allowed": False,
        "public_contract": True,
    }


__all__ = [
    "ACTION_CATEGORY_CONTRACT_ID",
    "ACTION_CATEGORY_OWNER_SURFACE",
    "CANONICAL_ACTION_CATEGORIES",
    "CATEGORY_ALIASES",
    "RETIRED_PUBLIC_COMMAND_ALIASES",
    "action_category",
    "action_category_contract_payload",
]
