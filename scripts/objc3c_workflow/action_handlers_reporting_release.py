"""Reporting, release-governance, stress, and security action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handler_sections import merge_action_handler_sections
from scripts.objc3c_workflow.action_handlers_reporting_conformance_stress import (
    REPORTING_CONFORMANCE_STRESS_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_reporting_public_performance import (
    REPORTING_PUBLIC_PERFORMANCE_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_reporting_release_channels import (
    REPORTING_RELEASE_CHANNEL_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_reporting_security import (
    REPORTING_SECURITY_HANDLERS,
)
from scripts.objc3c_workflow.action_spec import ActionHandler

REPORTING_RELEASE_AND_SECURITY_ACTION_HANDLERS: dict[str, ActionHandler] = (
    merge_action_handler_sections(
        REPORTING_CONFORMANCE_STRESS_HANDLERS,
        REPORTING_PUBLIC_PERFORMANCE_HANDLERS,
        REPORTING_RELEASE_CHANNEL_HANDLERS,
        REPORTING_SECURITY_HANDLERS,
    )
)
