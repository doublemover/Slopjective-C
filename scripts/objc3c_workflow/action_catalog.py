"""Canonical objc3c workflow action catalog."""

from __future__ import annotations

from .action_catalog_application import APPLICATION_AND_ECOSYSTEM_ACTION_SPECS
from .action_catalog_core import CORE_ACTION_SPECS
from .action_catalog_developer_performance import DEVELOPER_AND_PERFORMANCE_ACTION_SPECS
from .action_catalog_reporting_release import STRESS_REPORTING_AND_RELEASE_ACTION_SPECS
from .action_catalog_sections import merge_action_catalog_sections
from .action_catalog_tooling_test import TOOLING_AND_TEST_ACTION_SPECS
from .action_spec import ActionSpec

ACTION_SPECS: dict[str, ActionSpec] = merge_action_catalog_sections(
    CORE_ACTION_SPECS,
    APPLICATION_AND_ECOSYSTEM_ACTION_SPECS,
    DEVELOPER_AND_PERFORMANCE_ACTION_SPECS,
    STRESS_REPORTING_AND_RELEASE_ACTION_SPECS,
    TOOLING_AND_TEST_ACTION_SPECS,
)
