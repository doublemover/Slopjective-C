"""Stdlib integration validation action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

STDLIB_FOUNDATION_GUARANTEE_OWNER = (
    "stdlib boundary contracts, lowering/import artifact expectations, workspace "
    "materialization, and smoke compilation stay executable on the live public workflow"
)
STDLIB_ADVANCED_GUARANTEE_OWNER = (
    "advanced stdlib helper module contracts, profile gates, and shared smoke "
    "compilation stay executable on the live public workflow"
)
STDLIB_PROGRAM_GUARANTEE_OWNER = (
    "stdlib publish/adoption docs, capability demos, tutorial routing, and stdlib "
    "smoke integration stay executable on the live public workflow"
)
STDLIB_TEXT_MODEL_GUARANTEE_OWNER = (
    "runtime-backed objc3.text scalar, formatting, equality, and builder "
    "contracts stay replayable through the public workflow bridge"
)

APPLICATION_STDLIB_INTEGRATION_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-stdlib-foundation": ActionSpec(
        "validate-stdlib-foundation",
        "run the integrated stdlib boundary and smoke validation flow",
        "python:scripts/check_objc3c_stdlib_foundation_integration.py",
        validation_tier="repo",
        guarantee_owner=STDLIB_FOUNDATION_GUARANTEE_OWNER,
    ),
    "validate-stdlib-advanced": ActionSpec(
        "validate-stdlib-advanced",
        "run the integrated advanced stdlib helper validation flow",
        "python:scripts/check_objc3c_stdlib_advanced_integration.py",
        validation_tier="repo",
        guarantee_owner=STDLIB_ADVANCED_GUARANTEE_OWNER,
    ),
    "validate-stdlib-program": ActionSpec(
        "validate-stdlib-program",
        "run the integrated stdlib program docs, showcase, tutorial, and capability-adoption validation flow",
        "python:scripts/check_objc3c_stdlib_program_integration.py",
        validation_tier="repo",
        guarantee_owner=STDLIB_PROGRAM_GUARANTEE_OWNER,
    ),
    "validate-string-text-model-runtime": ActionSpec(
        "validate-string-text-model-runtime",
        "run the runtime-backed objc3.text model probe",
        "python:-m pytest tests/tooling/test_string_text_model_runtime.py",
        validation_tier="targeted",
        guarantee_owner=STDLIB_TEXT_MODEL_GUARANTEE_OWNER,
    ),
}


__all__ = [
    "APPLICATION_STDLIB_INTEGRATION_ACTION_SPECS",
    "STDLIB_ADVANCED_GUARANTEE_OWNER",
    "STDLIB_FOUNDATION_GUARANTEE_OWNER",
    "STDLIB_PROGRAM_GUARANTEE_OWNER",
    "STDLIB_TEXT_MODEL_GUARANTEE_OWNER",
]
