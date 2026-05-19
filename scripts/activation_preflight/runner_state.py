"""Activation preflight runner input and exit-state helpers."""

from __future__ import annotations

import argparse
from typing import Any, Sequence

from scripts.activation_preflight.payload_validation import (
    EXIT_GATE_CLOSED,
    EXIT_GATE_OPEN,
    EXIT_RUNNER_ERROR,
)
from scripts.activation_preflight.runner_paths import DEFAULT_ACTIONABLE_STATUSES


def parse_non_negative_int(raw: str) -> int:
    try:
        value = int(raw)
    except ValueError as exc:
        raise argparse.ArgumentTypeError("must be an integer") from exc
    if value < 0:
        raise argparse.ArgumentTypeError("must be >= 0")
    return value


def normalize_actionable_statuses(raw_values: Sequence[str] | None) -> tuple[str, ...]:
    if not raw_values:
        return DEFAULT_ACTIONABLE_STATUSES

    normalized: list[str] = []
    seen: set[str] = set()
    for raw in raw_values:
        status = raw.strip().lower()
        if not status:
            raise ValueError("actionable status filters must be non-empty")
        if status in seen:
            continue
        seen.add(status)
        normalized.append(status)
    if not normalized:
        raise ValueError("no actionable statuses were provided")
    return tuple(normalized)


def determine_final_exit(
    *,
    activation_payload: dict[str, Any] | None,
    spec_lint_exit_code: int,
    errors: Sequence[str],
) -> tuple[int, str]:
    if errors:
        return EXIT_RUNNER_ERROR, "runner-error"

    assert activation_payload is not None
    gate_open = bool(activation_payload["gate_open"])
    if gate_open:
        return EXIT_GATE_OPEN, "activation-open"

    if spec_lint_exit_code == 0:
        return EXIT_GATE_CLOSED, "ok"

    return EXIT_RUNNER_ERROR, "spec-lint-failed"


__all__ = [
    "determine_final_exit",
    "normalize_actionable_statuses",
    "parse_non_negative_int",
]
