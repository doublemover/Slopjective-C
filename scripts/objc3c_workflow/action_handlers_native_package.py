"""Native package/proof handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import native_build

NATIVE_PACKAGE_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "package-runnable-toolchain": native_build.action_package_runnable_toolchain,
    "proof-objc3c": native_build.action_proof_objc3c,
}
