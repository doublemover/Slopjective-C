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


def action_category(action: str) -> str:
    category = action.split("-", 1)[0]
    if category not in CANONICAL_ACTION_CATEGORIES:
        raise ValueError(f"unknown workflow action category: {category}")
    return category


def action_category_contract_payload() -> dict[str, object]:
    return {
        "contract_id": ACTION_CATEGORY_CONTRACT_ID,
        "owner_surface": ACTION_CATEGORY_OWNER_SURFACE,
        "canonical_categories": list(CANONICAL_ACTION_CATEGORIES),
        "unknown_category_fallback_allowed": False,
        "public_contract": True,
    }


__all__ = [
    "ACTION_CATEGORY_CONTRACT_ID",
    "ACTION_CATEGORY_OWNER_SURFACE",
    "CANONICAL_ACTION_CATEGORIES",
    "action_category",
    "action_category_contract_payload",
]
