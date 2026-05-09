"""Runtime runnable conformance action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

RUNTIME_RUNNABLE_CONFORMANCE_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-block-arc-conformance": ActionSpec("validate-block-arc-conformance", "validate runnable block/ARC conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_block_arc_conformance.py", validation_tier="full", guarantee_owner="integrated block/ARC conformance over the live runtime architecture workflow"),
    "validate-concurrency-conformance": ActionSpec("validate-concurrency-conformance", "validate runnable concurrency conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_concurrency_conformance.py", validation_tier="full", guarantee_owner="integrated async/task/executor/actor conformance over the live runtime architecture workflow"),
    "validate-object-model-conformance": ActionSpec("validate-object-model-conformance", "validate runnable object-model conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_object_model_conformance.py", validation_tier="full", guarantee_owner="integrated object-model conformance over the live runtime architecture workflow"),
    "validate-storage-reflection-conformance": ActionSpec("validate-storage-reflection-conformance", "validate runnable storage/reflection conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_storage_reflection_conformance.py", validation_tier="full", guarantee_owner="integrated storage/accessor/reflection conformance over the live runtime architecture workflow"),
    "validate-error-conformance": ActionSpec("validate-error-conformance", "validate runnable error conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_error_conformance.py", validation_tier="full", guarantee_owner="integrated error conformance over the live runtime architecture workflow"),
    "validate-interop-conformance": ActionSpec("validate-interop-conformance", "validate runnable mixed-module and interop conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_interop_conformance.py", validation_tier="full", guarantee_owner="integrated mixed-module runtime packaging and interop conformance over the live runtime architecture workflow"),
    "validate-metaprogramming-conformance": ActionSpec("validate-metaprogramming-conformance", "validate runnable metaprogramming conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_metaprogramming_conformance.py", validation_tier="full", guarantee_owner="integrated metaprogramming conformance over the live runtime architecture workflow"),
    "validate-release-candidate-conformance": ActionSpec("validate-release-candidate-conformance", "validate runnable release-candidate conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_release_candidate_conformance.py", validation_tier="full", guarantee_owner="integrated public-claims strict-profile and release-candidate conformance over the live runtime architecture workflow"),
}


__all__ = ["RUNTIME_RUNNABLE_CONFORMANCE_ACTION_SPECS"]
