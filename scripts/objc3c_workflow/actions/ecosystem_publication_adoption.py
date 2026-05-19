"""Adoption-legibility publication workflow actions."""

from __future__ import annotations

from .ecosystem_publication_contracts import (
    ADOPTION_LEGIBILITY_INTEGRATION_PY,
    ADOPTION_LEGIBILITY_PUBLICATION_PY,
)
from .ecosystem_publication_owner_contracts import (
    require_ecosystem_publication_owner_contract,
)
from .ecosystem_publication_runner import run_publication_action

ADOPTION_LEGIBILITY_VALIDATE_ACTION = "validate-adoption-legibility"
ADOPTION_LEGIBILITY_PUBLISH_ACTION = "publish-adoption-legibility"
ADOPTION_LEGIBILITY_PUBLIC_ACTIONS = (
    ADOPTION_LEGIBILITY_VALIDATE_ACTION,
    ADOPTION_LEGIBILITY_PUBLISH_ACTION,
)


def action_validate_adoption_legibility(_: list[str]) -> int:
    require_ecosystem_publication_owner_contract(ADOPTION_LEGIBILITY_VALIDATE_ACTION)
    return run_publication_action(ADOPTION_LEGIBILITY_VALIDATE_ACTION)


def action_publish_adoption_legibility(_: list[str]) -> int:
    require_ecosystem_publication_owner_contract(ADOPTION_LEGIBILITY_PUBLISH_ACTION)
    return run_publication_action(ADOPTION_LEGIBILITY_PUBLISH_ACTION)


__all__ = [
    "ADOPTION_LEGIBILITY_INTEGRATION_PY",
    "ADOPTION_LEGIBILITY_PUBLIC_ACTIONS",
    "ADOPTION_LEGIBILITY_PUBLICATION_PY",
    "ADOPTION_LEGIBILITY_PUBLISH_ACTION",
    "ADOPTION_LEGIBILITY_VALIDATE_ACTION",
    "action_publish_adoption_legibility",
    "action_validate_adoption_legibility",
]
