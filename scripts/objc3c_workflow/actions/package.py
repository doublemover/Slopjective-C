"""Package action inventory."""

from __future__ import annotations

from .command_facades_inventory import category_group_action_names

CATEGORIES = ("package", "packaging")
CATEGORY_OWNER_CONTRACTS: dict[str, dict[str, object]] = {
    "package": {
        "action_surface_owner": "native-package-toolchain",
        "source_owner": "packaging-channels-source",
        "gate_owner": "packaging-channels-gate",
        "blocker_owner": "packaging-channels-blockers",
        "wrapper_only_action_surface_allowed": False,
    },
    "packaging": {
        "action_surface_owner": "packaging-channels",
        "source_owner": "packaging-channels-source",
        "gate_owner": "packaging-channels-gate",
        "blocker_owner": "packaging-channels-blockers",
        "wrapper_only_action_surface_allowed": False,
    },
}


def action_names() -> list[str]:
    return category_group_action_names(CATEGORIES)


def category_owner_contracts() -> dict[str, dict[str, object]]:
    return {
        category: dict(contract)
        for category, contract in CATEGORY_OWNER_CONTRACTS.items()
    }
