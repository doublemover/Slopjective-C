"""Runnable stdlib package validation action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

RUNNABLE_STDLIB_ADVANCED_GUARANTEE_OWNER = (
    "packaged advanced stdlib helper contracts, profile gates, and subset smoke "
    "compilation stay reproducible from the staged runnable toolchain bundle"
)
RUNNABLE_STDLIB_FOUNDATION_GUARANTEE_OWNER = (
    "packaged stdlib boundary contracts, lowering/import artifact metadata, module "
    "smoke compilation, and runtime-archive linkage stay reproducible from the staged runnable toolchain bundle"
)
RUNNABLE_STDLIB_PROGRAM_GUARANTEE_OWNER = (
    "packaged stdlib program docs, showcase examples, and publish-input metadata "
    "stay reproducible from the staged runnable toolchain bundle"
)

APPLICATION_STDLIB_RUNNABLE_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-runnable-stdlib-advanced": ActionSpec(
        "validate-runnable-stdlib-advanced",
        "validate runnable advanced stdlib helper packaging and smoke compilation end to end from the package root",
        "python:scripts/check_objc3c_runnable_stdlib_advanced_end_to_end.py",
        validation_tier="full",
        guarantee_owner=RUNNABLE_STDLIB_ADVANCED_GUARANTEE_OWNER,
    ),
    "validate-runnable-stdlib-foundation": ActionSpec(
        "validate-runnable-stdlib-foundation",
        "validate runnable stdlib foundation packaging and smoke compilation end to end from the package root",
        "python:scripts/check_objc3c_runnable_stdlib_foundation_end_to_end.py",
        validation_tier="full",
        guarantee_owner=RUNNABLE_STDLIB_FOUNDATION_GUARANTEE_OWNER,
    ),
    "validate-runnable-stdlib-program": ActionSpec(
        "validate-runnable-stdlib-program",
        "validate the staged runnable stdlib program docs/example package surface end to end",
        "python:scripts/check_objc3c_runnable_stdlib_program_end_to_end.py",
        validation_tier="full",
        guarantee_owner=RUNNABLE_STDLIB_PROGRAM_GUARANTEE_OWNER,
    ),
}


__all__ = [
    "APPLICATION_STDLIB_RUNNABLE_ACTION_SPECS",
    "RUNNABLE_STDLIB_ADVANCED_GUARANTEE_OWNER",
    "RUNNABLE_STDLIB_FOUNDATION_GUARANTEE_OWNER",
    "RUNNABLE_STDLIB_PROGRAM_GUARANTEE_OWNER",
]
