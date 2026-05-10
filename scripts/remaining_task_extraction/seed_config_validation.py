"""Validation and normalization helpers for seed configuration payloads."""

from __future__ import annotations

import re
from datetime import date
from typing import Any

from remaining_task_extraction.seed_config_models import ConfigError

ISO_DATE_PATTERN = re.compile(r"^\d{4}-\d{2}-\d{2}$")
REQUIRED_LANES = {"A", "B", "C", "D"}


def expect_dict(
    payload: dict[str, Any],
    key: str,
    context: str,
) -> dict[str, Any]:
    value = payload.get(key)
    if not isinstance(value, dict):
        raise ConfigError(f"{context} must define object key '{key}'")
    return value


def expect_str(
    payload: dict[str, Any],
    key: str,
    context: str,
) -> str:
    value = payload.get(key)
    if not isinstance(value, str) or not value:
        raise ConfigError(f"{context} must define non-empty string key '{key}'")
    return value


def expect_numeric(
    payload: dict[str, Any],
    key: str,
    context: str,
) -> float:
    value = payload.get(key)
    if not isinstance(value, (int, float)):
        raise ConfigError(f"{context} must define numeric key '{key}'")
    return float(value)


def parse_iso_date(raw_value: str, *, context: str) -> date:
    if not ISO_DATE_PATTERN.fullmatch(raw_value):
        raise ConfigError(f"{context} must use YYYY-MM-DD format (found '{raw_value}')")
    try:
        return date.fromisoformat(raw_value)
    except ValueError as exc:
        raise ConfigError(f"{context} must use a valid calendar date (found '{raw_value}')") from exc


def normalize_required_lane_map(
    raw_values: dict[str, Any],
    *,
    key: str,
    context: str,
) -> dict[str, str]:
    values = {
        str(raw_key): str(raw_value)
        for raw_key, raw_value in raw_values.items()
        if isinstance(raw_value, str)
    }
    if set(values) != REQUIRED_LANES:
        raise ConfigError(f"{context} key '{key}' must define exactly lanes A, B, C, D")
    return values


def normalize_conformance_milestone_by_tag(
    raw_values: dict[str, Any],
    *,
    context: str,
) -> dict[str, str]:
    values = {
        str(raw_key): str(raw_value)
        for raw_key, raw_value in raw_values.items()
        if isinstance(raw_value, str)
    }
    if not values:
        raise ConfigError(f"{context} key 'conformance_milestone_by_tag' must not be empty")
    return values


def normalize_label_defs(
    raw_values: dict[str, Any],
    *,
    context: str,
) -> dict[str, tuple[str, str]]:
    label_defs: dict[str, tuple[str, str]] = {}
    for name, raw_entry in raw_values.items():
        if not isinstance(name, str):
            raise ConfigError(f"{context} key 'label_defs' must use string label names")
        if not isinstance(raw_entry, dict):
            raise ConfigError(f"{context} label_defs['{name}'] must be an object")
        color = raw_entry.get("color")
        description = raw_entry.get("description")
        if not isinstance(color, str) or not color:
            raise ConfigError(f"{context} label_defs['{name}'].color must be a non-empty string")
        if not isinstance(description, str) or not description:
            raise ConfigError(
                f"{context} label_defs['{name}'].description must be a non-empty string"
            )
        label_defs[name] = (color, description)
    if not label_defs:
        raise ConfigError(f"{context} key 'label_defs' must not be empty")
    return label_defs


def normalize_planning_lane_by_issue(
    raw_values: dict[str, Any],
    *,
    context: str,
) -> dict[int, str]:
    planning_lane_by_issue: dict[int, str] = {}
    for issue_number, lane in raw_values.items():
        if not isinstance(issue_number, str) or not issue_number.isdigit():
            raise ConfigError(
                f"{context} planning_lane_by_issue keys must be numeric strings (found '{issue_number}')"
            )
        if lane not in REQUIRED_LANES:
            raise ConfigError(
                f"{context} planning_lane_by_issue['{issue_number}'] must be one of A, B, C, D"
            )
        planning_lane_by_issue[int(issue_number)] = lane
    return planning_lane_by_issue


__all__ = [
    "ISO_DATE_PATTERN",
    "REQUIRED_LANES",
    "expect_dict",
    "expect_numeric",
    "expect_str",
    "normalize_conformance_milestone_by_tag",
    "normalize_label_defs",
    "normalize_planning_lane_by_issue",
    "normalize_required_lane_map",
    "parse_iso_date",
]
