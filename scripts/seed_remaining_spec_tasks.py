#!/usr/bin/env python3
"""Review unchecked spec tasks and seed organized GitHub issues.

This script does three things in one deterministic pass:
1. Extract every unchecked checkbox task from spec/*.md files.
2. Generate a comprehensive review catalog with improved task definitions.
3. Optionally create GitHub issues for each task with lane/milestone metadata.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import subprocess
import sys
import time
from dataclasses import dataclass
from datetime import date
from pathlib import Path
from typing import Any

SCRIPT_ROOT = Path(__file__).resolve().parent
if str(SCRIPT_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPT_ROOT))

from remaining_task_extraction.seed_config import (
    ConfigError,
    SeedToolingConfig,
    load_seed_tooling_config,
    resolve_generated_on,
)

ROOT = SCRIPT_ROOT.parents[0]
SPEC_ROOT = ROOT / "spec"

CHECKBOX_PATTERN = re.compile(r"^- \[ \]\s+(.*)$")
ISSUE_REF_PATTERN = re.compile(r"Issue #(\d+)")
TASK_REF_PATTERN = re.compile(r"\b([ABCD]-\d{2})\b")
LINK_PATTERN = re.compile(r"\[([^\]]+)\]\([^\)]+\)")
CODE_PATTERN = re.compile(r"`([^`]+)`")
SPACES_PATTERN = re.compile(r"\s+")
PLANNING_ISSUE_PATTERN = re.compile(r"issue_(\d+)_")
DEFAULT_EXECUTION_STATUS = "open"
LEGACY_CONFORMANCE_PROFILE_SOURCE = (
    "docs/reference/legacy_spec_anchor_index.md#conformance-profile-checklist"
)
LEGACY_PLANNING_SOURCE = "docs/reference/legacy_spec_anchor_index.md"


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


class GhError(RuntimeError):
    pass


def run_cmd(args: list[str], input_text: str | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        args,
        cwd=ROOT,
        input=input_text,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def run_gh_json(args: list[str], input_payload: dict[str, Any] | None = None) -> Any:
    input_text = None
    full_args = ["gh", *args]
    if input_payload is not None:
        full_args.extend(["--input", "-"])
        input_text = json.dumps(input_payload)

    proc = run_cmd(full_args, input_text=input_text)
    if proc.returncode != 0:
        detail = proc.stderr.strip() or proc.stdout.strip() or f"exit {proc.returncode}"
        raise GhError(f"{' '.join(full_args)} failed: {detail}")

    stdout = proc.stdout.strip()
    if not stdout:
        return None

    try:
        return json.loads(stdout)
    except json.JSONDecodeError as exc:
        raise GhError(f"{' '.join(full_args)} returned invalid JSON: {exc}") from exc


def extract_raw_tasks() -> list[RawTask]:
    tasks: list[RawTask] = []
    for path in sorted(SPEC_ROOT.rglob("*.md")):
        rel = path.relative_to(ROOT).as_posix()
        if rel == "spec/CONFORMANCE_PROFILE_CHECKLIST.md":
            source_rel = LEGACY_CONFORMANCE_PROFILE_SOURCE
        elif PLANNING_ISSUE_PATTERN.search(rel):
            source_rel = LEGACY_PLANNING_SOURCE
        else:
            source_rel = rel
        lines = path.read_text(encoding="utf-8").splitlines()
        for idx, line in enumerate(lines, start=1):
            match = CHECKBOX_PATTERN.match(line)
            if not match:
                continue
            tasks.append(
                RawTask(
                    path=source_rel,
                    line=idx,
                    text=match.group(1).strip(),
                    origin_path=rel,
                )
            )
    return tasks


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


def render_catalog_markdown(
    tasks: list[ReviewedTask],
    lane_name: dict[str, str],
    generated_on: date,
) -> str:
    lane_counts: dict[str, int] = {"A": 0, "B": 0, "C": 0, "D": 0}
    bucket_counts: dict[str, int] = {}
    quality_counts: dict[str, int] = {}

    for task in tasks:
        lane_counts[task.lane] = lane_counts.get(task.lane, 0) + 1
        bucket_counts[task.bucket] = bucket_counts.get(task.bucket, 0) + 1
        quality_counts[task.quality] = quality_counts.get(task.quality, 0) + 1

    lines: list[str] = [
        "# Remaining Task Review Catalog (510-Task Sweep)",
        "",
        f"_Generated on {generated_on.isoformat()} by scripts/seed_remaining_spec_tasks.py._",
        "",
        "## Coverage Summary",
        "",
        f"- Total reviewed tasks: **{len(tasks)}**",
        f"- Bucket counts: conformance-checklist **{bucket_counts.get('conformance-checklist', 0)}**, release-evidence **{bucket_counts.get('release-evidence', 0)}**, planning-checklist **{bucket_counts.get('planning-checklist', 0)}**",
        f"- Lane counts: A **{lane_counts.get('A', 0)}**, B **{lane_counts.get('B', 0)}**, C **{lane_counts.get('C', 0)}**, D **{lane_counts.get('D', 0)}**",
        f"- Definition quality counts: strong **{quality_counts.get('strong', 0)}**, medium **{quality_counts.get('medium', 0)}**, weak **{quality_counts.get('weak', 0)}**",
        "",
        "## Parallel Worklane Guidance",
        "",
        "- Lane A: normative/spec closure tasks with cross-part semantic contracts.",
        "- Lane B: implementation/tooling and conformance automation tasks.",
        "- Lane C: governance, extension process, and ecosystem policy tasks.",
        "- Lane D: release readiness, checkpoints, handoff, and operational control tasks.",
        "",
        "## Task Reviews",
        "",
    ]

    for task in tasks:
        lines.append(f"### {task.task_id} - {task.title}")
        lines.append("")
        lines.append(f"- Source: `{format_source_reference(task)}`")
        lines.append(f"- Bucket: `{task.bucket}`")
        lines.append(f"- Lane: `Lane {task.lane} - {lane_name[task.lane]}`")
        lines.append(f"- Milestone: `{task.milestone_title}`")
        lines.append(f"- Shard: `{task.shard}`")
        lines.append(f"- Original checkbox: `{task.original}`")
        lines.append(f"- Review quality: `{task.quality}`")
        if task.quality_gaps:
            lines.append("- Gaps identified:")
            for gap in task.quality_gaps:
                lines.append(f"  - {gap}")
        else:
            lines.append("- Gaps identified: none; wording already has strong structure.")
        lines.append(f"- Improved objective: {task.objective}")
        lines.append("- Improved deliverables:")
        for item in task.deliverables:
            lines.append(f"  - {item}")
        lines.append("- Improved acceptance criteria:")
        for item in task.acceptance_criteria:
            lines.append(f"  - {item}")
        lines.append("- Dependencies:")
        for item in task.dependencies:
            lines.append(f"  - {item}")
        lines.append("- Validation commands:")
        for cmd in task.validation_commands:
            lines.append(f"  - `{cmd}`")
        lines.append("")

    return "\n".join(lines).rstrip() + "\n"


def serialize_catalog_json(
    tasks: list[ReviewedTask],
    lane_name: dict[str, str],
    generated_on: date,
) -> str:
    payload = {
        "generated_on": generated_on.isoformat(),
        "task_count": len(tasks),
        "tasks": [
            {
                "task_id": task.task_id,
                "task_key": task.task_key,
                "source_line_hash": task.source_line_hash,
                "title": task.title,
                "path": task.path,
                "line": task.line,
                "bucket": task.bucket,
                "lane": task.lane,
                "lane_name": lane_name[task.lane],
                "milestone_title": task.milestone_title,
                "priority_label": task.priority_label,
                "area_label": task.area_label,
                "type_label": task.type_label,
                "labels": task.labels,
                "quality": task.quality,
                "quality_gaps": task.quality_gaps,
                "original": task.original,
                "cleaned": task.cleaned,
                "objective": task.objective,
                "deliverables": task.deliverables,
                "acceptance_criteria": task.acceptance_criteria,
                "dependencies": task.dependencies,
                "validation_commands": task.validation_commands,
                "shard": task.shard,
                "execution_status": task.execution_status,
            }
            for task in tasks
        ],
    }
    return json.dumps(payload, indent=2) + "\n"


def build_issue_body(task: ReviewedTask, lane_name: dict[str, str]) -> str:
    quality_line = {
        "strong": "Strong baseline wording; improvements focus on execution rigor and evidence shape.",
        "medium": "Partially defined; improvements add missing acceptance and validation precision.",
        "weak": "Underspecified; improvements convert this into an outcome-driven implementation task.",
    }[task.quality]

    gap_lines = task.quality_gaps or [
        "No structural gaps detected in source wording; issue still adds deterministic execution metadata."
    ]

    parallel_with = ", ".join(lane for lane in ["A", "B", "C", "D"] if lane != task.lane)

    body = [
        f"## Source Traceability",
        f"- Task ID: `{task.task_id}`",
        f"- Source: `{format_source_reference(task)}`",
        f"- Source bucket: `{task.bucket}`",
        f"- Original checkbox: `{task.original}`",
        "",
        "## Definition Review",
        f"- Assessment: **{task.quality.upper()}**",
        f"- Summary: {quality_line}",
        "- What I would do differently:",
    ]

    for gap in gap_lines:
        body.append(f"  - {gap}")

    body.extend(
        [
            "",
            "## Improved Task Definition",
            f"### Objective\n{task.objective}",
            "",
            "### Deliverables",
        ]
    )
    for item in task.deliverables:
        body.append(f"- {item}")

    body.append("")
    body.append("### Acceptance Criteria")
    for idx, item in enumerate(task.acceptance_criteria, start=1):
        body.append(f"{idx}. {item}")

    body.append("")
    body.append("### Dependencies")
    for item in task.dependencies:
        body.append(f"- {item}")

    body.append("")
    body.append("### Validation Commands")
    for cmd in task.validation_commands:
        body.append(f"- `{cmd}`")

    body.extend(
        [
            "",
            "## Parallel Execution Plan",
            f"- Primary lane: **Lane {task.lane} - {lane_name[task.lane]}**",
            f"- Parallelizable with lanes: **{parallel_with}**",
            f"- Suggested shard key: `{task.shard}` (batch same-shard tasks to reduce context switching)",
            "",
            "## Closeout Evidence Required",
            "- Commit SHA(s) implementing the task",
            "- Paths to produced artifacts/tests/evidence",
            "- Validation command outputs",
            "- Checklist row update reference",
        ]
    )

    return "\n".join(body).strip() + "\n"


def ensure_labels(repo: str, label_defs: dict[str, tuple[str, str]]) -> None:
    existing = run_gh_json(["label", "list", "--limit", "500", "--json", "name"])
    existing_names = {
        item["name"] for item in existing if isinstance(item, dict) and isinstance(item.get("name"), str)
    }

    for name, (color, description) in label_defs.items():
        if name in existing_names:
            continue
        run_gh_json(
            [
                "api",
                f"repos/{repo}/labels",
                "-X",
                "POST",
                "-f",
                f"name={name}",
                "-f",
                f"color={color}",
                "-f",
                f"description={description}",
            ]
        )


def fetch_milestone_map(repo: str) -> dict[str, int]:
    payload = run_gh_json(
        ["api", f"repos/{repo}/milestones?state=all&per_page=100"]
    )
    mapping: dict[str, int] = {}
    if isinstance(payload, list):
        for item in payload:
            if not isinstance(item, dict):
                continue
            title = item.get("title")
            number = item.get("number")
            if isinstance(title, str) and isinstance(number, int):
                mapping[title] = number
    return mapping


def ensure_lane_milestones(
    repo: str,
    lane_milestone_titles: dict[str, str],
    lane_milestone_due_on: dict[str, str],
    lane_name: dict[str, str],
) -> dict[str, int]:
    mapping = fetch_milestone_map(repo)
    for lane, title in lane_milestone_titles.items():
        if title in mapping:
            continue
        description = (
            f"Parallel lane {lane} ({lane_name[lane]}) task batch generated from the 510-task unchecked spec sweep."
        )
        due_on = lane_milestone_due_on[lane]
        payload = run_gh_json(
            ["api", f"repos/{repo}/milestones", "-X", "POST"],
            input_payload={
                "title": title,
                "description": description,
                "due_on": due_on,
            },
        )
        if isinstance(payload, dict) and isinstance(payload.get("number"), int):
            mapping[title] = int(payload["number"])

    return mapping


def fetch_existing_seeded_task_ids() -> set[str]:
    payload = run_gh_json(
        ["issue", "list", "--state", "all", "--limit", "2000", "--json", "title"]
    )
    ids: set[str] = set()
    if not isinstance(payload, list):
        return ids

    pattern = re.compile(r"^\[(SPT-\d{4})\]")
    for item in payload:
        if not isinstance(item, dict):
            continue
        title = item.get("title")
        if not isinstance(title, str):
            continue
        match = pattern.match(title)
        if match:
            ids.add(match.group(1))
    return ids


def create_issue(repo: str, payload: dict[str, Any]) -> dict[str, Any]:
    retries = 6
    wait_seconds = 1.0
    for attempt in range(1, retries + 1):
        proc = run_cmd(
            ["gh", "api", f"repos/{repo}/issues", "-X", "POST", "--input", "-"],
            input_text=json.dumps(payload),
        )
        if proc.returncode == 0:
            try:
                result = json.loads(proc.stdout)
            except json.JSONDecodeError as exc:
                raise GhError(f"Issue creation returned non-JSON output: {exc}") from exc
            if not isinstance(result, dict):
                raise GhError("Issue creation returned unexpected JSON shape")
            return result

        stderr = proc.stderr.strip() or proc.stdout.strip()
        too_fast = "secondary rate limit" in stderr.lower() or "abuse detection" in stderr.lower()
        transient = "502" in stderr or "503" in stderr or "504" in stderr

        if attempt < retries and (too_fast or transient):
            sleep_for = max(wait_seconds, 60.0 if too_fast else wait_seconds)
            time.sleep(sleep_for)
            wait_seconds *= 2
            continue

        raise GhError(f"Issue creation failed: {stderr or f'exit {proc.returncode}'}")

    raise GhError("Issue creation failed after retries")


def seed_issues(
    repo: str,
    tasks: list[ReviewedTask],
    milestone_map: dict[str, int],
    lane_name: dict[str, str],
    sleep_seconds: float,
    limit: int | None,
) -> tuple[int, int]:
    existing_ids = fetch_existing_seeded_task_ids()

    created = 0
    skipped = 0
    processed = 0

    for task in tasks:
        if limit is not None and processed >= limit:
            break
        processed += 1

        if task.task_id in existing_ids:
            skipped += 1
            continue

        milestone_number = milestone_map.get(task.milestone_title)
        if milestone_number is None:
            raise GhError(
                f"Missing milestone mapping for '{task.milestone_title}' while seeding {task.task_id}"
            )

        payload = {
            "title": task.title,
            "body": build_issue_body(task, lane_name),
            "milestone": milestone_number,
            "labels": task.labels,
        }

        result = create_issue(repo, payload)
        number = result.get("number")
        url = result.get("html_url")
        if isinstance(number, int) and isinstance(url, str):
            print(f"created #{number} {task.task_id} {url}")
        else:
            print(f"created {task.task_id}")

        created += 1
        time.sleep(sleep_seconds)

    return created, skipped


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        prog="seed_remaining_spec_tasks.py",
        description=(
            "Review all unchecked spec tasks, generate improved task definitions, "
            "and optionally seed GitHub issues with milestones/lanes."
        ),
    )
    parser.add_argument(
        "--config",
        type=Path,
        default=None,
        help=(
            "Optional path to tooling config JSON. When omitted, built-in defaults are used; "
            "built-in defaults match pre-HB-06 behavior."
        ),
    )
    parser.add_argument(
        "--repo",
        default=None,
        help=(
            "GitHub repository in owner/name format. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--catalog-md",
        type=Path,
        default=None,
        help=(
            "Path to write markdown review catalog. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--catalog-json",
        type=Path,
        default=None,
        help=(
            "Path to write JSON review catalog. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--create-issues",
        action="store_true",
        help="Create GitHub issues for reviewed tasks.",
    )
    parser.add_argument(
        "--limit",
        type=int,
        default=None,
        help="Optional limit for issue creation (for staged runs).",
    )
    parser.add_argument(
        "--sleep-seconds",
        type=float,
        default=None,
        help=(
            "Delay between issue creations to reduce rate-limit risk. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--generated-on",
        default=None,
        help=(
            "Date to embed in catalog outputs in YYYY-MM-DD format. "
            "When omitted, SOURCE_DATE_EPOCH is honored if present; otherwise today's date is used."
        ),
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    config = load_seed_tooling_config(args.config, root=ROOT)

    repo = args.repo if args.repo is not None else config.repo_default
    catalog_md = args.catalog_md if args.catalog_md is not None else config.catalog_md_default
    catalog_json = args.catalog_json if args.catalog_json is not None else config.catalog_json_default
    sleep_seconds = args.sleep_seconds if args.sleep_seconds is not None else config.sleep_seconds_default
    generated_on = resolve_generated_on(args.generated_on)

    raw_tasks = extract_raw_tasks()
    reviewed = review_tasks(raw_tasks, config)

    if len(reviewed) != config.expected_task_count:
        print(
            f"warning: expected {config.expected_task_count} tasks, found {len(reviewed)}",
            file=sys.stderr,
        )

    catalog_md.parent.mkdir(parents=True, exist_ok=True)
    catalog_json.parent.mkdir(parents=True, exist_ok=True)

    catalog_md.write_text(
        render_catalog_markdown(reviewed, config.lane_name, generated_on),
        encoding="utf-8",
    )
    catalog_json.write_text(
        serialize_catalog_json(reviewed, config.lane_name, generated_on),
        encoding="utf-8",
    )

    print(
        f"catalog_written tasks={len(reviewed)} md={catalog_md.as_posix()} json={catalog_json.as_posix()}"
    )

    if not args.create_issues:
        return 0

    ensure_labels(repo, config.label_defs)
    milestone_map = ensure_lane_milestones(
        repo,
        config.lane_milestone_titles,
        config.lane_milestone_due_on,
        config.lane_name,
    )
    milestone_map = fetch_milestone_map(repo)

    created, skipped = seed_issues(
        repo=repo,
        tasks=reviewed,
        milestone_map=milestone_map,
        lane_name=config.lane_name,
        sleep_seconds=sleep_seconds,
        limit=args.limit,
    )

    print(
        f"issue_seed_complete created={created} skipped_existing={skipped} total_reviewed={len(reviewed)}"
    )
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (ConfigError, GhError, RuntimeError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(2)
