"""Runtime runnable conformance action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import runtime_runnable_conformance

RUNTIME_RUNNABLE_CONFORMANCE_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "validate-block-arc-conformance": (
        runtime_runnable_conformance.action_validate_block_arc_conformance
    ),
    "validate-concurrency-conformance": (
        runtime_runnable_conformance.action_validate_concurrency_conformance
    ),
    "validate-object-model-conformance": (
        runtime_runnable_conformance.action_validate_object_model_conformance
    ),
    "validate-storage-reflection-conformance": (
        runtime_runnable_conformance.action_validate_storage_reflection_conformance
    ),
    "validate-error-conformance": (
        runtime_runnable_conformance.action_validate_error_conformance
    ),
    "validate-interop-conformance": (
        runtime_runnable_conformance.action_validate_interop_conformance
    ),
    "validate-metaprogramming-conformance": (
        runtime_runnable_conformance.action_validate_metaprogramming_conformance
    ),
    "validate-release-candidate-conformance": (
        runtime_runnable_conformance.action_validate_release_candidate_conformance
    ),
}


__all__ = ["RUNTIME_RUNNABLE_CONFORMANCE_ACTION_HANDLERS"]
