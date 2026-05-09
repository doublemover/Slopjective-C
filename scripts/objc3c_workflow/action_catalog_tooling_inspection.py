"""Developer tooling inspection action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

TOOLING_INSPECTION_ACTION_SPECS: dict[str, ActionSpec] = {
    "inspect-bonus-tool-integration": ActionSpec(
        "inspect-bonus-tool-integration",
        "emit the live bonus-tool integration surface from the build-owned source-of-truth artifact and checked-in showcase/tutorial contracts",
        "runner-internal + tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json",
        validation_tier="repo",
        guarantee_owner=(
            "bonus-tool integration stays rooted in the build-owned source-of-truth "
            "artifact and checked-in showcase/tutorial contracts"
        ),
    ),
    "inspect-validation-timing": ActionSpec(
        "inspect-validation-timing",
        "build the local validation-speed dashboard from latest generated timing reports",
        "runner-internal + tmp timing reports",
        validation_tier="repo",
        guarantee_owner=(
            "validation timing, warning budgets, child reports, and issue-specific "
            "profiles stay explainable from generated suite reports"
        ),
    ),
    "trace-compile-stages": ActionSpec(
        "trace-compile-stages",
        "compile one source through the frontend C API runner and dump the stage trace object",
        "runner-internal + artifacts/bin/objc3c-frontend-c-api-runner.exe",
        validation_tier="repo",
        guarantee_owner=(
            "developer-facing compile stage traces stay tied to the real frontend runner "
            "stage summaries and process exit semantics"
        ),
        pass_through_args=True,
    ),
}
