"""Source hygiene action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

SOURCE_HYGIENE_AUTHENTICITY_ACTION = "check-source-hygiene-authenticity"
SOURCE_HYGIENE_HARD_CUTOVER_ACTION = "check-source-hygiene-hard-cutover"
TASK_HYGIENE_ACTION = "check-task-hygiene"

SOURCE_HYGIENE_AUTHENTICITY_BACKEND = "python:scripts/check_source_hygiene_authenticity.py"
SOURCE_HYGIENE_HARD_CUTOVER_BACKEND = "python -m scripts.source_hygiene"
TASK_HYGIENE_BACKEND = "python:scripts/ci/run_task_hygiene_gate.py"

SOURCE_HYGIENE_VALIDATION_TIER = "repo"
SOURCE_HYGIENE_AUTHENTICITY_GUARANTEE_OWNER = (
    "product truth surfaces, synthetic fixtures, and genuine generated outputs "
    "stay mechanically distinguished and fail closed when provenance drifts"
)
SOURCE_HYGIENE_HARD_CUTOVER_GUARANTEE_OWNER = (
    "hard-cutover forbidden strings, direct helper exposure, and tracked "
    "generated reports fail closed as active hygiene blockers"
)
TASK_HYGIENE_GUARANTEE_OWNER = (
    "task hygiene gate remains executable over the live repo script and path surface"
)

CORE_SOURCE_HYGIENE_ACTION_SPECS: dict[str, ActionSpec] = {
    SOURCE_HYGIENE_AUTHENTICITY_ACTION: ActionSpec(
        SOURCE_HYGIENE_AUTHENTICITY_ACTION,
        "check source-hygiene residue removal, authenticity labeling, and genuine-output provenance against the live enforcement contract",
        SOURCE_HYGIENE_AUTHENTICITY_BACKEND,
        validation_tier=SOURCE_HYGIENE_VALIDATION_TIER,
        guarantee_owner=SOURCE_HYGIENE_AUTHENTICITY_GUARANTEE_OWNER,
    ),
    SOURCE_HYGIENE_HARD_CUTOVER_ACTION: ActionSpec(
        SOURCE_HYGIENE_HARD_CUTOVER_ACTION,
        "scan active source roots for hard-cutover forbidden residue, direct helper exposure, adapter wording, and generated-report residue",
        SOURCE_HYGIENE_HARD_CUTOVER_BACKEND,
        validation_tier=SOURCE_HYGIENE_VALIDATION_TIER,
        guarantee_owner=SOURCE_HYGIENE_HARD_CUTOVER_GUARANTEE_OWNER,
    ),
    TASK_HYGIENE_ACTION: ActionSpec(
        TASK_HYGIENE_ACTION,
        "run the task-hygiene gate over package scripts and checked-in roots",
        TASK_HYGIENE_BACKEND,
        validation_tier=SOURCE_HYGIENE_VALIDATION_TIER,
        guarantee_owner=TASK_HYGIENE_GUARANTEE_OWNER,
    ),
}
