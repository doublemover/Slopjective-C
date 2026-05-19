"""Probe result parsing and assertions for runnable concurrency validation."""

from __future__ import annotations

from objc3c_tooling.probe_output import parse_key_value_output

from .assertions import expect_payload_fields
from .catalog import ConcurrencyScenario


def parse_probe_payload(result: object, scenario: ConcurrencyScenario) -> dict[str, object]:
    return parse_key_value_output(result, scenario.probe_label)


def assert_probe_payload(scenario: ConcurrencyScenario, payload: dict[str, object]) -> None:
    expect_payload_fields(
        payload,
        scenario.expected_payload,
        context=scenario.probe_label,
    )


__all__ = ["assert_probe_payload", "parse_probe_payload"]
