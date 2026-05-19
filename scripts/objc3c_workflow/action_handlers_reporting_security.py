"""Security-hardening handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions.release_governance_security_hardening import (
    SECURITY_HARDENING_ACTION_HANDLERS,
)

REPORTING_SECURITY_HANDLERS: dict[str, ActionHandler] = {
    **SECURITY_HARDENING_ACTION_HANDLERS,
}
