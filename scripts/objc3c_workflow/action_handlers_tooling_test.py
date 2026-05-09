"""Tooling, ecosystem, runtime, and test action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handler_sections import merge_action_handler_sections
from scripts.objc3c_workflow.action_handlers_native_package import (
    NATIVE_PACKAGE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_public_tests import PUBLIC_TEST_ACTION_HANDLERS
from scripts.objc3c_workflow.action_handlers_runtime_validation import (
    RUNTIME_VALIDATION_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_tooling_developer import (
    DEVELOPER_ECOSYSTEM_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_spec import ActionHandler

TOOLING_ECOSYSTEM_AND_TEST_ACTION_HANDLERS: dict[str, ActionHandler] = (
    merge_action_handler_sections(
        DEVELOPER_ECOSYSTEM_ACTION_HANDLERS,
        PUBLIC_TEST_ACTION_HANDLERS,
        RUNTIME_VALIDATION_ACTION_HANDLERS,
        NATIVE_PACKAGE_ACTION_HANDLERS,
    )
)
