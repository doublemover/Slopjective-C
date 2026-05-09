"""Adoption-legibility publication workflow actions."""

from __future__ import annotations

from .ecosystem_publication_contracts import (
    ADOPTION_LEGIBILITY_INTEGRATION_PY,
    ADOPTION_LEGIBILITY_PUBLICATION_PY,
)
from .ecosystem_publication_runner import run_publication_action


def action_validate_adoption_legibility(_: list[str]) -> int:
    return run_publication_action("validate-adoption-legibility")


def action_publish_adoption_legibility(_: list[str]) -> int:
    return run_publication_action("publish-adoption-legibility")


__all__ = [
    "ADOPTION_LEGIBILITY_INTEGRATION_PY",
    "ADOPTION_LEGIBILITY_PUBLICATION_PY",
    "action_publish_adoption_legibility",
    "action_validate_adoption_legibility",
]
