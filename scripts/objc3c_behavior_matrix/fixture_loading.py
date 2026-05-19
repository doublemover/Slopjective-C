"""Behavior fixture selection."""

from __future__ import annotations

from typing import Sequence

from objc3c_tooling.behavior_fixtures import BehaviorFixture

from .errors import BehaviorMatrixFailure


def select_fixtures(
    fixtures: list[BehaviorFixture],
    *,
    phases: Sequence[str],
    limit: int,
) -> list[BehaviorFixture]:
    selected = [fixture for fixture in fixtures if not phases or fixture.owner_phase in phases]
    if limit < 0:
        raise BehaviorMatrixFailure("--limit must be non-negative")
    if limit:
        selected = selected[:limit]
    if not selected:
        raise BehaviorMatrixFailure("no behavior fixtures matched the requested selection")
    return selected
