"""Canonical application architecture action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CANONICAL_APPLICATION_WORKSPACE_GUARANTEE_OWNER = (
    "canonical application workspace materialization stays derived from the "
    "checked-in showcase and stdlib contracts"
)
APPLICATION_ARCHITECTURE_GUARANTEE_OWNER = (
    "template harnesses and canonical application workspaces stay derived from "
    "live showcase, stdlib, and public workflow surfaces"
)
RUNNABLE_APPLICATION_ARCHITECTURE_GUARANTEE_OWNER = (
    "packaged canonical application workspaces and template harness validation "
    "stay reproducible from the staged runnable toolchain bundle"
)
APPLICATION_FRAMEWORK_SAMPLES_GUARANTEE_OWNER = (
    "application framework samples stay checked-in, package-aware, and "
    "compiled through the public objc3c workflow surface"
)

APPLICATION_ARCHITECTURE_ACTION_SPECS: dict[str, ActionSpec] = {
    "materialize-canonical-application-workspace": ActionSpec(
        "materialize-canonical-application-workspace",
        "materialize the canonical application workspace from the checked-in showcase and stdlib surfaces",
        "python:scripts/materialize_objc3c_canonical_application_workspace.py",
        validation_tier="repo",
        guarantee_owner=CANONICAL_APPLICATION_WORKSPACE_GUARANTEE_OWNER,
        pass_through_args=True,
    ),
    "validate-application-architecture": ActionSpec(
        "validate-application-architecture",
        "run the integrated template and canonical application workspace validation flow",
        "python:scripts/check_objc3c_application_architecture_integration.py",
        validation_tier="repo",
        guarantee_owner=APPLICATION_ARCHITECTURE_GUARANTEE_OWNER,
    ),
    "validate-application-framework-samples": ActionSpec(
        "validate-application-framework-samples",
        "compile and validate the checked application framework sample libraries and apps",
        "python:scripts/check_objc3c_application_framework_samples.py",
        validation_tier="repo",
        guarantee_owner=APPLICATION_FRAMEWORK_SAMPLES_GUARANTEE_OWNER,
    ),
    "validate-runnable-application-architecture": ActionSpec(
        "validate-runnable-application-architecture",
        "validate packaged canonical application workspace and template surfaces end to end from the staged runnable toolchain bundle",
        "python:scripts/check_objc3c_runnable_application_architecture_end_to_end.py",
        validation_tier="full",
        guarantee_owner=RUNNABLE_APPLICATION_ARCHITECTURE_GUARANTEE_OWNER,
    ),
}


__all__ = [
    "APPLICATION_ARCHITECTURE_ACTION_SPECS",
    "APPLICATION_FRAMEWORK_SAMPLES_GUARANTEE_OWNER",
    "APPLICATION_ARCHITECTURE_GUARANTEE_OWNER",
    "CANONICAL_APPLICATION_WORKSPACE_GUARANTEE_OWNER",
    "RUNNABLE_APPLICATION_ARCHITECTURE_GUARANTEE_OWNER",
]
