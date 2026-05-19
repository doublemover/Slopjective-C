from __future__ import annotations

from remaining_task_extraction.seed_review_models import RawTask
from remaining_task_extraction.seed_review_text import ISSUE_REF_PATTERN, TASK_REF_PATTERN


def evaluate_definition(task: RawTask, cleaned: str) -> tuple[str, list[str]]:
    lower = task.text.lower()

    has_path = "/" in task.text or ".md" in task.text or "artifact" in lower
    has_validation = (
        "pass" in lower
        or "validated" in lower
        or "validation" in lower
        or "test" in lower
        or "command" in lower
    )
    has_dependency = (
        "depend" in lower
        or "against" in lower
        or "mapping" in lower
        or "reviewed" in lower
        or bool(TASK_REF_PATTERN.search(task.text))
    )
    has_measurable_outcome = any(
        token in lower
        for token in [
            "explicit",
            "complete",
            "includes",
            "is present",
            "are recorded",
            "is accepted",
            "passes",
        ]
    )

    gaps: list[str] = []
    if not has_path:
        gaps.append("Missing explicit artifact/file target in the task line.")
    if not has_validation:
        gaps.append("Missing deterministic validation command or test expectation.")
    if not has_dependency:
        gaps.append("Missing explicit dependency/order constraints.")
    if not has_measurable_outcome:
        gaps.append("Outcome is activity-oriented; add measurable completion criteria.")

    if not gaps:
        quality = "strong"
    elif len(gaps) <= 2:
        quality = "medium"
    else:
        quality = "weak"

    return quality, gaps


def build_objective(cleaned: str) -> str:
    return cleaned.rstrip(".")


def build_deliverables(task: RawTask, bucket: str) -> list[str]:
    if bucket == "conformance-checklist":
        return [
            "Implement or codify the specified language/toolchain behavior.",
            "Add or update positive/negative conformance tests that demonstrate the behavior.",
            "Update conformance status evidence so the checklist row can be marked complete with traceable links.",
        ]

    if bucket == "release-evidence":
        return [
            "Produce the required evidence bundle fields/artifacts for the referenced profile gate.",
            "Attach validation outputs proving schema/command-level correctness.",
            "Record artifact digests and evidence pointers needed for release sign-off.",
        ]

    return [
        "Update the referenced planning artifact with concrete, non-placeholder content for this checklist row.",
        "Capture review/approval evidence (owner, date, and decision record) linked in the issue.",
        "Attach validation command results and closeout traceability so the row can be checked reliably.",
    ]


def build_acceptance_criteria(task: RawTask, bucket: str) -> list[str]:
    criteria = [
        "The original checklist intent is fully satisfied with concrete artifacts and links.",
        "Validation commands run successfully and outputs are attached in issue comments.",
        "Source checklist row is updated to checked state with commit SHA + evidence reference.",
    ]

    if bucket == "conformance-checklist":
        criteria[0] = "Required compiler/spec behavior is implemented (or explicitly documented as unsupported) with no ambiguity."

    if bucket == "release-evidence":
        criteria[0] = "All required release evidence keys/metrics for the row are present and conform to schema/contract."

    return criteria


def build_dependencies(
    task: RawTask,
    lane: str,
    bucket: str,
    lane_name: dict[str, str],
) -> list[str]:
    issue_refs = sorted(set(int(value) for value in ISSUE_REF_PATTERN.findall(task.text)))
    task_refs = sorted(set(TASK_REF_PATTERN.findall(task.text)))

    deps: list[str] = []
    if issue_refs:
        refs = ", ".join(f"#{number}" for number in issue_refs)
        deps.append(f"Traceability references: closed seed issue(s) {refs}; use for context while implementing this new task.")
    if task_refs:
        refs = ", ".join(task_refs)
        deps.append(f"Explicit cross-task references detected: {refs}; honor sequencing when these artifacts are touched.")

    if not deps:
        if bucket == "planning-checklist":
            deps.append("No hard dependency encoded in the row; schedule as parallel-ready within the same lane shard.")
        else:
            deps.append("No explicit hard dependency in the source row; treat as lane-parallel unless blocked by shared files.")

    deps.append(f"Lane-level dependency: execute under Lane {lane} governance ({lane_name[lane]}).")
    return deps


def build_validation_commands(bucket: str) -> list[str]:
    cmds = ["npm run objc3c -- lint-spec"]
    if bucket in {"conformance-checklist", "release-evidence"}:
        cmds.append("npm run objc3c -- check-task-hygiene")
    if bucket == "release-evidence":
        cmds.append("npm run objc3c -- check-release-evidence")
    return cmds


def build_labels(
    lane: str,
    bucket: str,
    priority_label: str,
    area_label: str,
    type_label: str,
) -> list[str]:
    labels = [
        f"lane:{lane}",
        f"source:{bucket}",
        "parallelizable",
        priority_label,
        area_label,
        type_label,
    ]
    deduped: list[str] = []
    seen: set[str] = set()
    for label in labels:
        if label in seen:
            continue
        seen.add(label)
        deduped.append(label)
    return deduped
