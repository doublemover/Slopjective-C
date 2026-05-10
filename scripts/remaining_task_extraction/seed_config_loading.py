"""Load and construct remaining spec task seed configuration."""

from __future__ import annotations

import json
from copy import deepcopy
from pathlib import Path
from typing import Any

from remaining_task_extraction.seed_config_defaults import (
    DEFAULT_SEED_CONFIG,
    ROOT,
    SEED_CONFIG_SECTION,
)
from remaining_task_extraction.seed_config_models import ConfigError
from remaining_task_extraction.seed_config_models import SeedToolingConfig
from remaining_task_extraction.seed_config_paths import resolve_config_path
from remaining_task_extraction.seed_config_paths import resolve_root_path
from remaining_task_extraction.seed_config_validation import expect_dict
from remaining_task_extraction.seed_config_validation import expect_numeric
from remaining_task_extraction.seed_config_validation import expect_str
from remaining_task_extraction.seed_config_validation import normalize_conformance_milestone_by_tag
from remaining_task_extraction.seed_config_validation import normalize_label_defs
from remaining_task_extraction.seed_config_validation import normalize_planning_lane_by_issue
from remaining_task_extraction.seed_config_validation import normalize_required_lane_map


def deep_merge_dict(base: dict[str, Any], override: dict[str, Any]) -> dict[str, Any]:
    merged: dict[str, Any] = deepcopy(base)
    for key, value in override.items():
        if isinstance(value, dict) and isinstance(merged.get(key), dict):
            merged[key] = deep_merge_dict(merged[key], value)  # type: ignore[arg-type]
            continue
        merged[key] = value
    return merged


def read_json_config(path: Path, section: str) -> dict[str, Any]:
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


def load_seed_tooling_config(config_path: Path | None, *, root: Path = ROOT) -> SeedToolingConfig:
    merged = deepcopy(DEFAULT_SEED_CONFIG)
    if config_path is not None:
        resolved_config = resolve_config_path(config_path, root=root)
        override = read_json_config(resolved_config, SEED_CONFIG_SECTION)
        merged = deep_merge_dict(merged, override)

    context = f"configuration section '{SEED_CONFIG_SECTION}'"
    expected_task_count = expect_numeric(merged, "expected_task_count", context)
    if expected_task_count < 0 or expected_task_count != int(expected_task_count):
        raise ConfigError(f"{context} key 'expected_task_count' must be a non-negative integer")

    repo_default = expect_str(merged, "repo_default", context)
    catalog_md_default = resolve_root_path(
        expect_str(merged, "catalog_md_default", context),
        root=root,
    )
    catalog_json_default = resolve_root_path(
        expect_str(merged, "catalog_json_default", context),
        root=root,
    )
    sleep_seconds_default = expect_numeric(merged, "sleep_seconds_default", context)
    if sleep_seconds_default < 0:
        raise ConfigError(f"{context} key 'sleep_seconds_default' must be non-negative")

    lane_name_raw = expect_dict(merged, "lane_name", context)
    conformance_milestone_by_tag_raw = expect_dict(merged, "conformance_milestone_by_tag", context)
    lane_milestone_titles_raw = expect_dict(merged, "lane_milestone_titles", context)
    lane_milestone_due_on_raw = expect_dict(merged, "lane_milestone_due_on", context)
    label_defs_raw = expect_dict(merged, "label_defs", context)
    planning_lane_by_issue_raw = expect_dict(merged, "planning_lane_by_issue", context)

    lane_name = normalize_required_lane_map(lane_name_raw, key="lane_name", context=context)
    lane_milestone_titles = normalize_required_lane_map(
        lane_milestone_titles_raw,
        key="lane_milestone_titles",
        context=context,
    )
    lane_milestone_due_on = normalize_required_lane_map(
        lane_milestone_due_on_raw,
        key="lane_milestone_due_on",
        context=context,
    )
    conformance_milestone_by_tag = normalize_conformance_milestone_by_tag(
        conformance_milestone_by_tag_raw,
        context=context,
    )
    label_defs = normalize_label_defs(label_defs_raw, context=context)
    planning_lane_by_issue = normalize_planning_lane_by_issue(
        planning_lane_by_issue_raw,
        context=context,
    )

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
    "deep_merge_dict",
    "load_seed_tooling_config",
    "read_json_config",
]
