"""Native package/proof handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handler_sections import merge_action_handler_sections
from scripts.objc3c_workflow.action_handlers_native_package_proof import (
    NATIVE_PACKAGE_PROOF_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_native_package_toolchain import (
    NATIVE_PACKAGE_TOOLCHAIN_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_spec import ActionHandler

NATIVE_PACKAGE_ACTION_HANDLERS: dict[str, ActionHandler] = merge_action_handler_sections(
    NATIVE_PACKAGE_TOOLCHAIN_ACTION_HANDLERS,
    NATIVE_PACKAGE_PROOF_ACTION_HANDLERS,
)

__all__ = ["NATIVE_PACKAGE_ACTION_HANDLERS"]
