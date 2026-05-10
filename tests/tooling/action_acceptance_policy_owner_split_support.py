from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionSpec


def strict_action_spec() -> ActionSpec:
    return ActionSpec("lint", "Core", "Lint", "npm run objc3c -- lint")


def passthrough_action_spec() -> ActionSpec:
    return ActionSpec(
        "compile-objc3c",
        "Compiler",
        "Compile",
        "npm run objc3c -- compile-objc3c -- <source>",
        pass_through_args=True,
    )
