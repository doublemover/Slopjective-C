from __future__ import annotations

from pathlib import Path

from remaining_task_extraction.seed_review_models import RawTask
from remaining_task_extraction.seed_review_text import PLANNING_ISSUE_PATTERN


def infer_bucket(task: RawTask) -> str:
    if task.path == "docs/reference/legacy_spec_anchor_index.md#conformance-profile-checklist":
        return "conformance-checklist"
    if task.path == "spec/conformance/profile_release_evidence_checklist.md":
        return "release-evidence"
    return "planning-checklist"


def infer_lane(task: RawTask, bucket: str, planning_lane_by_issue: dict[int, str]) -> str:
    if bucket == "conformance-checklist":
        return "B"
    if bucket == "release-evidence":
        return "D"

    match = PLANNING_ISSUE_PATTERN.search(task.origin_path)
    if match:
        issue_number = int(match.group(1))
        return planning_lane_by_issue.get(issue_number, "D")

    if task.origin_path.endswith("ROADMAP_REFRESH_CADENCE_AND_SNAPSHOT_PROTOCOL.md"):
        return "D"

    return "D"


def infer_milestone_title(
    task: RawTask,
    bucket: str,
    lane: str,
    conformance_milestone_by_tag: dict[str, str],
    lane_milestone_titles: dict[str, str],
) -> str:
    core_milestone = conformance_milestone_by_tag.get("[CORE]", "Conformance: Core (E)")
    strict_milestone = conformance_milestone_by_tag.get("[STRICT]", core_milestone)
    strict_concurrency_milestone = conformance_milestone_by_tag.get("[CONC]", core_milestone)
    strict_system_milestone = conformance_milestone_by_tag.get("[SYSTEM]", core_milestone)

    if bucket == "conformance-checklist":
        for tag, milestone in conformance_milestone_by_tag.items():
            if tag in task.text:
                return milestone
        return core_milestone

    if bucket == "release-evidence":
        lower = task.text.lower()
        if "strict-system" in lower:
            return strict_system_milestone
        if "strict-concurrency" in lower:
            return strict_concurrency_milestone
        if "mode.strictness=strict" in lower and "concurrency=off" in lower:
            return strict_milestone
        return core_milestone

    return lane_milestone_titles[lane]


def infer_area_label(task: RawTask, cleaned: str, lane: str, bucket: str) -> str:
    hay = f"{task.path} {cleaned}".lower()

    keyword_map: list[tuple[list[str], str]] = [
        (["mangl", "abi", "manifest"], "area:abi"),
        (["nullability", "optional", "iuo"], "area:nullability"),
        (["generic", "reification", "key path", "demangler"], "area:generics"),
        (["macro", "derive", "extension"], "area:macros"),
        (["concurrency", "async", "await", "actor", "sendable", "executor"], "area:concurrency"),
        (["error", "throws", "nserror", "try", "catch"], "area:errors"),
        (["defer", "guard", "match", "pattern"], "area:control-flow"),
        (["borrowed", "lifetime", "resource", "arc", "capture"], "area:memory"),
        (["module", "interface", "metadata"], "area:modules"),
        (["interop", "swift", "c++", "vendor"], "area:interop"),
        (["diagnostic", "fix-it", "lint", "tool", "ci", "validation"], "area:tooling"),
        (["grammar", "syntax", "parser"], "area:syntax"),
        (["version", "strictness", "mode"], "area:versioning"),
        (["roadmap", "checkpoint", "milestone", "carryover", "kickoff", "readiness"], "area:structure"),
    ]

    for needles, label in keyword_map:
        if any(needle in hay for needle in needles):
            return label

    if bucket == "conformance-checklist":
        return "area:conformance"
    if lane == "A":
        return "area:semantics"
    if lane == "B":
        return "area:tooling"
    if lane == "C":
        return "area:docs"
    return "area:structure"


def infer_priority_label(task: RawTask, bucket: str, lane: str) -> str:
    text = task.text
    lower = text.lower()
    if bucket == "conformance-checklist":
        if "[opt-" in lower:
            return "priority:P2"
        return "priority:P1"

    if bucket == "release-evidence":
        return "priority:P1"

    if any(token in lower for token in ["blocking", "critical", "go/no-go", "readiness", "quality gate", "kickoff"]):
        return "priority:P1"

    if lane in {"A", "B"}:
        return "priority:P1"

    return "priority:P2"


def infer_type_label(bucket: str) -> str:
    if bucket == "planning-checklist":
        return "type:process"
    if bucket == "release-evidence":
        return "type:tooling"
    return "type:spec-gap"


def derive_shard(task: RawTask) -> str:
    if task.path == "docs/reference/legacy_spec_anchor_index.md#conformance-profile-checklist":
        return "conformance-profile-checklist"
    if task.path == "spec/conformance/profile_release_evidence_checklist.md":
        return "release-evidence-checklist"

    stem = Path(task.origin_path).stem
    match = PLANNING_ISSUE_PATTERN.search(task.origin_path)
    if match:
        return f"planning-issue-{match.group(1)}"
    return stem
