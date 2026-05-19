"""Developer tooling validation action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

TOOLING_VALIDATION_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-developer-tooling": ActionSpec(
        "validate-developer-tooling",
        "run the integrated developer-tooling inspect and trace validation flow",
        "python:scripts/check_objc3c_developer_tooling_integration.py",
        validation_tier="repo",
        guarantee_owner=(
            "developer-facing inspect and trace commands stay executable, artifact-backed, "
            "and tied to the live frontend runner"
        ),
    ),
    "validate-bonus-experiences": ActionSpec(
        "validate-bonus-experiences",
        "run the integrated bonus-experience validation flow across the live showcase tutorial and template surfaces",
        "python:scripts/check_objc3c_bonus_experience_integration.py",
        validation_tier="repo",
        guarantee_owner=(
            "bonus-experience workflows stay executable, template-derived, and tied to "
            "the live showcase tutorial and developer-tooling surfaces"
        ),
    ),
    "validate-runnable-bonus-experiences": ActionSpec(
        "validate-runnable-bonus-experiences",
        "validate template-derived bonus experiences end to end from the staged runnable toolchain bundle",
        "python:scripts/check_objc3c_runnable_bonus_experience_end_to_end.py",
        validation_tier="full",
        guarantee_owner=(
            "staged runnable toolchain bundles preserve bonus-experience template "
            "compilation runtime execution and capability-probe semantics"
        ),
    ),
}
