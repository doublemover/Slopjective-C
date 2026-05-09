"""Canonical application architecture action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

APPLICATION_ARCHITECTURE_ACTION_SPECS: dict[str, ActionSpec] = {
    "materialize-canonical-application-workspace": ActionSpec("materialize-canonical-application-workspace", "materialize the canonical application workspace from the checked-in showcase and stdlib surfaces", "python:scripts/materialize_objc3c_canonical_application_workspace.py", validation_tier="repo", guarantee_owner="canonical application workspace materialization stays derived from the checked-in showcase and stdlib contracts", pass_through_args=True),
    "validate-application-architecture": ActionSpec("validate-application-architecture", "run the integrated template and canonical application workspace validation flow", "python:scripts/check_objc3c_application_architecture_integration.py", validation_tier="repo", guarantee_owner="template harnesses and canonical application workspaces stay derived from live showcase, stdlib, and public workflow surfaces"),
    "validate-runnable-application-architecture": ActionSpec("validate-runnable-application-architecture", "validate packaged canonical application workspace and template surfaces end to end from the staged runnable toolchain bundle", "python:scripts/check_objc3c_runnable_application_architecture_end_to_end.py", validation_tier="full", guarantee_owner="packaged canonical application workspaces and template harness validation stay reproducible from the staged runnable toolchain bundle"),
}


__all__ = ["APPLICATION_ARCHITECTURE_ACTION_SPECS"]
