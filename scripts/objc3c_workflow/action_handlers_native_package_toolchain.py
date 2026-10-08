"""Native runnable toolchain package handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import native_build_package

NATIVE_PACKAGE_TOOLCHAIN_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "package-runnable-toolchain": native_build_package.action_package_runnable_toolchain,
    "package-runnable-toolchain-asan": native_build_package.action_package_runnable_toolchain_asan,
    "package-runnable-toolchain-ubsan": native_build_package.action_package_runnable_toolchain_ubsan,
}

__all__ = ["NATIVE_PACKAGE_TOOLCHAIN_ACTION_HANDLERS"]
