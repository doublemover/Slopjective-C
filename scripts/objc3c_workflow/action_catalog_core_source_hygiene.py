"""Source hygiene action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CORE_SOURCE_HYGIENE_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-source-hygiene-authenticity": ActionSpec(
        "check-source-hygiene-authenticity",
        "check source-hygiene residue removal, authenticity labeling, and genuine-output provenance against the live enforcement contract",
        "python:scripts/check_source_hygiene_authenticity.py",
        validation_tier="repo",
        guarantee_owner=(
            "product truth surfaces, synthetic fixtures, and genuine generated outputs "
            "stay mechanically distinguished and fail closed when provenance drifts"
        ),
    ),
    "check-source-hygiene-hard-cutover": ActionSpec(
        "check-source-hygiene-hard-cutover",
        "scan active source roots for hard-cutover forbidden residue, direct helper exposure, adapter wording, and generated-report residue",
        "python -m scripts.source_hygiene",
        validation_tier="repo",
        guarantee_owner=(
            "hard-cutover forbidden strings, direct helper exposure, and tracked "
            "generated reports fail closed as active hygiene blockers"
        ),
    ),
    "check-task-hygiene": ActionSpec(
        "check-task-hygiene",
        "run the task-hygiene gate over package scripts and checked-in roots",
        "python:scripts/ci/run_task_hygiene_gate.py",
        validation_tier="repo",
        guarantee_owner=(
            "task hygiene gate remains executable over the live repo script and path surface"
        ),
    ),
}
