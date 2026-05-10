"""Configuration loading for remaining spec task seeding."""

from __future__ import annotations

import json
import os
import re
from copy import deepcopy
from dataclasses import dataclass
from datetime import date, datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
SEED_CONFIG_SECTION = "seed_remaining_spec_tasks"
ISO_DATE_PATTERN = re.compile(r"^\d{4}-\d{2}-\d{2}$")

DEFAULT_SEED_CONFIG: dict[str, Any] = {
    "expected_task_count": 510,
    "repo_default": "doublemover/Slopjective-C",
    "catalog_md_default": "tmp/reports/remaining_task_review_catalog.md",
    "catalog_json_default": "tmp/reports/remaining_task_review_catalog.json",
    "sleep_seconds_default": 0.35,
    "lane_name": {
        "A": "Normative Closure",
        "B": "Implementation & Tooling",
        "C": "Governance & Ecosystem",
        "D": "Program Control & Release",
    },
    "conformance_milestone_by_tag": {
        "[CORE]": "Conformance: Core (E)",
        "[STRICT]": "Conformance: Strict (E)",
        "[CONC]": "Conformance: Strict Concurrency (E)",
        "[SYSTEM]": "Conformance: Strict System (E)",
        "[OPT-META]": "Conformance: Optional Metaprogramming (E)",
        "[OPT-CXX]": "Conformance: Optional Interop (E)",
        "[OPT-SWIFT]": "Conformance: Optional Interop (E)",
    },
    "lane_milestone_titles": {
        "A": "v0.12 Lane A - Normative Closure",
        "B": "v0.12 Lane B - Implementation & Tooling",
        "C": "v0.12 Lane C - Governance & Ecosystem",
        "D": "v0.12 Lane D - Program Control & Release",
    },
    "lane_milestone_due_on": {
        "A": "2026-04-15T00:00:00Z",
        "B": "2026-05-15T00:00:00Z",
        "C": "2026-05-01T00:00:00Z",
        "D": "2026-05-30T00:00:00Z",
    },
    "label_defs": {
        "lane:A": {"color": "0052CC", "description": "Parallel lane A: normative closure"},
        "lane:B": {"color": "1D76DB", "description": "Parallel lane B: implementation/tooling"},
        "lane:C": {"color": "0E8A16", "description": "Parallel lane C: governance/ecosystem"},
        "lane:D": {"color": "5319E7", "description": "Parallel lane D: program control/release"},
        "source:conformance-checklist": {
            "color": "D93F0B",
            "description": "Task sourced from conformance profile checklist",
        },
        "source:release-evidence": {
            "color": "FBCA04",
            "description": "Task sourced from release evidence checklist",
        },
        "source:planning-checklist": {
            "color": "BFDADC",
            "description": "Task sourced from planning package checklist",
        },
        "parallelizable": {"color": "C2E0C6", "description": "Can run in parallel lane scheduling"},
    },
    "planning_lane_by_issue": {
        "111": "D",
        "134": "A",
        "137": "A",
        "138": "C",
        "140": "B",
        "142": "D",
        "143": "A",
        "144": "A",
        "145": "A",
        "146": "C",
        "149": "A",
        "150": "B",
        "151": "B",
        "152": "C",
        "153": "C",
        "155": "B",
        "156": "B",
        "157": "B",
        "158": "B",
        "159": "D",
        "161": "B",
        "162": "B",
        "163": "B",
        "164": "C",
        "165": "D",
        "167": "B",
        "168": "C",
        "169": "C",
        "170": "C",
        "171": "D",
        "173": "B",
        "174": "C",
        "175": "C",
        "176": "C",
        "177": "D",
        "179": "B",
        "180": "C",
        "181": "C",
        "182": "C",
        "183": "D",
        "185": "D",
        "186": "D",
        "187": "D",
        "188": "D",
        "189": "D",
        "190": "D",
        "191": "D",
    },
}


@dataclass(frozen=True)
class SeedToolingConfig:
    expected_task_count: int
    repo_default: str
    catalog_md_default: Path
    catalog_json_default: Path
    sleep_seconds_default: float
    lane_name: dict[str, str]
    conformance_milestone_by_tag: dict[str, str]
    lane_milestone_titles: dict[str, str]
    lane_milestone_due_on: dict[str, str]
    label_defs: dict[str, tuple[str, str]]
    planning_lane_by_issue: dict[int, str]


class ConfigError(RuntimeError):
    pass


def _deep_merge_dict(base: dict[str, Any], override: dict[str, Any]) -> dict[str, Any]:
    merged: dict[str, Any] = deepcopy(base)
    for key, value in override.items():
        if isinstance(value, dict) and isinstance(merged.get(key), dict):
            merged[key] = _deep_merge_dict(merged[key], value)  # type: ignore[arg-type]
            continue
        merged[key] = value
    return merged


def _read_json_config(path: Path, section: str) -> dict[str, Any]:
    if not path.exists():
        raise ConfigError(f"config file not found: {path.as_posix()}")

    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except OSError as exc:
        raise ConfigError(f"unable to read config file '{path.as_posix()}': {exc}") from exc
    except json.JSONDecodeError as exc:
        raise ConfigError(f"invalid JSON in config file '{path.as_posix()}': {exc}") from exc

    if not isinstance(payload, dict):
        raise ConfigError(
            f"invalid config file '{path.as_posix()}': top-level JSON must be an object"
        )

    section_payload = payload.get(section)
    if not isinstance(section_payload, dict):
        raise ConfigError(
            f"invalid config file '{path.as_posix()}': missing object section '{section}'"
        )

    return section_payload


def _expect_dict(
    payload: dict[str, Any],
    key: str,
    context: str,
) -> dict[str, Any]:
    value = payload.get(key)
    if not isinstance(value, dict):
        raise ConfigError(f"{context} must define object key '{key}'")
    return value


def _expect_str(
    payload: dict[str, Any],
    key: str,
    context: str,
) -> str:
    value = payload.get(key)
    if not isinstance(value, str) or not value:
        raise ConfigError(f"{context} must define non-empty string key '{key}'")
    return value


def _expect_numeric(
    payload: dict[str, Any],
    key: str,
    context: str,
) -> float:
    value = payload.get(key)
    if not isinstance(value, (int, float)):
        raise ConfigError(f"{context} must define numeric key '{key}'")
    return float(value)


def _resolve_root_path(raw_path: str, *, root: Path) -> Path:
    path = Path(raw_path)
    if path.is_absolute():
        return path
    return root / path


def parse_iso_date(raw_value: str, *, context: str) -> date:
    if not ISO_DATE_PATTERN.fullmatch(raw_value):
        raise ConfigError(f"{context} must use YYYY-MM-DD format (found '{raw_value}')")
    try:
        return date.fromisoformat(raw_value)
    except ValueError as exc:
        raise ConfigError(f"{context} must use a valid calendar date (found '{raw_value}')") from exc


def resolve_generated_on(raw_generated_on: str | None) -> date:
    if raw_generated_on is not None:
        return parse_iso_date(raw_generated_on, context="--generated-on")

    raw_epoch = os.environ.get("SOURCE_DATE_EPOCH")
    if raw_epoch is None or not raw_epoch.strip():
        return date.today()

    try:
        epoch_seconds = int(raw_epoch)
    except ValueError as exc:
        raise ConfigError(f"SOURCE_DATE_EPOCH must be an integer Unix timestamp (found '{raw_epoch}')") from exc

    if epoch_seconds < 0:
        raise ConfigError("SOURCE_DATE_EPOCH must be a non-negative Unix timestamp")

    try:
        return datetime.fromtimestamp(epoch_seconds, tz=timezone.utc).date()
    except (OverflowError, OSError, ValueError) as exc:
        raise ConfigError(f"SOURCE_DATE_EPOCH is out of supported range (found '{raw_epoch}')") from exc


def load_seed_tooling_config(config_path: Path | None, *, root: Path = ROOT) -> SeedToolingConfig:
    merged = deepcopy(DEFAULT_SEED_CONFIG)
    if config_path is not None:
        resolved_config = config_path if config_path.is_absolute() else root / config_path
        override = _read_json_config(resolved_config, SEED_CONFIG_SECTION)
        merged = _deep_merge_dict(merged, override)

    context = f"configuration section '{SEED_CONFIG_SECTION}'"
    expected_task_count = _expect_numeric(merged, "expected_task_count", context)
    if expected_task_count < 0 or expected_task_count != int(expected_task_count):
        raise ConfigError(f"{context} key 'expected_task_count' must be a non-negative integer")

    repo_default = _expect_str(merged, "repo_default", context)
    catalog_md_default = _resolve_root_path(
        _expect_str(merged, "catalog_md_default", context),
        root=root,
    )
    catalog_json_default = _resolve_root_path(
        _expect_str(merged, "catalog_json_default", context),
        root=root,
    )
    sleep_seconds_default = _expect_numeric(merged, "sleep_seconds_default", context)
    if sleep_seconds_default < 0:
        raise ConfigError(f"{context} key 'sleep_seconds_default' must be non-negative")

    lane_name_raw = _expect_dict(merged, "lane_name", context)
    conformance_milestone_by_tag_raw = _expect_dict(merged, "conformance_milestone_by_tag", context)
    lane_milestone_titles_raw = _expect_dict(merged, "lane_milestone_titles", context)
    lane_milestone_due_on_raw = _expect_dict(merged, "lane_milestone_due_on", context)
    label_defs_raw = _expect_dict(merged, "label_defs", context)
    planning_lane_by_issue_raw = _expect_dict(merged, "planning_lane_by_issue", context)

    required_lanes = {"A", "B", "C", "D"}

    lane_name = {str(key): str(value) for key, value in lane_name_raw.items() if isinstance(value, str)}
    if set(lane_name) != required_lanes:
        raise ConfigError(f"{context} key 'lane_name' must define exactly lanes A, B, C, D")

    lane_milestone_titles = {
        str(key): str(value)
        for key, value in lane_milestone_titles_raw.items()
        if isinstance(value, str)
    }
    if set(lane_milestone_titles) != required_lanes:
        raise ConfigError(
            f"{context} key 'lane_milestone_titles' must define exactly lanes A, B, C, D"
        )

    lane_milestone_due_on = {
        str(key): str(value)
        for key, value in lane_milestone_due_on_raw.items()
        if isinstance(value, str)
    }
    if set(lane_milestone_due_on) != required_lanes:
        raise ConfigError(
            f"{context} key 'lane_milestone_due_on' must define exactly lanes A, B, C, D"
        )

    conformance_milestone_by_tag = {
        str(key): str(value)
        for key, value in conformance_milestone_by_tag_raw.items()
        if isinstance(value, str)
    }
    if not conformance_milestone_by_tag:
        raise ConfigError(f"{context} key 'conformance_milestone_by_tag' must not be empty")

    label_defs: dict[str, tuple[str, str]] = {}
    for name, raw_entry in label_defs_raw.items():
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

    planning_lane_by_issue: dict[int, str] = {}
    for issue_number, lane in planning_lane_by_issue_raw.items():
        if not isinstance(issue_number, str) or not issue_number.isdigit():
            raise ConfigError(
                f"{context} planning_lane_by_issue keys must be numeric strings (found '{issue_number}')"
            )
        if lane not in required_lanes:
            raise ConfigError(
                f"{context} planning_lane_by_issue['{issue_number}'] must be one of A, B, C, D"
            )
        planning_lane_by_issue[int(issue_number)] = lane

    return SeedToolingConfig(
        expected_task_count=int(expected_task_count),
        repo_default=repo_default,
        catalog_md_default=catalog_md_default,
        catalog_json_default=catalog_json_default,
        sleep_seconds_default=sleep_seconds_default,
        lane_name=lane_name,
        conformance_milestone_by_tag=conformance_milestone_by_tag,
        lane_milestone_titles=lane_milestone_titles,
        lane_milestone_due_on=lane_milestone_due_on,
        label_defs=label_defs,
        planning_lane_by_issue=planning_lane_by_issue,
    )


__all__ = [
    "ConfigError",
    "SeedToolingConfig",
    "load_seed_tooling_config",
    "resolve_generated_on",
]
