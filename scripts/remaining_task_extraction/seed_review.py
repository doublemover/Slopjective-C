from __future__ import annotations

import hashlib
import re
from dataclasses import dataclass
from pathlib import Path

from remaining_task_extraction.seed_config import SeedToolingConfig

ISSUE_REF_PATTERN = re.compile(r"Issue #(\d+)")
TASK_REF_PATTERN = re.compile(r"\b([ABCD]-\d{2})\b")
LINK_PATTERN = re.compile(r"\[([^\]]+)\]\([^\)]+\)")
CODE_PATTERN = re.compile(r"`([^`]+)`")
SPACES_PATTERN = re.compile(r"\s+")
PLANNING_ISSUE_PATTERN = re.compile(r"issue_(\d+)_")
DEFAULT_EXECUTION_STATUS = "open"


@dataclass(frozen=True)
class RawTask:
    path: str
    line: int
    text: str
    origin_path: str


@dataclass(frozen=True)
class ReviewedTask:
    task_id: str
    task_key: str
    source_line_hash: str
    path: str
    line: int
    original: str
    cleaned: str
    bucket: str
    lane: str
    milestone_title: str
    priority_label: str
    area_label: str
    type_label: str
    labels: list[str]
    quality: str
    quality_gaps: list[str]
    objective: str
    deliverables: list[str]
    acceptance_criteria: list[str]
    dependencies: list[str]
    validation_commands: list[str]
    shard: str
    title: str
    execution_status: str


def clean_markdown_text(text: str) -> str:
    without_links = LINK_PATTERN.sub(r"\1", text)
    without_code = CODE_PATTERN.sub(r"\1", without_links)
    without_emphasis = without_code.replace("**", "")
    without_issue_url = re.sub(r"\(\[Issue #\d+\]\([^\)]+\)\)", "", without_emphasis)
    compact = SPACES_PATTERN.sub(" ", without_issue_url)
    return compact.strip(" .:")


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


def normalize_title(cleaned: str) -> str:
    text = cleaned
    text = re.sub(r"\s*\([^)]+\)$", "", text)
    text = text.replace('"', "'")
    text = text.strip(" .:")
    if len(text) > 150:
        text = text[:147].rstrip() + "..."
    return text


def build_title(task_id: str, lane: str, cleaned: str) -> str:
    base = normalize_title(cleaned)
    title = f"[{task_id}][Lane {lane}] {base}"
    if len(title) > 240:
        title = title[:237].rstrip() + "..."
    return title


def build_task_key(task: RawTask) -> str:
    return f"{task.path}:{task.line}"


def build_source_line_hash(task: RawTask) -> str:
    payload = f"{task.origin_path}:{task.line}:{task.text}".encode("utf-8")
    return hashlib.sha256(payload).hexdigest()


def format_source_reference(task: ReviewedTask) -> str:
    return f"{task.path}:{task.line}"


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


def review_tasks(raw_tasks: list[RawTask], config: SeedToolingConfig) -> list[ReviewedTask]:
    reviewed: list[ReviewedTask] = []
    for idx, raw in enumerate(raw_tasks, start=1):
        task_id = f"SPT-{idx:04d}"
        task_key = build_task_key(raw)
        source_line_hash = build_source_line_hash(raw)
        cleaned = clean_markdown_text(raw.text)
        bucket = infer_bucket(raw)
        lane = infer_lane(raw, bucket, config.planning_lane_by_issue)
        milestone_title = infer_milestone_title(
            raw,
            bucket,
            lane,
            config.conformance_milestone_by_tag,
            config.lane_milestone_titles,
        )
        priority_label = infer_priority_label(raw, bucket, lane)
        area_label = infer_area_label(raw, cleaned, lane, bucket)
        type_label = infer_type_label(bucket)
        quality, gaps = evaluate_definition(raw, cleaned)
        objective = build_objective(cleaned)
        deliverables = build_deliverables(raw, bucket)
        acceptance_criteria = build_acceptance_criteria(raw, bucket)
        dependencies = build_dependencies(raw, lane, bucket, config.lane_name)
        validation_commands = build_validation_commands(bucket)
        shard = derive_shard(raw)
        labels = build_labels(
            lane=lane,
            bucket=bucket,
            priority_label=priority_label,
            area_label=area_label,
            type_label=type_label,
        )
        title = build_title(task_id, lane, cleaned)

        reviewed.append(
            ReviewedTask(
                task_id=task_id,
                task_key=task_key,
                source_line_hash=source_line_hash,
                path=raw.path,
                line=raw.line,
                original=raw.text,
                cleaned=cleaned,
                bucket=bucket,
                lane=lane,
                milestone_title=milestone_title,
                priority_label=priority_label,
                area_label=area_label,
                type_label=type_label,
                labels=labels,
                quality=quality,
                quality_gaps=gaps,
                objective=objective,
                deliverables=deliverables,
                acceptance_criteria=acceptance_criteria,
                dependencies=dependencies,
                validation_commands=validation_commands,
                shard=shard,
                title=title,
                execution_status=DEFAULT_EXECUTION_STATUS,
            )
        )

    return reviewed
