"""Top-level workflow action catalog section groups."""

from __future__ import annotations

from collections.abc import Mapping

from .action_catalog_application import APPLICATION_AND_ECOSYSTEM_ACTION_SPECS
from .action_catalog_core import CORE_ACTION_SPECS
from .action_catalog_developer_performance import DEVELOPER_AND_PERFORMANCE_ACTION_SPECS
from .action_catalog_reporting_release import STRESS_REPORTING_AND_RELEASE_ACTION_SPECS
from .action_catalog_tooling_test import TOOLING_AND_TEST_ACTION_SPECS
from .action_spec import ActionSpec


ACTION_CATALOG_SECTION_GROUPS: tuple[Mapping[str, ActionSpec], ...] = (
    CORE_ACTION_SPECS,
    APPLICATION_AND_ECOSYSTEM_ACTION_SPECS,
    DEVELOPER_AND_PERFORMANCE_ACTION_SPECS,
    STRESS_REPORTING_AND_RELEASE_ACTION_SPECS,
    TOOLING_AND_TEST_ACTION_SPECS,
)


__all__ = ["ACTION_CATALOG_SECTION_GROUPS"]
