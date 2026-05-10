from __future__ import annotations

import json
import re
from copy import deepcopy
from pathlib import Path
from typing import Any

from spt_issue_closure.models import CloseToolingConfig

ROOT = Path(__file__).resolve().parents[2]
CLOSE_CONFIG_SECTION = "close_spt_issues_from_checkboxes"
DEFAULT_CLOSE_CONFIG: dict[str, Any] = {
    "catalog_default": "tmp/reports/remaining_task_review_catalog.json",
    "task_id_prefix_default": "SPT-",
    "task_id_pattern": r"^\[(SPT-\d{4})\]",
}


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


def _expect_str(payload: dict[str, Any], key: str, context: str) -> str:
    value = payload.get(key)
    if not isinstance(value, str) or not value:
        raise ConfigError(f"{context} must define non-empty string key '{key}'")
    return value


def _resolve_root_path(raw_path: str) -> Path:
    path = Path(raw_path)
    if path.is_absolute():
        return path
    return ROOT / path


def load_close_tooling_config(config_path: Path | None) -> CloseToolingConfig:
    merged = deepcopy(DEFAULT_CLOSE_CONFIG)
    if config_path is not None:
        resolved_config = config_path if config_path.is_absolute() else ROOT / config_path
        override = _read_json_config(resolved_config, CLOSE_CONFIG_SECTION)
        merged = _deep_merge_dict(merged, override)

    context = f"configuration section '{CLOSE_CONFIG_SECTION}'"
    catalog_default = _resolve_root_path(_expect_str(merged, "catalog_default", context))
    task_id_prefix_default = _expect_str(merged, "task_id_prefix_default", context)
    task_id_pattern_raw = _expect_str(merged, "task_id_pattern", context)

    try:
        task_id_pattern = re.compile(task_id_pattern_raw)
    except re.error as exc:
        raise ConfigError(f"{context} key 'task_id_pattern' is not a valid regex: {exc}") from exc

    if task_id_pattern.groups < 1:
        raise ConfigError(
            f"{context} key 'task_id_pattern' must include a capture group for task ID extraction"
        )

    return CloseToolingConfig(
        catalog_default=catalog_default,
        task_id_prefix_default=task_id_prefix_default,
        task_id_pattern=task_id_pattern,
    )
