"""Block/ARC capture legality assertions."""

from __future__ import annotations

from collections.abc import Mapping

from ...expectation_matching import expect
from .data import CaptureFixtureFacts, CaptureFixtureSpec
from .predicates import has_copy_dispose_profile, has_escape_profile


def expect_capture_fixture_set(
    facts_by_key: Mapping[str, CaptureFixtureFacts],
    specs: tuple[CaptureFixtureSpec, ...],
) -> None:
    for spec in specs:
        expect_capture_fixture_facts(facts_by_key[spec.key], spec)


def expect_capture_fixture_facts(
    facts: CaptureFixtureFacts,
    spec: CaptureFixtureSpec,
) -> None:
    expect(
        has_escape_profile(facts.escape_surface, spec.expected_profile),
        spec.escape_message,
    )
    expect(
        has_copy_dispose_profile(facts.copy_dispose_surface, spec.expected_profile),
        spec.copy_dispose_message,
    )


__all__ = [
    "expect_capture_fixture_facts",
    "expect_capture_fixture_set",
]
