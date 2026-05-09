"""Runtime validation handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import runtime_runnable_conformance
from scripts.objc3c_workflow.actions import runtime_runnable_e2e
from scripts.objc3c_workflow.actions import runtime_test_acceptance

RUNTIME_VALIDATION_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "test-runtime-acceptance": runtime_test_acceptance.action_test_runtime_acceptance,
    "test-runtime-acceptance-fast": runtime_test_acceptance.action_test_runtime_acceptance_fast,
    "test-runtime-acceptance-diagnostics": (
        runtime_test_acceptance.action_test_runtime_acceptance_diagnostics
    ),
    "test-runtime-acceptance-cross-module": (
        runtime_test_acceptance.action_test_runtime_acceptance_cross_module
    ),
    "test-runtime-acceptance-block-arc": (
        runtime_test_acceptance.action_test_runtime_acceptance_block_arc
    ),
    "test-runtime-acceptance-concurrency": (
        runtime_test_acceptance.action_test_runtime_acceptance_concurrency
    ),
    "proof-runtime-architecture": runtime_test_acceptance.action_proof_runtime_architecture,
    "validate-runtime-architecture": (
        runtime_test_acceptance.action_validate_runtime_architecture
    ),
    "validate-runnable-bootstrap": runtime_runnable_e2e.action_validate_runnable_bootstrap,
    "validate-block-arc-conformance": (
        runtime_runnable_conformance.action_validate_block_arc_conformance
    ),
    "validate-runnable-block-arc": runtime_runnable_e2e.action_validate_runnable_block_arc,
    "validate-concurrency-conformance": (
        runtime_runnable_conformance.action_validate_concurrency_conformance
    ),
    "validate-runnable-concurrency": (
        runtime_runnable_e2e.action_validate_runnable_concurrency
    ),
    "validate-object-model-conformance": (
        runtime_runnable_conformance.action_validate_object_model_conformance
    ),
    "validate-storage-reflection-conformance": (
        runtime_runnable_conformance.action_validate_storage_reflection_conformance
    ),
    "validate-runnable-object-model": (
        runtime_runnable_e2e.action_validate_runnable_object_model
    ),
    "validate-runnable-storage-reflection": (
        runtime_runnable_e2e.action_validate_runnable_storage_reflection
    ),
    "validate-error-conformance": (
        runtime_runnable_conformance.action_validate_error_conformance
    ),
    "validate-runnable-error": runtime_runnable_e2e.action_validate_runnable_error,
    "validate-interop-conformance": (
        runtime_runnable_conformance.action_validate_interop_conformance
    ),
    "validate-runnable-interop": runtime_runnable_e2e.action_validate_runnable_interop,
    "validate-metaprogramming-conformance": (
        runtime_runnable_conformance.action_validate_metaprogramming_conformance
    ),
    "validate-runnable-metaprogramming": (
        runtime_runnable_e2e.action_validate_runnable_metaprogramming
    ),
    "validate-release-candidate-conformance": (
        runtime_runnable_conformance.action_validate_release_candidate_conformance
    ),
    "validate-runnable-release-candidate": (
        runtime_runnable_e2e.action_validate_runnable_release_candidate
    ),
}
