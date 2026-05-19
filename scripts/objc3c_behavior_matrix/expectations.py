"""Expectation and assertion helpers for behavior fixture outputs."""

from __future__ import annotations

from typing import Sequence

from objc3c_tooling.behavior_fixtures import BehaviorFixture
from objc3c_tooling.subprocesses import bounded_text

from .errors import BehaviorMatrixFailure


def missing_tokens(text: str, tokens: Sequence[str]) -> list[str]:
    return [token for token in tokens if token not in text]


def assert_tokens(fixture: BehaviorFixture, text: str) -> None:
    missing = missing_tokens(text, fixture.required_tokens)
    if missing:
        raise BehaviorMatrixFailure(
            f"missing expected token(s) for {fixture.relative_source}: {', '.join(missing)}\n"
            f"{bounded_text(text)}"
        )
