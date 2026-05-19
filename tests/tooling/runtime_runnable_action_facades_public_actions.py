from runtime_runnable_action_facades_support import (
    runtime_runnable_conformance,
    runtime_runnable_e2e,
    workflow_action_module,
)


def runtime_runnable_facades_preserve_public_actions() -> None:
    assert (
        runtime_runnable_conformance.action_validate_block_arc_conformance
        is workflow_action_module(
            "runtime_runnable_block_arc"
        ).action_validate_block_arc_conformance
    )
    assert (
        runtime_runnable_conformance.action_validate_error_conformance
        is workflow_action_module(
            "runtime_runnable_error"
        ).action_validate_error_conformance
    )
    assert (
        runtime_runnable_e2e.action_validate_runnable_interop
        is workflow_action_module(
            "runtime_runnable_interop"
        ).action_validate_runnable_interop
    )
    assert (
        runtime_runnable_e2e.action_validate_runnable_release_candidate
        is workflow_action_module(
            "runtime_runnable_release_candidate"
        ).action_validate_runnable_release_candidate
    )
