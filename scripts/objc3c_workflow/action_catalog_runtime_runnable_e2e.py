"""Runtime runnable end-to-end action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

RUNTIME_RUNNABLE_E2E_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-runnable-bootstrap": ActionSpec("validate-runnable-bootstrap", "validate the staged runnable toolchain end to end from the package root", "python:scripts/check_objc3c_runnable_bootstrap_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-runnable-block-arc": ActionSpec("validate-runnable-block-arc", "validate runnable block/ARC execution end to end from the package root", "python:scripts/check_objc3c_runnable_block_arc_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, block/ARC probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-runnable-concurrency": ActionSpec("validate-runnable-concurrency", "validate runnable concurrency execution end to end from the package root", "python:scripts/check_objc3c_runnable_concurrency_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, concurrency probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-runnable-object-model": ActionSpec("validate-runnable-object-model", "validate runnable object-model execution end to end from the package root", "python:scripts/check_objc3c_runnable_object_model_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, object-model probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-runnable-storage-reflection": ActionSpec("validate-runnable-storage-reflection", "validate runnable storage/reflection execution end to end from the package root", "python:scripts/check_objc3c_runnable_storage_reflection_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, storage/reflection probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-runnable-error": ActionSpec("validate-runnable-error", "validate runnable error execution end to end from the package root", "python:scripts/check_objc3c_runnable_error_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, error probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-runnable-interop": ActionSpec("validate-runnable-interop", "validate runnable mixed-module and interop execution end to end from the package root", "python:scripts/check_objc3c_runnable_interop_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, interop probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-runnable-metaprogramming": ActionSpec("validate-runnable-metaprogramming", "validate runnable metaprogramming execution end to end from the package root", "python:scripts/check_objc3c_runnable_metaprogramming_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, metaprogramming probe execution, smoke, and replay from the staged runnable toolchain bundle"),
    "validate-runnable-release-candidate": ActionSpec("validate-runnable-release-candidate", "validate runnable release-candidate packaging and validation end to end from the package root", "python:scripts/check_objc3c_runnable_release_candidate_end_to_end.py", validation_tier="full", guarantee_owner="packaged compile, release-candidate validation, runtime probe execution, smoke, and replay from the staged runnable toolchain bundle"),
}


__all__ = ["RUNTIME_RUNNABLE_E2E_ACTION_SPECS"]
