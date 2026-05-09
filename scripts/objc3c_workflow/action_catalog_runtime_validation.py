"""Runtime validation action specs."""

from __future__ import annotations

from .actions.runtime_acceptance_routes import RUNTIME_ACCEPTANCE_ROUTES
from .action_spec import ActionSpec


def _runtime_acceptance_specs() -> dict[str, ActionSpec]:
    return {
        action: ActionSpec(
            route.action,
            route.title,
            route.target,
            validation_tier=route.validation_tier,
            guarantee_owner=route.guarantee_owner,
        )
        for action, route in RUNTIME_ACCEPTANCE_ROUTES.items()
    }


RUNTIME_VALIDATION_ACTION_SPECS: dict[str, ActionSpec] = {
    **_runtime_acceptance_specs(),
    "proof-runtime-architecture": ActionSpec("proof-runtime-architecture", "emit the integrated runtime architecture evidence bundle", "python:scripts/check_objc3c_runtime_architecture_proof_packet.py"),
    "validate-runtime-architecture": ActionSpec("validate-runtime-architecture", "validate runtime architecture across the full public workflow and evidence bundle", "python:scripts/check_objc3c_runtime_architecture_integration.py", validation_tier="full", guarantee_owner="full public workflow and runtime architecture evidence bundle alignment"),
    "validate-runnable-bootstrap": ActionSpec("validate-runnable-bootstrap", "validate the staged runnable toolchain end to end from the package root", "python:scripts/check_objc3c_runnable_bootstrap_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-block-arc-conformance": ActionSpec("validate-block-arc-conformance", "validate runnable block/ARC conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_block_arc_conformance.py", validation_tier="full", guarantee_owner="integrated block/ARC conformance over the live runtime architecture workflow"),
    "validate-runnable-block-arc": ActionSpec("validate-runnable-block-arc", "validate runnable block/ARC execution end to end from the package root", "python:scripts/check_objc3c_runnable_block_arc_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, block/ARC probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-concurrency-conformance": ActionSpec("validate-concurrency-conformance", "validate runnable concurrency conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_concurrency_conformance.py", validation_tier="full", guarantee_owner="integrated async/task/executor/actor conformance over the live runtime architecture workflow"),
    "validate-runnable-concurrency": ActionSpec("validate-runnable-concurrency", "validate runnable concurrency execution end to end from the package root", "python:scripts/check_objc3c_runnable_concurrency_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, concurrency probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-object-model-conformance": ActionSpec("validate-object-model-conformance", "validate runnable object-model conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_object_model_conformance.py", validation_tier="full", guarantee_owner="integrated object-model conformance over the live runtime architecture workflow"),
    "validate-storage-reflection-conformance": ActionSpec("validate-storage-reflection-conformance", "validate runnable storage/reflection conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_storage_reflection_conformance.py", validation_tier="full", guarantee_owner="integrated storage/accessor/reflection conformance over the live runtime architecture workflow"),
    "validate-runnable-object-model": ActionSpec("validate-runnable-object-model", "validate runnable object-model execution end to end from the package root", "python:scripts/check_objc3c_runnable_object_model_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, object-model probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-runnable-storage-reflection": ActionSpec("validate-runnable-storage-reflection", "validate runnable storage/reflection execution end to end from the package root", "python:scripts/check_objc3c_runnable_storage_reflection_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, storage/reflection probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-error-conformance": ActionSpec("validate-error-conformance", "validate runnable error conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_error_conformance.py", validation_tier="full", guarantee_owner="integrated error conformance over the live runtime architecture workflow"),
    "validate-runnable-error": ActionSpec("validate-runnable-error", "validate runnable error execution end to end from the package root", "python:scripts/check_objc3c_runnable_error_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, error probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-interop-conformance": ActionSpec("validate-interop-conformance", "validate runnable mixed-module and interop conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_interop_conformance.py", validation_tier="full", guarantee_owner="integrated mixed-module runtime packaging and interop conformance over the live runtime architecture workflow"),
    "validate-runnable-interop": ActionSpec("validate-runnable-interop", "validate runnable mixed-module and interop execution end to end from the package root", "python:scripts/check_objc3c_runnable_interop_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, interop probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-metaprogramming-conformance": ActionSpec("validate-metaprogramming-conformance", "validate runnable metaprogramming conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_metaprogramming_conformance.py", validation_tier="full", guarantee_owner="integrated metaprogramming conformance over the live runtime architecture workflow"),
    "validate-runnable-metaprogramming": ActionSpec("validate-runnable-metaprogramming", "validate runnable metaprogramming execution end to end from the package root", "python:scripts/check_objc3c_runnable_metaprogramming_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, metaprogramming probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-release-candidate-conformance": ActionSpec("validate-release-candidate-conformance", "validate runnable release-candidate conformance across the integrated live workflow", "python:scripts/check_objc3c_runnable_release_candidate_conformance.py", validation_tier="full", guarantee_owner="integrated public-claims strict-profile and release-candidate conformance over the live runtime architecture workflow"),
    "validate-runnable-release-candidate": ActionSpec("validate-runnable-release-candidate", "validate runnable release-candidate packaging and validation end to end from the package root", "python:scripts/check_objc3c_runnable_release_candidate_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, release-candidate validation, runtime probe execution, smoke, and replay from the staged runnable toolchain bundle"),
}
