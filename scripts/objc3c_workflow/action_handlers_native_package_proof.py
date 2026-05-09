"""Native compile proof handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import native_build

NATIVE_PACKAGE_PROOF_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "proof-objc3c": native_build.action_proof_objc3c,
}

__all__ = ["NATIVE_PACKAGE_PROOF_ACTION_HANDLERS"]
