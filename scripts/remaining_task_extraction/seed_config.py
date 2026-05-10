"""Configuration loading facade for remaining spec task seeding."""

from __future__ import annotations

from remaining_task_extraction.seed_config_defaults import DEFAULT_SEED_CONFIG
from remaining_task_extraction.seed_config_defaults import ROOT
from remaining_task_extraction.seed_config_defaults import SEED_CONFIG_SECTION
from remaining_task_extraction.seed_config_loading import deep_merge_dict as _deep_merge_dict
from remaining_task_extraction.seed_config_loading import load_seed_tooling_config
from remaining_task_extraction.seed_config_loading import read_json_config as _read_json_config
from remaining_task_extraction.seed_config_models import ConfigError
from remaining_task_extraction.seed_config_models import SeedToolingConfig
from remaining_task_extraction.seed_config_paths import resolve_generated_on
from remaining_task_extraction.seed_config_paths import resolve_root_path as _resolve_root_path
from remaining_task_extraction.seed_config_validation import ISO_DATE_PATTERN
from remaining_task_extraction.seed_config_validation import expect_dict as _expect_dict
from remaining_task_extraction.seed_config_validation import expect_numeric as _expect_numeric
from remaining_task_extraction.seed_config_validation import expect_str as _expect_str
from remaining_task_extraction.seed_config_validation import parse_iso_date

__all__ = [
    "ConfigError",
    "SeedToolingConfig",
    "load_seed_tooling_config",
    "resolve_generated_on",
]
