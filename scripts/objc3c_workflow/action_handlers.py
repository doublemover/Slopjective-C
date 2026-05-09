"""Action handler registry for the objc3c workflow CLI."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handler_sections import merge_action_handler_sections
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

ACTION_HANDLERS: dict[str, ActionHandler] = merge_action_handler_sections(
    CORE_ACTION_HANDLERS,
    APPLICATION_ACTION_HANDLERS,
    DEVELOPER_AND_PERFORMANCE_ACTION_HANDLERS,
    REPORTING_RELEASE_AND_SECURITY_ACTION_HANDLERS,
    TOOLING_ECOSYSTEM_AND_TEST_ACTION_HANDLERS,
)
