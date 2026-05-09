"""Runtime acceptance action specs."""

from __future__ import annotations

from .actions.runtime_acceptance_routes import RUNTIME_ACCEPTANCE_ROUTES
from .action_spec import ActionSpec


def runtime_acceptance_action_specs() -> dict[str, ActionSpec]:
    return {
        action: ActionSpec(
            route.action,
            route.title,
            route.target,
            validation_tier=route.validation_tier,
            guarantee_owner=route.guarantee_owner,
        )
        for action, route in RUNTIME_ACCEPTANCE_ROUTES.items()
    }


RUNTIME_ACCEPTANCE_ACTION_SPECS: dict[str, ActionSpec] = runtime_acceptance_action_specs()


__all__ = ["RUNTIME_ACCEPTANCE_ACTION_SPECS", "runtime_acceptance_action_specs"]
