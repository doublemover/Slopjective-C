"""Top-level workflow action handler section groups."""

from __future__ import annotations

from collections.abc import Mapping

from scripts.objc3c_workflow.action_handlers_application import APPLICATION_ACTION_HANDLERS
from scripts.objc3c_workflow.action_handlers_core import CORE_ACTION_HANDLERS
from scripts.objc3c_workflow.action_handlers_developer_performance import (
    DEVELOPER_AND_PERFORMANCE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_reporting_release import (
    REPORTING_RELEASE_AND_SECURITY_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_tooling_test import (
    TOOLING_ECOSYSTEM_AND_TEST_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_spec import ActionHandler


ACTION_HANDLER_SECTION_GROUPS: tuple[Mapping[str, ActionHandler], ...] = (
    CORE_ACTION_HANDLERS,
    APPLICATION_ACTION_HANDLERS,
    DEVELOPER_AND_PERFORMANCE_ACTION_HANDLERS,
    REPORTING_RELEASE_AND_SECURITY_ACTION_HANDLERS,
    TOOLING_ECOSYSTEM_AND_TEST_ACTION_HANDLERS,
)


__all__ = ["ACTION_HANDLER_SECTION_GROUPS"]
