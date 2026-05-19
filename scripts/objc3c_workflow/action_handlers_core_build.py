"""Core build and compile handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handlers_defaults import action_build_default
from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import native_build

CORE_BUILD_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "build-default": action_build_default,
    "build-native-binaries": native_build.action_build_native_binaries,
    "build-native-contracts": native_build.action_build_native_contracts,
    "build-native-full": native_build.action_build_native_full,
    "build-native-reconfigure": native_build.action_build_native_reconfigure,
    "compile-objc3c": native_build.action_compile_objc3c,
}
