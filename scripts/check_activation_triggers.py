#!/usr/bin/env python3
"""Evaluate deterministic activation triggers from offline snapshot inputs."""

from __future__ import annotations

import argparse
import json
import re
import sys
import textwrap
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_CATALOG_JSON = ROOT / "spec" / "planning" / "remaining_task_review_catalog.json"
DEFAULT_ACTIONABLE_STATUSES: tuple[str, ...] = ("open", "open-blocked", "blocked")
OPEN_BLOCKERS_TRIGGER_ID = "T5-OPEN-BLOCKERS"
ACTIVATION_SEED_CONTRACT_ID = "activation-seed-contract/v0.15"
TRIGGER_ORDER: tuple[str, ...] = (
    "T1-ISSUES",
    "T2-MILESTONES",
    "T3-ACTIONABLE-ROWS",
    OPEN_BLOCKERS_TRIGGER_ID,
)
TRIGGER_CONDITIONS: dict[str, str] = {
    "T1-ISSUES": "open issues > 0",
    "T2-MILESTONES": "open milestones > 0",
    "T3-ACTIONABLE-ROWS": "actionable catalog rows > 0",
    OPEN_BLOCKERS_TRIGGER_ID: "open blockers > 0",
}
OPEN_SNAPSHOT_OBJECT_KEY_ORDER: tuple[str, ...] = (
    "generated_at_utc",
    "source",
    "count",
    "open",
    "items",
    "issues",
    "milestones",
)
MILESTONE_ROW_KEY_ORDER: tuple[str, ...] = (
    "number",
    "title",
    "state",
    "description",
    "open_issues",
    "closed_issues",
    "url",
    "html_url",
    "created_at",
    "updated_at",
    "due_on",
    "closed_at",
)
OPEN_BLOCKERS_SNAPSHOT_KEY_ORDER: tuple[str, ...] = (
    "generated_at_utc",
    "source",
    "open_blocker_count",
    "count",
    "open_blockers",
)
OPEN_BLOCKER_ROW_KEY_ORDER: tuple[str, ...] = (
    "blocker_id",
    "source_path",
    "line_number",
    "line",
)
CATALOG_SNAPSHOT_OBJECT_KEY_ORDER: tuple[str, ...] = (
    "generated_at_utc",
    "source",
    "generated_on",
    "task_count",
    "count",
    "tasks",
)
CATALOG_TASK_ROW_KEY_ORDER: tuple[str, ...] = (
    "task_id",
    "title",
    "path",
    "line",
    "bucket",
    "lane",
    "lane_name",
    "milestone_title",
    "priority_label",
    "area_label",
    "type_label",
    "labels",
    "quality",
    "quality_gaps",
    "original",
    "cleaned",
    "objective",
    "deliverables",
    "acceptance_criteria",
    "dependencies",
    "validation_commands",
    "shard",
    "execution_status",
    "blocker_owner",
    "blocker_notes",
    "status",
    "execution_status_rationale",
    "execution_status_evidence_refs",
    "execution_status_override_source",
)
CATALOG_OPTIONAL_CONTRACT_TEXT_FIELDS: tuple[str, ...] = (
    "lane_name",
    "milestone_title",
)
CATALOG_OPTIONAL_EXECUTION_CONTRACT_TEXT_FIELDS: tuple[str, ...] = (
    "execution_status_rationale",
    "execution_status_override_source",
    "blocker_owner",
    "blocker_notes",
)
CATALOG_OPTIONAL_CONTRACT_STRING_ARRAY_FIELDS: tuple[str, ...] = (
    "labels",
    "dependencies",
    "validation_commands",
    "execution_status_evidence_refs",
)
DISPATCH_EVIDENCE_RELIABILITY_MILESTONE_PATTERN = re.compile(
    r"^v0\.15 Dispatch Evidence Reliability W([1-9]\d*)$"
)
DISPATCH_EVIDENCE_RELIABILITY_VALIDATION_FORBIDDEN_TOKENS: tuple[str, ...] = (
    "&&",
    "||",
    ";",
    "|",
    "<",
    ">",
)
DISPATCH_EVIDENCE_RELIABILITY_LANE_B_EVIDENCE_PREFIX = "spec/planning/evidence/lane_b/"
DISPATCH_EVIDENCE_CONTRACT_ROW_LABEL = "dispatch-evidence"
DISPATCH_EVIDENCE_RELIABILITY_W5_WEEK = 5
DISPATCH_EVIDENCE_RELIABILITY_W5_EVIDENCE_ARTIFACT = (
    "spec/planning/evidence/lane_b/v015_dispatch_evidence_reliability_w5_tooling_validation_20260225.md"
)
DISPATCH_EVIDENCE_RELIABILITY_W5_REQUIRED_DEPENDENCIES: tuple[str, ...] = (
    "#1071",
    "#1072",
)
DISPATCH_EVIDENCE_RELIABILITY_W5_REQUIRED_VALIDATION_COMMANDS: tuple[str, ...] = (
    "python -m pytest tests/tooling -q",
    "python scripts/check_dispatch_evidence_reliability_w5_contract.py",
    "python scripts/spec_lint.py",
)
RELEASE_SCALE_FOUNDATION_MILESTONE_PATTERN = re.compile(
    r"^v0\.16 Release and Scale Foundation W([1-9]\d*)$"
)
RELEASE_SCALE_CONTRACT_ROW_LABEL = "release-scale-foundation"
RELEASE_SCALE_FOUNDATION_W1_WEEK = 1
RELEASE_SCALE_FOUNDATION_W1_EVIDENCE_ARTIFACT = (
    "spec/planning/evidence/lane_b/v016_release_and_scale_foundation_w1_tooling_validation_20260225.md"
)
RELEASE_SCALE_FOUNDATION_W1_REQUIRED_DEPENDENCIES: tuple[str, ...] = (
    "#1081",
    "#1082",
)
RELEASE_SCALE_FOUNDATION_W1_REQUIRED_VALIDATION_COMMANDS: tuple[str, ...] = (
    "python -m pytest tests/tooling -q",
    "python scripts/check_release_and_scale_foundation_w1_contract.py",
    "python scripts/spec_lint.py",
)
RELEASE_SCALE_FOUNDATION_W2_WEEK = 2
RELEASE_SCALE_FOUNDATION_W2_EVIDENCE_ARTIFACT = (
    "spec/planning/evidence/lane_b/v016_release_and_scale_foundation_w2_tooling_validation_20260225.md"
)
RELEASE_SCALE_FOUNDATION_W2_REQUIRED_DEPENDENCIES: tuple[str, ...] = (
    "#1091",
    "#1092",
)
RELEASE_SCALE_FOUNDATION_W2_REQUIRED_VALIDATION_COMMANDS: tuple[str, ...] = (
    "python -m pytest tests/tooling -q",
    "python scripts/check_release_and_scale_foundation_w2_contract.py",
    "python scripts/spec_lint.py",
)
RELEASE_SCALE_FOUNDATION_W3_WEEK = 3
RELEASE_SCALE_FOUNDATION_W3_EVIDENCE_ARTIFACT = (
    "spec/planning/evidence/lane_b/v016_release_and_scale_foundation_w3_tooling_validation_20260225.md"
)
RELEASE_SCALE_FOUNDATION_W3_REQUIRED_DEPENDENCIES: tuple[str, ...] = (
    "#1101",
    "#1102",
)
RELEASE_SCALE_FOUNDATION_W3_REQUIRED_VALIDATION_COMMANDS: tuple[str, ...] = (
    "python -m pytest tests/tooling -q",
    "python scripts/check_release_and_scale_foundation_w3_contract.py",
    "python scripts/spec_lint.py",
)
RELEASE_SCALE_FOUNDATION_W4_WEEK = 4
RELEASE_SCALE_FOUNDATION_W4_EVIDENCE_ARTIFACT = (
    "spec/planning/evidence/lane_b/v016_release_and_scale_foundation_w4_tooling_validation_20260225.md"
)
RELEASE_SCALE_FOUNDATION_W4_REQUIRED_DEPENDENCIES: tuple[str, ...] = (
    "#1111",
    "#1112",
)
RELEASE_SCALE_FOUNDATION_W4_REQUIRED_VALIDATION_COMMANDS: tuple[str, ...] = (
    "python -m pytest tests/tooling -q",
    "python scripts/check_release_and_scale_foundation_w4_contract.py",
    "python scripts/spec_lint.py",
)
RELEASE_SCALE_FOUNDATION_W5_WEEK = 5
RELEASE_SCALE_FOUNDATION_W5_EVIDENCE_ARTIFACT = (
    "spec/planning/evidence/lane_b/v016_release_and_scale_foundation_w5_tooling_validation_20260225.md"
)
RELEASE_SCALE_FOUNDATION_W5_REQUIRED_DEPENDENCIES: tuple[str, ...] = (
    "#1121",
    "#1122",
)
RELEASE_SCALE_FOUNDATION_W5_REQUIRED_VALIDATION_COMMANDS: tuple[str, ...] = (
    "python -m pytest tests/tooling -q",
    "python scripts/check_release_and_scale_foundation_w5_contract.py",
    "python scripts/spec_lint.py",
)
T4_OVERLAY_OBJECT_KEY_ORDER: tuple[str, ...] = (
    "t4_new_scope_publish",
    "T4_NEW_SCOPE_PUBLISH",
)


class ActivationTriggerContractError(ValueError):
    """Fail-closed validation error for activation trigger contract inputs."""


class DispatchEvidenceContractError(ActivationTriggerContractError):
    """Fail-closed validation error for lane-B dispatch evidence contract rows."""


@dataclass(frozen=True)
class TriggerResult:
    trigger_id: str
    condition: str
    count: int
    fired: bool


@dataclass(frozen=True)
class T4OverlayState:
    new_scope_publish: bool
    source: str
    overlay_path: Path | None


@dataclass(frozen=True)
class SnapshotFreshnessState:
    requested: bool
    max_age_seconds: int | None
    generated_at_utc: str | None
    age_seconds: int | None
    fresh: bool | None


@dataclass(frozen=True)
class OpenBlockersState:
    count: int
    snapshot_path: Path | None


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def resolve_input_path(raw_path: Path) -> Path:
    if raw_path.is_absolute():
        return raw_path
    return ROOT / raw_path


def write_stdout(value: str) -> None:
    buffer = getattr(sys.stdout, "buffer", None)
    if buffer is not None:
        buffer.write(value.encode("utf-8"))
        return
    sys.stdout.write(value)


def normalize_status(raw_value: Any) -> str:
    if not isinstance(raw_value, str):
        return "missing"
    normalized = raw_value.strip().lower()
    if not normalized:
        return "missing"
    return normalized


def normalize_actionable_statuses(raw_values: Sequence[str] | None) -> tuple[str, ...]:
    if not raw_values:
        return DEFAULT_ACTIONABLE_STATUSES

    normalized: list[str] = []
    seen: set[str] = set()
    for raw in raw_values:
        status = raw.strip().lower()
        if not status:
            raise ValueError("actionable status filters must be non-empty")
        if status in seen:
            continue
        seen.add(status)
        normalized.append(status)

    if not normalized:
        raise ValueError("no actionable statuses were provided")
    return tuple(normalized)


def parse_non_negative_int_argument(raw_value: str) -> int:
    try:
        parsed = int(raw_value)
    except ValueError as exc:
        raise argparse.ArgumentTypeError("must be a non-negative integer") from exc
    if parsed < 0:
        raise argparse.ArgumentTypeError("must be a non-negative integer")
    return parsed


def load_json(path: Path, *, label: str) -> Any:
    if not path.exists():
        raise ValueError(f"{label} JSON file does not exist: {display_path(path)}")

    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except OSError as exc:
        raise ValueError(f"unable to read {label} JSON {display_path(path)}: {exc}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(f"invalid JSON in {label} file {display_path(path)}: {exc}") from exc


def parse_non_negative_count(raw_value: Any, *, context: str) -> int:
    if isinstance(raw_value, bool) or not isinstance(raw_value, int) or raw_value < 0:
        raise ValueError(f"{context} must be a non-negative integer")
    return raw_value


def parse_optional_non_negative_count(raw_value: Any, *, context: str) -> int | None:
    if raw_value is None:
        return None
    return parse_non_negative_count(raw_value, context=context)


def parse_positive_int(raw_value: Any, *, context: str) -> int:
    if isinstance(raw_value, bool) or not isinstance(raw_value, int) or raw_value <= 0:
        raise ValueError(f"{context} must be a positive integer")
    return raw_value


def parse_non_empty_string(raw_value: Any, *, context: str) -> str:
    if not isinstance(raw_value, str) or not raw_value.strip():
        raise ValueError(f"{context} must be a non-empty string")
    return raw_value.strip()


def parse_canonical_non_empty_string(raw_value: Any, *, context: str) -> str:
    if not isinstance(raw_value, str):
        raise ValueError(f"{context} must be a non-empty string")
    if not raw_value.strip():
        raise ValueError(f"{context} must be a non-empty string")
    if raw_value != raw_value.strip():
        raise ValueError(f"{context} must not contain leading/trailing whitespace")
    return raw_value


def parse_canonical_relative_posix_path(raw_value: Any, *, context: str) -> str:
    value = parse_canonical_non_empty_string(raw_value, context=context)
    if "\\" in value:
        raise ValueError(f"{context} must use '/' path separators")
    if value.startswith("/"):
        raise ValueError(f"{context} must be a relative path")
    if len(value) >= 3 and value[0].isalpha() and value[1] == ":" and value[2] == "/":
        raise ValueError(f"{context} must be a relative path")

    segments = value.split("/")
    if any(segment == "" for segment in segments):
        raise ValueError(
            f"{context} must be normalized (no empty path segments or trailing slash)"
        )
    if any(segment in {".", ".."} for segment in segments):
        raise ValueError(
            f"{context} must be normalized ('.' and '..' segments are not allowed)"
        )
    return value


def parse_optional_canonical_non_empty_string(
    raw_value: Any,
    *,
    context: str,
) -> str | None:
    if raw_value is None:
        return None
    return parse_canonical_non_empty_string(raw_value, context=context)


def parse_optional_canonical_string_array(raw_value: Any, *, context: str) -> list[str] | None:
    if raw_value is None:
        return None
    if not isinstance(raw_value, list):
        raise ValueError(f"{context} must be an array of non-empty strings")
    if not raw_value:
        raise ValueError(f"{context} must not be an empty array")

    entries: list[str] = []
    seen_entries: set[str] = set()
    for index, entry in enumerate(raw_value):
        value = parse_canonical_non_empty_string(
            entry,
            context=f"{context}[{index}]",
        )
        if value in seen_entries:
            raise ValueError(f"{context} contains duplicate entry {value!r}")
        seen_entries.add(value)
        entries.append(value)
    return entries


def sort_casefolded(values: Sequence[str]) -> list[str]:
    return sorted(values, key=lambda value: (value.casefold(), value))


def parse_dispatch_evidence_reference(raw_value: str, *, context: str) -> tuple[str, int]:
    if raw_value.startswith(("http://", "https://")):
        raise DispatchEvidenceContractError(
            f"{context} must be a local relative evidence reference with ':line'"
        )

    line_separator = raw_value.rfind(":")
    if line_separator <= 0:
        raise DispatchEvidenceContractError(f"{context} must include explicit ':line' suffix")

    relative_path_text = raw_value[:line_separator]
    line_text = raw_value[line_separator + 1 :]
    try:
        parse_canonical_relative_posix_path(
            relative_path_text,
            context=f"{context} path component",
        )
    except ValueError as exc:
        raise DispatchEvidenceContractError(str(exc)) from exc
    if not relative_path_text.endswith(".md"):
        raise DispatchEvidenceContractError(f"{context} path component must reference a '.md' file")
    try:
        line_number = parse_positive_int(
            int(line_text) if line_text.isdigit() else line_text,
            context=f"{context} line component",
        )
    except ValueError as exc:
        raise DispatchEvidenceContractError(str(exc)) from exc
    return relative_path_text, line_number


def ensure_dispatch_evidence_file_exists(
    *,
    path_text: str,
    context: str,
    catalog_path_text: str,
) -> None:
    evidence_path = ROOT / path_text
    if not evidence_path.exists():
        raise DispatchEvidenceContractError(
            f"{context} must reference existing evidence file {path_text!r}"
            f"{catalog_path_text}"
        )
    if not evidence_path.is_file():
        raise DispatchEvidenceContractError(
            f"{context} must reference a file path; observed={path_text!r}"
            f"{catalog_path_text}"
        )


def validate_lane_b_milestone_execution_contract(
    *,
    index: int,
    lane: str | None,
    milestone_title: str | None,
    override_source: str | None,
    string_array_values: dict[str, list[str]],
    catalog_path_text: str,
    milestone_pattern: re.Pattern[str],
    contract_row_label: str,
    required_week: int,
    required_week_label: str,
    required_dependencies: Sequence[str],
    required_validation_commands: Sequence[str],
    required_evidence_artifact: str,
) -> None:
    if lane != "B" or milestone_title is None:
        return
    milestone_match = milestone_pattern.fullmatch(milestone_title)
    if milestone_match is None:
        return
    milestone_week = int(milestone_match.group(1))

    validation_commands = string_array_values.get("validation_commands")
    if validation_commands is None:
        raise DispatchEvidenceContractError(
            "catalog task row "
            f"{index} field 'validation_commands' is required for lane-B "
            f"{milestone_title!r} contract rows{catalog_path_text}"
        )
    expected_validation_commands = sort_casefolded(validation_commands)
    if validation_commands != expected_validation_commands:
        raise DispatchEvidenceContractError(
            "catalog task row "
            f"{index} field 'validation_commands' must be sorted deterministically "
            "(case-insensitive lexicographic order) for lane-B "
            f"{contract_row_label} contract rows{catalog_path_text}: "
            f"expected={expected_validation_commands!r} observed={validation_commands!r}"
        )
    for command_index, command in enumerate(validation_commands):
        if any(control in command for control in ("\n", "\r", "\t")):
            raise DispatchEvidenceContractError(
                f"catalog task row {index} field 'validation_commands[{command_index}]' "
                "must be single-line text without control characters "
                f"for lane-B {contract_row_label} contract rows{catalog_path_text}"
            )
        for forbidden_token in DISPATCH_EVIDENCE_RELIABILITY_VALIDATION_FORBIDDEN_TOKENS:
            if forbidden_token in command:
                raise DispatchEvidenceContractError(
                    f"catalog task row {index} field 'validation_commands[{command_index}]' "
                    f"contains forbidden shell token {forbidden_token!r}; "
                    f"{contract_row_label} lane-B contract commands must be single "
                    f"deterministic commands{catalog_path_text}"
                )

    evidence_refs = string_array_values.get("execution_status_evidence_refs")
    if evidence_refs is None:
        raise DispatchEvidenceContractError(
            "catalog task row "
            f"{index} field 'execution_status_evidence_refs' is required for lane-B "
            f"{milestone_title!r} contract rows{catalog_path_text}"
        )
    parsed_evidence_refs: list[tuple[str, int, str]] = []
    for evidence_index, evidence_ref in enumerate(evidence_refs):
        evidence_path, evidence_line = parse_dispatch_evidence_reference(
            evidence_ref,
            context=(
                f"catalog task row {index} field "
                f"'execution_status_evidence_refs[{evidence_index}]'"
            ),
        )
        parsed_evidence_refs.append((evidence_path, evidence_line, evidence_ref))
        if not evidence_path.startswith(DISPATCH_EVIDENCE_RELIABILITY_LANE_B_EVIDENCE_PREFIX):
            raise DispatchEvidenceContractError(
                "catalog task row "
                f"{index} field 'execution_status_evidence_refs[{evidence_index}]' must reference "
                f"lane-B evidence under '{DISPATCH_EVIDENCE_RELIABILITY_LANE_B_EVIDENCE_PREFIX}'"
                f"{catalog_path_text}"
            )
        ensure_dispatch_evidence_file_exists(
            path_text=evidence_path,
            context=(
                f"catalog task row {index} field "
                f"'execution_status_evidence_refs[{evidence_index}]'"
            ),
            catalog_path_text=catalog_path_text,
        )

    expected_by_path_and_line = [
        entry[2]
        for entry in sorted(
            parsed_evidence_refs,
            key=lambda entry: (
                entry[0].casefold(),
                entry[0],
                entry[1],
                entry[2].casefold(),
                entry[2],
            ),
        )
    ]
    if evidence_refs != expected_by_path_and_line:
        raise DispatchEvidenceContractError(
            "catalog task row "
            f"{index} field 'execution_status_evidence_refs' must be sorted "
            "deterministically by evidence path and numeric ':line' for lane-B "
            f"{contract_row_label} contract rows{catalog_path_text}: "
            f"expected={expected_by_path_and_line!r} observed={evidence_refs!r}"
        )

    if override_source is None:
        raise DispatchEvidenceContractError(
            "catalog task row "
            f"{index} field 'execution_status_override_source' is required for lane-B "
            f"{milestone_title!r} contract rows{catalog_path_text}"
        )
    try:
        override_source_path = parse_canonical_relative_posix_path(
            override_source,
            context=f"catalog task row {index} field 'execution_status_override_source'",
        )
    except ValueError as exc:
        raise DispatchEvidenceContractError(str(exc)) from exc
    if not override_source_path.endswith(".md"):
        raise DispatchEvidenceContractError(
            "catalog task row "
            f"{index} field 'execution_status_override_source' must reference a '.md' file"
            f"{catalog_path_text}"
        )
    if not override_source_path.startswith(DISPATCH_EVIDENCE_RELIABILITY_LANE_B_EVIDENCE_PREFIX):
        raise DispatchEvidenceContractError(
            "catalog task row "
            f"{index} field 'execution_status_override_source' must reference lane-B "
            f"evidence under '{DISPATCH_EVIDENCE_RELIABILITY_LANE_B_EVIDENCE_PREFIX}'"
            f"{catalog_path_text}"
        )
    evidence_paths = sorted(
        {entry[0] for entry in parsed_evidence_refs},
        key=lambda value: (value.casefold(), value),
    )
    if override_source_path not in evidence_paths:
        raise DispatchEvidenceContractError(
            "catalog task row "
            f"{index} field 'execution_status_override_source' must match one of the "
            "'execution_status_evidence_refs' path components for lane-B "
            f"{contract_row_label} contract rows{catalog_path_text}: "
            f"override_source={override_source_path!r} evidence_paths={evidence_paths!r}"
        )
    ensure_dispatch_evidence_file_exists(
        path_text=override_source_path,
        context=f"catalog task row {index} field 'execution_status_override_source'",
        catalog_path_text=catalog_path_text,
    )

    if milestone_week != required_week:
        return

    dependencies = string_array_values.get("dependencies")
    if dependencies is None:
        raise DispatchEvidenceContractError(
            "catalog task row "
            f"{index} field 'dependencies' is required for lane-B "
            f"{milestone_title!r} contract rows{catalog_path_text}"
        )
    expected_dependencies = list(required_dependencies)
    if dependencies != expected_dependencies:
        raise DispatchEvidenceContractError(
            "catalog task row "
            f"{index} field 'dependencies' must match deterministic {required_week_label} "
            f"dependency set {expected_dependencies!r} for lane-B {contract_row_label} "
            f"contract rows{catalog_path_text}: observed={dependencies!r}"
        )

    missing_required_commands = [
        command for command in required_validation_commands if command not in validation_commands
    ]
    if missing_required_commands:
        raise DispatchEvidenceContractError(
            "catalog task row "
            f"{index} field 'validation_commands' is missing required "
            f"{required_week_label} command(s) {missing_required_commands!r} "
            f"for lane-B {contract_row_label} contract rows{catalog_path_text}"
        )

    if override_source_path != required_evidence_artifact:
        raise DispatchEvidenceContractError(
            "catalog task row "
            f"{index} field 'execution_status_override_source' must equal {required_week_label} "
            "lane-B tooling evidence artifact "
            f"{required_evidence_artifact!r} for lane-B {contract_row_label} "
            f"contract rows{catalog_path_text}: observed={override_source_path!r}"
        )

    if not any(
        evidence_path == required_evidence_artifact
        for evidence_path, _, _ in parsed_evidence_refs
    ):
        raise DispatchEvidenceContractError(
            "catalog task row "
            f"{index} field 'execution_status_evidence_refs' must include {required_week_label} "
            "lane-B tooling evidence artifact "
            f"{required_evidence_artifact!r} for lane-B {contract_row_label} "
            f"contract rows{catalog_path_text}"
        )


def validate_dispatch_lane_b_execution_contract(
    *,
    index: int,
    lane: str | None,
    milestone_title: str | None,
    override_source: str | None,
    string_array_values: dict[str, list[str]],
    catalog_path_text: str,
) -> None:
    validate_lane_b_milestone_execution_contract(
        index=index,
        lane=lane,
        milestone_title=milestone_title,
        override_source=override_source,
        string_array_values=string_array_values,
        catalog_path_text=catalog_path_text,
        milestone_pattern=DISPATCH_EVIDENCE_RELIABILITY_MILESTONE_PATTERN,
        contract_row_label=DISPATCH_EVIDENCE_CONTRACT_ROW_LABEL,
        required_week=DISPATCH_EVIDENCE_RELIABILITY_W5_WEEK,
        required_week_label="W5",
        required_dependencies=DISPATCH_EVIDENCE_RELIABILITY_W5_REQUIRED_DEPENDENCIES,
        required_validation_commands=DISPATCH_EVIDENCE_RELIABILITY_W5_REQUIRED_VALIDATION_COMMANDS,
        required_evidence_artifact=DISPATCH_EVIDENCE_RELIABILITY_W5_EVIDENCE_ARTIFACT,
    )


def validate_release_scale_lane_b_execution_contract(
    *,
    index: int,
    lane: str | None,
    milestone_title: str | None,
    override_source: str | None,
    string_array_values: dict[str, list[str]],
    catalog_path_text: str,
) -> None:
    validate_lane_b_milestone_execution_contract(
        index=index,
        lane=lane,
        milestone_title=milestone_title,
        override_source=override_source,
        string_array_values=string_array_values,
        catalog_path_text=catalog_path_text,
        milestone_pattern=RELEASE_SCALE_FOUNDATION_MILESTONE_PATTERN,
        contract_row_label=RELEASE_SCALE_CONTRACT_ROW_LABEL,
        required_week=RELEASE_SCALE_FOUNDATION_W1_WEEK,
        required_week_label="W1",
        required_dependencies=RELEASE_SCALE_FOUNDATION_W1_REQUIRED_DEPENDENCIES,
        required_validation_commands=RELEASE_SCALE_FOUNDATION_W1_REQUIRED_VALIDATION_COMMANDS,
        required_evidence_artifact=RELEASE_SCALE_FOUNDATION_W1_EVIDENCE_ARTIFACT,
    )
    validate_lane_b_milestone_execution_contract(
        index=index,
        lane=lane,
        milestone_title=milestone_title,
        override_source=override_source,
        string_array_values=string_array_values,
        catalog_path_text=catalog_path_text,
        milestone_pattern=RELEASE_SCALE_FOUNDATION_MILESTONE_PATTERN,
        contract_row_label=RELEASE_SCALE_CONTRACT_ROW_LABEL,
        required_week=RELEASE_SCALE_FOUNDATION_W2_WEEK,
        required_week_label="W2",
        required_dependencies=RELEASE_SCALE_FOUNDATION_W2_REQUIRED_DEPENDENCIES,
        required_validation_commands=RELEASE_SCALE_FOUNDATION_W2_REQUIRED_VALIDATION_COMMANDS,
        required_evidence_artifact=RELEASE_SCALE_FOUNDATION_W2_EVIDENCE_ARTIFACT,
    )
    validate_lane_b_milestone_execution_contract(
        index=index,
        lane=lane,
        milestone_title=milestone_title,
        override_source=override_source,
        string_array_values=string_array_values,
        catalog_path_text=catalog_path_text,
        milestone_pattern=RELEASE_SCALE_FOUNDATION_MILESTONE_PATTERN,
        contract_row_label=RELEASE_SCALE_CONTRACT_ROW_LABEL,
        required_week=RELEASE_SCALE_FOUNDATION_W3_WEEK,
        required_week_label="W3",
        required_dependencies=RELEASE_SCALE_FOUNDATION_W3_REQUIRED_DEPENDENCIES,
        required_validation_commands=RELEASE_SCALE_FOUNDATION_W3_REQUIRED_VALIDATION_COMMANDS,
        required_evidence_artifact=RELEASE_SCALE_FOUNDATION_W3_EVIDENCE_ARTIFACT,
    )
    validate_lane_b_milestone_execution_contract(
        index=index,
        lane=lane,
        milestone_title=milestone_title,
        override_source=override_source,
        string_array_values=string_array_values,
        catalog_path_text=catalog_path_text,
        milestone_pattern=RELEASE_SCALE_FOUNDATION_MILESTONE_PATTERN,
        contract_row_label=RELEASE_SCALE_CONTRACT_ROW_LABEL,
        required_week=RELEASE_SCALE_FOUNDATION_W4_WEEK,
        required_week_label="W4",
        required_dependencies=RELEASE_SCALE_FOUNDATION_W4_REQUIRED_DEPENDENCIES,
        required_validation_commands=RELEASE_SCALE_FOUNDATION_W4_REQUIRED_VALIDATION_COMMANDS,
        required_evidence_artifact=RELEASE_SCALE_FOUNDATION_W4_EVIDENCE_ARTIFACT,
    )
    validate_lane_b_milestone_execution_contract(
        index=index,
        lane=lane,
        milestone_title=milestone_title,
        override_source=override_source,
        string_array_values=string_array_values,
        catalog_path_text=catalog_path_text,
        milestone_pattern=RELEASE_SCALE_FOUNDATION_MILESTONE_PATTERN,
        contract_row_label=RELEASE_SCALE_CONTRACT_ROW_LABEL,
        required_week=RELEASE_SCALE_FOUNDATION_W5_WEEK,
        required_week_label="W5",
        required_dependencies=RELEASE_SCALE_FOUNDATION_W5_REQUIRED_DEPENDENCIES,
        required_validation_commands=RELEASE_SCALE_FOUNDATION_W5_REQUIRED_VALIDATION_COMMANDS,
        required_evidence_artifact=RELEASE_SCALE_FOUNDATION_W5_EVIDENCE_ARTIFACT,
    )


def validate_object_key_order(
    payload: dict[str, Any],
    *,
    expected_key_order: Sequence[str],
    context: str,
) -> None:
    observed_keys = tuple(payload.keys())
    unexpected_keys = [key for key in observed_keys if key not in expected_key_order]
    if unexpected_keys:
        raise ValueError(
            f"{context} contains unexpected field(s) {unexpected_keys!r}; "
            f"allowed fields are {list(expected_key_order)!r}"
        )

    expected_observed_keys = tuple(key for key in expected_key_order if key in payload)
    if observed_keys != expected_observed_keys:
        raise ValueError(
            f"{context} key order drift: "
            f"expected={list(expected_observed_keys)!r} observed={list(observed_keys)!r}"
        )


def ensure_snapshot_row_objects(rows: Sequence[Any], *, label: str, context: str) -> None:
    for index, row in enumerate(rows):
        if not isinstance(row, dict):
            raise ValueError(f"{label} snapshot {context}[{index}] must be an object")


def validate_milestone_snapshot_rows(
    rows: Sequence[Any],
    *,
    context: str,
    snapshot_path: Path | None = None,
) -> None:
    snapshot_path_text = (
        f" in {display_path(snapshot_path)}" if snapshot_path is not None else ""
    )
    observed_numbers: list[int] = []
    seen_numbers: set[int] = set()
    for index, row in enumerate(rows):
        if not isinstance(row, dict):
            raise ValueError(
                f"open milestones snapshot {context}[{index}] must be an object"
                f"{snapshot_path_text}"
            )
        validate_object_key_order(
            row,
            expected_key_order=MILESTONE_ROW_KEY_ORDER,
            context=(
                f"open milestones snapshot {context}[{index}] object"
                f"{snapshot_path_text}"
            ),
        )
        milestone_number = parse_positive_int(
            row.get("number"),
            context=f"open milestones snapshot {context}[{index}].number",
        )
        parse_canonical_non_empty_string(
            row.get("title"),
            context=f"open milestones snapshot {context}[{index}].title",
        )
        parse_optional_canonical_non_empty_string(
            row.get("state"),
            context=f"open milestones snapshot {context}[{index}].state",
        )
        parse_optional_canonical_non_empty_string(
            row.get("description"),
            context=f"open milestones snapshot {context}[{index}].description",
        )
        parse_optional_non_negative_count(
            row.get("open_issues"),
            context=f"open milestones snapshot {context}[{index}].open_issues",
        )
        parse_optional_non_negative_count(
            row.get("closed_issues"),
            context=f"open milestones snapshot {context}[{index}].closed_issues",
        )
        parse_optional_canonical_non_empty_string(
            row.get("url"),
            context=f"open milestones snapshot {context}[{index}].url",
        )
        parse_optional_canonical_non_empty_string(
            row.get("html_url"),
            context=f"open milestones snapshot {context}[{index}].html_url",
        )
        parse_optional_canonical_non_empty_string(
            row.get("created_at"),
            context=f"open milestones snapshot {context}[{index}].created_at",
        )
        parse_optional_canonical_non_empty_string(
            row.get("updated_at"),
            context=f"open milestones snapshot {context}[{index}].updated_at",
        )
        parse_optional_canonical_non_empty_string(
            row.get("due_on"),
            context=f"open milestones snapshot {context}[{index}].due_on",
        )
        parse_optional_canonical_non_empty_string(
            row.get("closed_at"),
            context=f"open milestones snapshot {context}[{index}].closed_at",
        )

        if milestone_number in seen_numbers:
            raise ValueError(
                "open milestones snapshot rows contain duplicate milestone number "
                f"{milestone_number}{snapshot_path_text}"
            )
        seen_numbers.add(milestone_number)
        observed_numbers.append(milestone_number)

    if observed_numbers != sorted(observed_numbers):
        raise ValueError(
            "open milestones snapshot rows must be sorted by 'number'"
            f"{snapshot_path_text}"
        )


def utc_now() -> datetime:
    return datetime.now(timezone.utc).replace(microsecond=0)


def json_shape_name(value: Any) -> str:
    if isinstance(value, dict):
        return "object"
    if isinstance(value, list):
        return "array"
    if value is None:
        return "null"
    return type(value).__name__


def format_utc_timestamp(value: datetime) -> str:
    normalized = value.astimezone(timezone.utc).replace(microsecond=0)
    return normalized.isoformat().replace("+00:00", "Z")


def parse_generated_at_utc(raw_value: Any, *, label: str) -> datetime:
    if not isinstance(raw_value, str) or not raw_value.strip():
        raise ValueError(
            f"{label} snapshot field 'generated_at_utc' must be a non-empty ISO-8601 timestamp"
        )

    normalized = raw_value.strip()
    if normalized.endswith("Z"):
        normalized = f"{normalized[:-1]}+00:00"

    try:
        parsed = datetime.fromisoformat(normalized)
    except ValueError as exc:
        raise ValueError(
            f"{label} snapshot field 'generated_at_utc' must be a valid ISO-8601 timestamp"
        ) from exc

    if parsed.tzinfo is None:
        raise ValueError(
            f"{label} snapshot field 'generated_at_utc' must include a timezone (for example 'Z')"
        )

    return parsed.astimezone(timezone.utc)


def count_open_snapshot_entries(
    payload: Any,
    *,
    label: str,
    snapshot_path: Path | None = None,
) -> int:
    if isinstance(payload, list):
        ensure_snapshot_row_objects(payload, label=label, context="entries")
        if label == "open milestones":
            validate_milestone_snapshot_rows(
                payload,
                context="entries",
                snapshot_path=snapshot_path,
            )
        return len(payload)

    if isinstance(payload, dict):
        snapshot_path_text = (
            f" in {display_path(snapshot_path)}" if snapshot_path is not None else ""
        )
        validate_object_key_order(
            payload,
            expected_key_order=OPEN_SNAPSHOT_OBJECT_KEY_ORDER,
            context=f"{label} snapshot object{snapshot_path_text}",
        )

        has_generated_at_utc = "generated_at_utc" in payload
        has_source = "source" in payload
        if has_generated_at_utc != has_source:
            raise ValueError(
                f"{label} snapshot object must include both 'generated_at_utc' and "
                f"'source' when either field is present{snapshot_path_text}"
            )

        if has_generated_at_utc:
            try:
                parse_generated_at_utc(payload["generated_at_utc"], label=label)
            except ValueError as exc:
                raise ValueError(f"{exc}{snapshot_path_text}") from exc

            try:
                parse_canonical_non_empty_string(
                    payload["source"],
                    context=f"{label} snapshot field 'source'",
                )
            except ValueError as exc:
                raise ValueError(f"{exc}{snapshot_path_text}") from exc

        array_lengths: dict[str, int] = {}
        for key in ("open", "items", "issues", "milestones"):
            if key not in payload:
                continue
            raw_items = payload[key]
            if not isinstance(raw_items, list):
                raise ValueError(f"{label} snapshot field '{key}' must be an array")
            ensure_snapshot_row_objects(
                raw_items,
                label=label,
                context=f"field '{key}'",
            )
            if label == "open milestones":
                validate_milestone_snapshot_rows(
                    raw_items,
                    context=f"field '{key}'",
                    snapshot_path=snapshot_path,
                )
            array_lengths[key] = len(raw_items)

        if array_lengths:
            primary_key = next(iter(array_lengths))
            primary_length = array_lengths[primary_key]
            for key, length in array_lengths.items():
                if length != primary_length:
                    raise ValueError(
                        f"{label} snapshot parity mismatch: field '{primary_key}' length "
                        f"{primary_length} != field '{key}' length {length}"
                    )

            if "count" in payload:
                declared_count = parse_non_negative_count(
                    payload["count"],
                    context=f"{label} snapshot field 'count'",
                )
                if declared_count != primary_length:
                    raise ValueError(
                        f"{label} snapshot parity mismatch: declared count {declared_count} "
                        f"!= discovered count {primary_length}"
                    )

            return primary_length

        if "count" in payload:
            return parse_non_negative_count(payload["count"], context=f"{label} snapshot field 'count'")

    raise ValueError(
        f"{label} snapshot must be either an array or an object with "
        "'open'/'items'/'issues'/'milestones' array (or non-negative 'count')"
    )


def parse_open_blockers_count(payload: Any, *, snapshot_path: Path) -> int:
    if isinstance(payload, list):
        blocker_sort_keys: list[tuple[str, str, int]] = []
        seen_blocker_rows: set[tuple[str, str, int]] = set()
        for index, entry in enumerate(payload):
            if not isinstance(entry, dict):
                raise ValueError(
                    f"open blockers snapshot entries[{index}] must be an object "
                    f"in {display_path(snapshot_path)}"
                )
            blocker_id = parse_canonical_non_empty_string(
                entry.get("blocker_id"),
                context=f"open blockers snapshot field 'entries[{index}].blocker_id'",
            )
            source_path = parse_canonical_relative_posix_path(
                entry.get("source_path"),
                context=f"open blockers snapshot field 'entries[{index}].source_path'",
            )
            has_line_number = "line_number" in entry
            has_legacy_line = "line" in entry
            if has_line_number and has_legacy_line:
                canonical_line_number = parse_positive_int(
                    entry.get("line_number"),
                    context=f"open blockers snapshot field 'entries[{index}].line_number'",
                )
                legacy_line_number = parse_positive_int(
                    entry.get("line"),
                    context=f"open blockers snapshot field 'entries[{index}].line'",
                )
                if canonical_line_number != legacy_line_number:
                    raise ValueError(
                        "open blockers snapshot line alias mismatch in "
                        f"{display_path(snapshot_path)} for row index {index}: "
                        f"line_number={canonical_line_number} line={legacy_line_number}"
                    )
                line_number = canonical_line_number
            else:
                line_number = parse_positive_int(
                    entry.get("line_number", entry.get("line")),
                    context=(
                        f"open blockers snapshot field 'entries[{index}].line_number' "
                        "(or legacy 'line')"
                    ),
                )
            blocker_row_key = (blocker_id, source_path, line_number)
            if blocker_row_key in seen_blocker_rows:
                raise ValueError(
                    "open blockers snapshot array contains duplicate blocker row "
                    f"{blocker_row_key!r} in {display_path(snapshot_path)}"
                )
            seen_blocker_rows.add(blocker_row_key)
            blocker_sort_keys.append(blocker_row_key)

        expected_order = sorted(
            blocker_sort_keys,
            key=lambda row: (
                row[1].casefold(),
                row[1],
                row[2],
                row[0].casefold(),
                row[0],
            ),
        )
        if blocker_sort_keys != expected_order:
            raise ValueError(
                "open blockers snapshot array must be sorted by "
                "'source_path', then line number, then 'blocker_id' in "
                f"{display_path(snapshot_path)}"
            )
        return len(payload)

    if not isinstance(payload, dict):
        raise ValueError(
            "open blockers snapshot must be either an array of blocker rows or an object with "
            "'open_blockers' array and 'open_blocker_count' (or 'count')"
        )

    validate_object_key_order(
        payload,
        expected_key_order=OPEN_BLOCKERS_SNAPSHOT_KEY_ORDER,
        context=f"open blockers snapshot object in {display_path(snapshot_path)}",
    )

    if "open_blockers" not in payload:
        raise ValueError(
            "open blockers snapshot object must include 'open_blockers' array "
            f"in {display_path(snapshot_path)}"
        )

    has_generated_at_utc = "generated_at_utc" in payload
    has_source = "source" in payload
    if has_generated_at_utc != has_source:
        raise ValueError(
            "open blockers snapshot object must include both 'generated_at_utc' and "
            f"'source' when either field is present in {display_path(snapshot_path)}"
        )

    if has_generated_at_utc:
        try:
            parse_generated_at_utc(
                payload["generated_at_utc"],
                label="open blockers",
            )
        except ValueError as exc:
            raise ValueError(f"{exc} in {display_path(snapshot_path)}") from exc
        try:
            parse_canonical_non_empty_string(
                payload["source"],
                context="open blockers snapshot field 'source'",
            )
        except ValueError as exc:
            raise ValueError(f"{exc} in {display_path(snapshot_path)}") from exc

    raw_open_blockers = payload["open_blockers"]
    if not isinstance(raw_open_blockers, list):
        raise ValueError(
            "open blockers snapshot field 'open_blockers' must be an array "
            f"in {display_path(snapshot_path)}"
        )

    blocker_sort_keys: list[tuple[str, str, int]] = []
    seen_blocker_rows: set[tuple[str, str, int]] = set()
    for index, entry in enumerate(raw_open_blockers):
        if not isinstance(entry, dict):
            raise ValueError(
                f"open blockers snapshot field 'open_blockers[{index}]' "
                f"must be an object in {display_path(snapshot_path)}"
            )
        validate_object_key_order(
            entry,
            expected_key_order=OPEN_BLOCKER_ROW_KEY_ORDER,
            context=(
                "open blockers snapshot field "
                f"'open_blockers[{index}]' object in {display_path(snapshot_path)}"
            ),
        )
        blocker_id = parse_canonical_non_empty_string(
            entry.get("blocker_id"),
            context=f"open blockers snapshot field 'open_blockers[{index}].blocker_id'",
        )
        source_path = parse_canonical_relative_posix_path(
            entry.get("source_path"),
            context=f"open blockers snapshot field 'open_blockers[{index}].source_path'",
        )
        has_line_number = "line_number" in entry
        has_legacy_line = "line" in entry
        if has_line_number and has_legacy_line:
            canonical_line_number = parse_positive_int(
                entry.get("line_number"),
                context=f"open blockers snapshot field 'open_blockers[{index}].line_number'",
            )
            legacy_line_number = parse_positive_int(
                entry.get("line"),
                context=f"open blockers snapshot field 'open_blockers[{index}].line'",
            )
            if canonical_line_number != legacy_line_number:
                raise ValueError(
                    "open blockers snapshot line alias mismatch in "
                    f"{display_path(snapshot_path)} for row index {index}: "
                    f"line_number={canonical_line_number} line={legacy_line_number}"
                )
            line_number = canonical_line_number
        else:
            line_number = parse_positive_int(
                entry.get("line_number", entry.get("line")),
                context=(
                    f"open blockers snapshot field 'open_blockers[{index}].line_number' "
                    "(or legacy 'line')"
                ),
            )
        blocker_row_key = (blocker_id, source_path, line_number)
        if blocker_row_key in seen_blocker_rows:
            raise ValueError(
                "open blockers snapshot field 'open_blockers' contains duplicate blocker row "
                f"{blocker_row_key!r} in {display_path(snapshot_path)}"
            )
        seen_blocker_rows.add(blocker_row_key)
        blocker_sort_keys.append(blocker_row_key)

    expected_order = sorted(
        blocker_sort_keys,
        key=lambda row: (
            row[1].casefold(),
            row[1],
            row[2],
            row[0].casefold(),
            row[0],
        ),
    )
    if blocker_sort_keys != expected_order:
        raise ValueError(
            "open blockers snapshot field 'open_blockers' must be sorted by "
            "'source_path', then line number, then 'blocker_id' in "
            f"{display_path(snapshot_path)}"
        )

    declared_count: int | None = None
    if "open_blocker_count" in payload:
        declared_count = parse_non_negative_count(
            payload["open_blocker_count"],
            context="open blockers snapshot field 'open_blocker_count'",
        )
    if "count" in payload:
        legacy_count = parse_non_negative_count(
            payload["count"],
            context="open blockers snapshot field 'count'",
        )
        if declared_count is None:
            declared_count = legacy_count
        elif declared_count != legacy_count:
            raise ValueError(
                "open blockers snapshot fields 'open_blocker_count' and 'count' must match "
                f"in {display_path(snapshot_path)}"
            )
    if declared_count is None:
        raise ValueError(
            "open blockers snapshot object must include non-negative integer "
            "'open_blocker_count' (or legacy 'count') "
            f"in {display_path(snapshot_path)}"
        )

    discovered_count = len(raw_open_blockers)
    if declared_count != discovered_count:
        raise ValueError(
            "open blockers snapshot count mismatch in "
            f"{display_path(snapshot_path)}: declared={declared_count} "
            f"discovered={discovered_count}"
        )

    return declared_count


def evaluate_snapshot_freshness(
    *,
    payload: Any,
    label: str,
    snapshot_path: Path,
    max_age_seconds: int | None,
    flag_name: str,
    now_utc: datetime,
) -> SnapshotFreshnessState:
    if max_age_seconds is None:
        return SnapshotFreshnessState(
            requested=False,
            max_age_seconds=None,
            generated_at_utc=None,
            age_seconds=None,
            fresh=None,
        )

    if not isinstance(payload, dict):
        raise ValueError(
            f"{label} snapshot freshness check requires an object payload with "
            f"'generated_at_utc'. Found {json_shape_name(payload)} in {display_path(snapshot_path)}. "
            f"Provide an object snapshot or omit {flag_name}."
        )

    if "generated_at_utc" not in payload:
        raise ValueError(
            f"{label} snapshot freshness check requested via {flag_name}, but "
            f"{display_path(snapshot_path)} is missing object field 'generated_at_utc'. "
            f"Add the field or omit {flag_name}."
        )

    raw_source = payload.get("source")
    if not isinstance(raw_source, str) or not raw_source.strip():
        raise ValueError(
            f"{label} snapshot freshness check requested via {flag_name}, but "
            f"{display_path(snapshot_path)} is missing non-empty string field 'source'. "
            f"Add provenance field 'source' or omit {flag_name}."
        )

    try:
        generated_at_utc = parse_generated_at_utc(payload["generated_at_utc"], label=label)
    except ValueError as exc:
        raise ValueError(
            f"{label} snapshot freshness check requested via {flag_name} failed for "
            f"{display_path(snapshot_path)}: {exc}. Fix 'generated_at_utc' or omit {flag_name}."
        ) from exc
    age_seconds = int((now_utc - generated_at_utc).total_seconds())
    fresh = age_seconds <= max_age_seconds
    generated_at_text = format_utc_timestamp(generated_at_utc)

    if not fresh:
        raise ValueError(
            f"{label} snapshot freshness check failed for {display_path(snapshot_path)}: "
            f"age_seconds={age_seconds} exceeds max_age_seconds={max_age_seconds} "
            f"(generated_at_utc={generated_at_text}, now_utc={format_utc_timestamp(now_utc)}). "
            f"Refresh the snapshot or increase {flag_name}."
        )

    return SnapshotFreshnessState(
        requested=True,
        max_age_seconds=max_age_seconds,
        generated_at_utc=generated_at_text,
        age_seconds=age_seconds,
        fresh=True,
    )


def validate_catalog_contract_fields(
    row: dict[str, Any],
    *,
    index: int,
    catalog_path_text: str,
) -> None:
    optional_text_values: dict[str, str] = {}
    optional_execution_text_values: dict[str, str] = {}
    has_path = "path" in row
    has_line = "line" in row
    if has_path != has_line:
        raise ValueError(
            f"catalog task row {index} fields 'path' and 'line' "
            f"must either both be present or both be absent{catalog_path_text}"
        )
    if has_path:
        parse_canonical_relative_posix_path(
            row.get("path"),
            context=f"catalog task row {index} field 'path'",
        )
        parse_positive_int(
            row.get("line"),
            context=f"catalog task row {index} field 'line'",
        )

    lane: str | None = None
    if "lane" in row:
        lane = parse_canonical_non_empty_string(
            row.get("lane"),
            context=f"catalog task row {index} field 'lane'",
        )

    for field in CATALOG_OPTIONAL_CONTRACT_TEXT_FIELDS:
        if field not in row:
            continue
        value = parse_canonical_non_empty_string(
            row.get(field),
            context=f"catalog task row {index} field {field!r}",
        )
        optional_text_values[field] = value
    for field in CATALOG_OPTIONAL_EXECUTION_CONTRACT_TEXT_FIELDS:
        if field not in row:
            continue
        value = parse_canonical_non_empty_string(
            row.get(field),
            context=f"catalog task row {index} field {field!r}",
        )
        optional_execution_text_values[field] = value

    string_array_values: dict[str, list[str]] = {}
    for field in CATALOG_OPTIONAL_CONTRACT_STRING_ARRAY_FIELDS:
        if field not in row:
            continue
        values = parse_optional_canonical_string_array(
            row.get(field),
            context=f"catalog task row {index} field {field!r}",
        )
        assert values is not None
        string_array_values[field] = values

    labels = string_array_values.get("labels")
    if lane is not None and labels is not None:
        lane_label = f"lane:{lane}"
        if lane_label not in labels:
            raise ValueError(
                "catalog task lane label alias mismatch"
                f"{catalog_path_text} for row index {index}: "
                f"expected label {lane_label!r} in field 'labels'"
            )

    if "milestone_title" in row:
        for required_field in ("validation_commands", "execution_status_evidence_refs"):
            if required_field not in string_array_values:
                raise ValueError(
                    f"catalog task row {index} field 'milestone_title' requires non-empty "
                    f"field {required_field!r}{catalog_path_text}"
                )

    validate_dispatch_lane_b_execution_contract(
        index=index,
        lane=lane,
        milestone_title=optional_text_values.get("milestone_title"),
        override_source=optional_execution_text_values.get("execution_status_override_source"),
        string_array_values=string_array_values,
        catalog_path_text=catalog_path_text,
    )
    validate_release_scale_lane_b_execution_contract(
        index=index,
        lane=lane,
        milestone_title=optional_text_values.get("milestone_title"),
        override_source=optional_execution_text_values.get("execution_status_override_source"),
        string_array_values=string_array_values,
        catalog_path_text=catalog_path_text,
    )

    execution_rationale_present = "execution_status_rationale" in row
    execution_override_source_present = "execution_status_override_source" in row
    if execution_rationale_present != execution_override_source_present:
        raise ValueError(
            f"catalog task row {index} fields 'execution_status_rationale' and "
            f"'execution_status_override_source' must either both be present "
            f"or both be absent{catalog_path_text}"
        )

    blocker_owner_present = "blocker_owner" in row
    blocker_notes_present = "blocker_notes" in row
    if blocker_owner_present != blocker_notes_present:
        raise ValueError(
            f"catalog task row {index} fields 'blocker_owner' and 'blocker_notes' "
            f"must either both be present or both be absent{catalog_path_text}"
        )


def count_actionable_rows(
    payload: Any,
    *,
    actionable_statuses: set[str],
    catalog_path: Path | None = None,
) -> int:
    catalog_path_text = f" in {display_path(catalog_path)}" if catalog_path is not None else ""
    rows: Any
    declared_task_count: int | None = None
    if isinstance(payload, dict):
        validate_object_key_order(
            payload,
            expected_key_order=CATALOG_SNAPSHOT_OBJECT_KEY_ORDER,
            context=f"catalog snapshot object{catalog_path_text}",
        )
        if "tasks" not in payload:
            raise ValueError(
                "catalog JSON root object must include a 'tasks' array"
                f"{catalog_path_text}"
            )
        has_generated_at_utc = "generated_at_utc" in payload
        has_source = "source" in payload
        if has_generated_at_utc != has_source:
            raise ValueError(
                "catalog snapshot object must include both 'generated_at_utc' and "
                f"'source' when either field is present{catalog_path_text}"
            )
        if has_generated_at_utc:
            try:
                parse_generated_at_utc(
                    payload["generated_at_utc"],
                    label="catalog",
                )
            except ValueError as exc:
                raise ValueError(f"{exc}{catalog_path_text}") from exc
            try:
                parse_canonical_non_empty_string(
                    payload["source"],
                    context="catalog snapshot field 'source'",
                )
            except ValueError as exc:
                raise ValueError(f"{exc}{catalog_path_text}") from exc
        if "generated_on" in payload:
            try:
                parse_canonical_non_empty_string(
                    payload["generated_on"],
                    context="catalog snapshot field 'generated_on'",
                )
            except ValueError as exc:
                raise ValueError(f"{exc}{catalog_path_text}") from exc
        if "task_count" in payload:
            declared_task_count = parse_non_negative_count(
                payload["task_count"],
                context="catalog snapshot field 'task_count'",
            )
        if "count" in payload:
            legacy_count = parse_non_negative_count(
                payload["count"],
                context="catalog snapshot field 'count'",
            )
            if declared_task_count is None:
                declared_task_count = legacy_count
            elif declared_task_count != legacy_count:
                raise ValueError(
                    "catalog snapshot fields 'task_count' and 'count' must match"
                    f"{catalog_path_text}"
                )
        rows = payload["tasks"]
    elif isinstance(payload, list):
        rows = payload
    else:
        raise ValueError(
            "catalog JSON root must be either an object or an array"
            f"{catalog_path_text}"
        )

    if not isinstance(rows, list):
        raise ValueError(f"catalog tasks must be an array{catalog_path_text}")

    actionable_count = 0
    observed_task_ids: list[str] = []
    seen_task_ids: set[str] = set()
    for index, row in enumerate(rows):
        if not isinstance(row, dict):
            raise ValueError(
                f"catalog task at index {index} must be an object{catalog_path_text}"
            )
        validate_object_key_order(
            row,
            expected_key_order=CATALOG_TASK_ROW_KEY_ORDER,
            context=f"catalog task row {index}{catalog_path_text}",
        )
        task_id = parse_canonical_non_empty_string(
            row.get("task_id"),
            context=f"catalog task row {index} field 'task_id'",
        )
        if task_id in seen_task_ids:
            raise ValueError(
                "catalog tasks contain duplicate task_id "
                f"{task_id!r}{catalog_path_text}"
            )
        seen_task_ids.add(task_id)
        observed_task_ids.append(task_id)

        has_execution_status = "execution_status" in row
        has_legacy_status = "status" in row
        if not has_execution_status and not has_legacy_status:
            raise ValueError(
                f"catalog task row {index} must include non-empty string "
                f"'execution_status' or legacy 'status'{catalog_path_text}"
            )

        execution_status: str | None = None
        legacy_status: str | None = None
        if has_execution_status:
            execution_status = parse_canonical_non_empty_string(
                row.get("execution_status"),
                context=f"catalog task row {index} field 'execution_status'",
            )
        if has_legacy_status:
            legacy_status = parse_canonical_non_empty_string(
                row.get("status"),
                context=f"catalog task row {index} field 'status'",
            )
        if execution_status is not None and legacy_status is not None:
            if normalize_status(execution_status) != normalize_status(legacy_status):
                raise ValueError(
                    "catalog task status alias mismatch"
                    f"{catalog_path_text} for row index {index}: "
                    f"execution_status={execution_status!r} status={legacy_status!r}"
                )

        validate_catalog_contract_fields(
            row,
            index=index,
            catalog_path_text=catalog_path_text,
        )

        status = normalize_status(
            execution_status if execution_status is not None else legacy_status
        )
        if status in actionable_statuses:
            actionable_count += 1

    expected_task_id_order = sorted(
        observed_task_ids,
        key=lambda value: (value.casefold(), value),
    )
    if observed_task_ids != expected_task_id_order:
        raise ValueError(
            "catalog tasks must be sorted by 'task_id'"
            f"{catalog_path_text}"
        )

    if declared_task_count is not None and declared_task_count != len(rows):
        raise ValueError(
            "catalog snapshot count mismatch"
            f"{catalog_path_text}: declared={declared_task_count} "
            f"discovered={len(rows)}"
        )

    return actionable_count


def build_trigger_results(
    *,
    open_issue_count: int,
    open_milestone_count: int,
    actionable_row_count: int,
    open_blocker_count: int,
) -> list[TriggerResult]:
    counts = {
        "T1-ISSUES": open_issue_count,
        "T2-MILESTONES": open_milestone_count,
        "T3-ACTIONABLE-ROWS": actionable_row_count,
        OPEN_BLOCKERS_TRIGGER_ID: open_blocker_count,
    }
    return [
        TriggerResult(
            trigger_id=trigger_id,
            condition=TRIGGER_CONDITIONS[trigger_id],
            count=counts[trigger_id],
            fired=counts[trigger_id] > 0,
        )
        for trigger_id in TRIGGER_ORDER
    ]


def validate_trigger_result_order(trigger_results: Sequence[TriggerResult]) -> None:
    trigger_ids = tuple(entry.trigger_id for entry in trigger_results)
    if trigger_ids != TRIGGER_ORDER:
        raise ValueError(
            "internal trigger order drift: "
            f"expected={list(TRIGGER_ORDER)!r} observed={list(trigger_ids)!r}"
        )


def parse_t4_overlay_state(
    *,
    overlay_path: Path | None,
    flag_enabled: bool,
) -> T4OverlayState:
    if flag_enabled:
        return T4OverlayState(
            new_scope_publish=True,
            source="cli-flag",
            overlay_path=None,
        )

    if overlay_path is None:
        return T4OverlayState(
            new_scope_publish=False,
            source="default-false",
            overlay_path=None,
        )

    payload = load_json(overlay_path, label="T4 governance overlay")
    if isinstance(payload, bool):
        return T4OverlayState(
            new_scope_publish=payload,
            source="overlay-json",
            overlay_path=overlay_path,
        )

    if not isinstance(payload, dict):
        raise ValueError(
            "T4 governance overlay JSON must be either a boolean or an object "
            "containing a boolean 't4_new_scope_publish' field"
        )

    validate_object_key_order(
        payload,
        expected_key_order=T4_OVERLAY_OBJECT_KEY_ORDER,
        context="T4 governance overlay JSON object",
    )

    has_lower = "t4_new_scope_publish" in payload
    has_upper = "T4_NEW_SCOPE_PUBLISH" in payload
    if not has_lower and not has_upper:
        raise ValueError(
            "T4 governance overlay JSON object must include "
            "'t4_new_scope_publish' (or 'T4_NEW_SCOPE_PUBLISH')"
        )

    raw_value = payload["t4_new_scope_publish"] if has_lower else payload["T4_NEW_SCOPE_PUBLISH"]
    if not isinstance(raw_value, bool):
        raise ValueError("T4 governance overlay field 't4_new_scope_publish' must be a boolean")

    if has_lower and has_upper:
        legacy_value = payload["T4_NEW_SCOPE_PUBLISH"]
        if not isinstance(legacy_value, bool):
            raise ValueError("T4 governance overlay field 'T4_NEW_SCOPE_PUBLISH' must be a boolean")
        if raw_value != legacy_value:
            raise ValueError(
                "T4 governance overlay fields 't4_new_scope_publish' and "
                "'T4_NEW_SCOPE_PUBLISH' must match when both are provided"
            )

    return T4OverlayState(
        new_scope_publish=raw_value,
        source="overlay-json",
        overlay_path=overlay_path,
    )


def build_payload(
    *,
    issues_path: Path,
    milestones_path: Path,
    catalog_path: Path,
    open_blockers_state: OpenBlockersState,
    actionable_statuses: Sequence[str],
    trigger_results: Sequence[TriggerResult],
    t4_overlay_state: T4OverlayState,
    issues_freshness: SnapshotFreshnessState,
    milestones_freshness: SnapshotFreshnessState,
) -> dict[str, object]:
    active_trigger_ids = [entry.trigger_id for entry in trigger_results if entry.fired]
    activation_required = bool(active_trigger_ids)
    gate_open = activation_required or t4_overlay_state.new_scope_publish
    exit_code = 1 if gate_open else 0

    return {
        "mode": "offline-deterministic",
        "contract_id": ACTIVATION_SEED_CONTRACT_ID,
        "fail_closed": True,
        "inputs": {
            "issues_json": display_path(issues_path),
            "milestones_json": display_path(milestones_path),
            "catalog_json": display_path(catalog_path),
            "open_blockers_json": (
                display_path(open_blockers_state.snapshot_path)
                if open_blockers_state.snapshot_path is not None
                else None
            ),
            "t4_governance_overlay_json": (
                display_path(t4_overlay_state.overlay_path)
                if t4_overlay_state.overlay_path is not None
                else None
            ),
        },
        "actionable_statuses": list(actionable_statuses),
        "trigger_order": list(TRIGGER_ORDER),
        "freshness": {
            "issues": {
                "requested": issues_freshness.requested,
                "max_age_seconds": issues_freshness.max_age_seconds,
                "generated_at_utc": issues_freshness.generated_at_utc,
                "age_seconds": issues_freshness.age_seconds,
                "fresh": issues_freshness.fresh,
            },
            "milestones": {
                "requested": milestones_freshness.requested,
                "max_age_seconds": milestones_freshness.max_age_seconds,
                "generated_at_utc": milestones_freshness.generated_at_utc,
                "age_seconds": milestones_freshness.age_seconds,
                "fresh": milestones_freshness.fresh,
            },
        },
        "triggers": [
            {
                "id": entry.trigger_id,
                "condition": entry.condition,
                "count": entry.count,
                "fired": entry.fired,
            }
            for entry in trigger_results
        ],
        "active_trigger_ids": active_trigger_ids,
        "activation_required": activation_required,
        "open_blockers": {
            "count": open_blockers_state.count,
            "trigger_id": OPEN_BLOCKERS_TRIGGER_ID,
            "trigger_fired": open_blockers_state.count > 0,
        },
        "t4_governance_overlay": {
            "new_scope_publish": t4_overlay_state.new_scope_publish,
            "source": t4_overlay_state.source,
        },
        "gate_open": gate_open,
        "queue_state": "dispatch-open" if gate_open else "idle",
        "exit_code": exit_code,
    }


def render_json(payload: dict[str, object]) -> str:
    return json.dumps(payload, indent=2) + "\n"


def bool_text(value: bool) -> str:
    return "true" if value else "false"


def markdown_freshness_cell(raw_value: object) -> str:
    if raw_value is None:
        return "_none_"
    if isinstance(raw_value, bool):
        return f"`{bool_text(raw_value)}`"
    return f"`{raw_value}`"


def render_markdown(payload: dict[str, object]) -> str:
    inputs = payload["inputs"]
    assert isinstance(inputs, dict)
    trigger_entries = payload["triggers"]
    assert isinstance(trigger_entries, list)
    active_trigger_ids = payload["active_trigger_ids"]
    assert isinstance(active_trigger_ids, list)
    actionable_statuses = payload["actionable_statuses"]
    assert isinstance(actionable_statuses, list)
    trigger_order = payload["trigger_order"]
    assert isinstance(trigger_order, list)
    freshness_payload = payload["freshness"]
    assert isinstance(freshness_payload, dict)
    open_blockers = payload["open_blockers"]
    assert isinstance(open_blockers, dict)
    issues_freshness = freshness_payload["issues"]
    milestones_freshness = freshness_payload["milestones"]
    assert isinstance(issues_freshness, dict)
    assert isinstance(milestones_freshness, dict)
    open_blockers_input = inputs["open_blockers_json"]
    open_blockers_input_text = (
        f"`{open_blockers_input}`" if open_blockers_input is not None else "_none_"
    )
    t4_overlay = payload["t4_governance_overlay"]
    assert isinstance(t4_overlay, dict)
    t4_overlay_input = inputs["t4_governance_overlay_json"]
    t4_overlay_input_text = (
        f"`{t4_overlay_input}`" if t4_overlay_input is not None else "_none_"
    )

    lines = [
        "# Activation Trigger Check",
        "",
        f"- Mode: `{payload['mode']}`",
        f"- Contract ID: `{payload['contract_id']}`",
        f"- Fail closed: `{bool_text(bool(payload['fail_closed']))}`",
        f"- Issues snapshot: `{inputs['issues_json']}`",
        f"- Milestones snapshot: `{inputs['milestones_json']}`",
        f"- Catalog JSON: `{inputs['catalog_json']}`",
        f"- Open blockers JSON: {open_blockers_input_text}",
        f"- T4 governance overlay JSON: {t4_overlay_input_text}",
        (
            "- Actionable statuses: "
            + ", ".join(f"`{status}`" for status in actionable_statuses)
        ),
        "- Trigger order: " + ", ".join(f"`{trigger_id}`" for trigger_id in trigger_order),
        f"- Open blockers count: `{open_blockers['count']}`",
        (
            "- Open blockers trigger fired: "
            f"`{bool_text(bool(open_blockers['trigger_fired']))}`"
        ),
        f"- Activation required: `{bool_text(bool(payload['activation_required']))}`",
        f"- T4 new scope publish: `{bool_text(bool(t4_overlay['new_scope_publish']))}`",
        f"- T4 source: `{t4_overlay['source']}`",
        f"- Gate open: `{bool_text(bool(payload['gate_open']))}`",
        f"- Queue state: `{payload['queue_state']}`",
        f"- Exit code: `{payload['exit_code']}`",
        "",
        "## Snapshot Freshness",
        "",
        "| Snapshot | Requested | Max age (s) | Generated at UTC | Age (s) | Fresh |",
        "| --- | --- | --- | --- | --- | --- |",
        (
            f"| Issues | {markdown_freshness_cell(issues_freshness.get('requested'))} | "
            f"{markdown_freshness_cell(issues_freshness.get('max_age_seconds'))} | "
            f"{markdown_freshness_cell(issues_freshness.get('generated_at_utc'))} | "
            f"{markdown_freshness_cell(issues_freshness.get('age_seconds'))} | "
            f"{markdown_freshness_cell(issues_freshness.get('fresh'))} |"
        ),
        (
            f"| Milestones | {markdown_freshness_cell(milestones_freshness.get('requested'))} | "
            f"{markdown_freshness_cell(milestones_freshness.get('max_age_seconds'))} | "
            f"{markdown_freshness_cell(milestones_freshness.get('generated_at_utc'))} | "
            f"{markdown_freshness_cell(milestones_freshness.get('age_seconds'))} | "
            f"{markdown_freshness_cell(milestones_freshness.get('fresh'))} |"
        ),
        "",
        "## Trigger Results",
        "",
        "| Trigger ID | Fired | Count | Condition |",
        "| --- | --- | --- | --- |",
    ]

    for entry in trigger_entries:
        assert isinstance(entry, dict)
        lines.append(
            "| "
            f"`{entry['id']}` | "
            f"`{bool_text(bool(entry['fired']))}` | "
            f"{entry['count']} | "
            f"{entry['condition']} |"
        )

    lines.append("")
    if active_trigger_ids:
        lines.append(
            "- Active triggers: "
            + ", ".join(f"`{trigger_id}`" for trigger_id in active_trigger_ids)
        )
    else:
        lines.append("- Active triggers: _none_")

    return "\n".join(lines).rstrip() + "\n"


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_activation_triggers.py",
        description=(
            "Evaluate deterministic activation triggers from local JSON snapshots "
            "(offline mode)."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=textwrap.dedent(
            f"""\
            Exit semantics:
              0: Checker ran successfully and final gate_open is false.
              1: Checker ran successfully and final gate_open is true.
              2: Input/validation error (missing file, invalid JSON, malformed payload).

            Examples:
              python scripts/check_activation_triggers.py --issues-json tmp_open_issues_snapshot.json --milestones-json tmp_open_milestones_snapshot.json --catalog-json {display_path(DEFAULT_CATALOG_JSON)} --format json
              python scripts/check_activation_triggers.py --issues-json tmp_open_issues_snapshot.json --milestones-json tmp_open_milestones_snapshot.json --catalog-json {display_path(DEFAULT_CATALOG_JSON)} --issues-max-age-seconds 1800 --milestones-max-age-seconds 1800 --format json
              python scripts/check_activation_triggers.py --issues-json tmp_open_issues_snapshot.json --milestones-json tmp_open_milestones_snapshot.json --catalog-json {display_path(DEFAULT_CATALOG_JSON)} --open-blockers-json tmp_open_blockers.json --format json
              python scripts/check_activation_triggers.py --issues-json tmp_open_issues_snapshot.json --milestones-json tmp_open_milestones_snapshot.json --catalog-json {display_path(DEFAULT_CATALOG_JSON)} --format markdown
              python scripts/check_activation_triggers.py --issues-json tmp_open_issues_snapshot.json --milestones-json tmp_open_milestones_snapshot.json --catalog-json {display_path(DEFAULT_CATALOG_JSON)} --t4-governance-overlay-json tmp_t4_overlay.json --format json
              python scripts/check_activation_triggers.py --issues-json tmp_open_issues_snapshot.json --milestones-json tmp_open_milestones_snapshot.json --catalog-json {display_path(DEFAULT_CATALOG_JSON)} --t4-new-scope-publish --format json
            """
        ),
    )
    parser.add_argument(
        "--issues-json",
        type=Path,
        required=True,
        help="Path to JSON snapshot containing currently open issues.",
    )
    parser.add_argument(
        "--milestones-json",
        type=Path,
        required=True,
        help="Path to JSON snapshot containing currently open milestones.",
    )
    parser.add_argument(
        "--catalog-json",
        type=Path,
        default=DEFAULT_CATALOG_JSON,
        help=(
            "Path to remaining-task catalog JSON. "
            f"Default: {display_path(DEFAULT_CATALOG_JSON)}."
        ),
    )
    parser.add_argument(
        "--open-blockers-json",
        type=Path,
        help=(
            "Optional path to open blockers JSON. "
            "Strict schema: object with 'open_blockers' array and non-negative "
            "'open_blocker_count' (or legacy 'count') matching array length."
        ),
    )
    parser.add_argument(
        "--issues-max-age-seconds",
        type=parse_non_negative_int_argument,
        help=(
            "Optional freshness gate for issues snapshot based on root "
            "object field 'generated_at_utc'."
        ),
    )
    parser.add_argument(
        "--milestones-max-age-seconds",
        type=parse_non_negative_int_argument,
        help=(
            "Optional freshness gate for milestones snapshot based on root "
            "object field 'generated_at_utc'."
        ),
    )
    parser.add_argument(
        "--actionable-status",
        action="append",
        dest="actionable_statuses",
        help=(
            "Repeatable actionable status value. "
            "Defaults to: open, open-blocked, blocked."
        ),
    )
    t4_group = parser.add_mutually_exclusive_group()
    t4_group.add_argument(
        "--t4-governance-overlay-json",
        type=Path,
        help=(
            "Optional path to local T4 governance overlay JSON. "
            "Allowed shapes: boolean root, or object with boolean "
            "'t4_new_scope_publish' (or 'T4_NEW_SCOPE_PUBLISH')."
        ),
    )
    t4_group.add_argument(
        "--t4-new-scope-publish",
        action="store_true",
        help=(
            "Optional deterministic override that sets "
            "T4_NEW_SCOPE_PUBLISH=true without an overlay file."
        ),
    )
    parser.add_argument(
        "--format",
        choices=("json", "markdown"),
        default="json",
        help="Output format: json (default) or markdown.",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        actionable_statuses = normalize_actionable_statuses(args.actionable_statuses)
        issues_path = resolve_input_path(args.issues_json)
        milestones_path = resolve_input_path(args.milestones_json)
        catalog_path = resolve_input_path(args.catalog_json)
        open_blockers_path = (
            resolve_input_path(args.open_blockers_json)
            if args.open_blockers_json is not None
            else None
        )
        t4_overlay_path = (
            resolve_input_path(args.t4_governance_overlay_json)
            if args.t4_governance_overlay_json is not None
            else None
        )

        now_utc = utc_now()
        issues_payload = load_json(issues_path, label="open issues snapshot")
        milestones_payload = load_json(milestones_path, label="open milestones snapshot")
        catalog_payload = load_json(catalog_path, label="remaining-task catalog")

        open_issue_count = count_open_snapshot_entries(
            issues_payload,
            label="open issues",
            snapshot_path=issues_path,
        )
        open_milestone_count = count_open_snapshot_entries(
            milestones_payload,
            label="open milestones",
            snapshot_path=milestones_path,
        )
        actionable_row_count = count_actionable_rows(
            catalog_payload,
            actionable_statuses=set(actionable_statuses),
            catalog_path=catalog_path,
        )
        open_blockers_state = OpenBlockersState(
            count=0,
            snapshot_path=None,
        )
        if open_blockers_path is not None:
            open_blockers_payload = load_json(
                open_blockers_path,
                label="open blockers snapshot",
            )
            open_blockers_state = OpenBlockersState(
                count=parse_open_blockers_count(
                    open_blockers_payload,
                    snapshot_path=open_blockers_path,
                ),
                snapshot_path=open_blockers_path,
            )
        issues_freshness = evaluate_snapshot_freshness(
            payload=issues_payload,
            label="open issues",
            snapshot_path=issues_path,
            max_age_seconds=args.issues_max_age_seconds,
            flag_name="--issues-max-age-seconds",
            now_utc=now_utc,
        )
        milestones_freshness = evaluate_snapshot_freshness(
            payload=milestones_payload,
            label="open milestones",
            snapshot_path=milestones_path,
            max_age_seconds=args.milestones_max_age_seconds,
            flag_name="--milestones-max-age-seconds",
            now_utc=now_utc,
        )
        t4_overlay_state = parse_t4_overlay_state(
            overlay_path=t4_overlay_path,
            flag_enabled=args.t4_new_scope_publish,
        )
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    try:
        trigger_results = build_trigger_results(
            open_issue_count=open_issue_count,
            open_milestone_count=open_milestone_count,
            actionable_row_count=actionable_row_count,
            open_blocker_count=open_blockers_state.count,
        )
        validate_trigger_result_order(trigger_results)
        payload = build_payload(
            issues_path=issues_path,
            milestones_path=milestones_path,
            catalog_path=catalog_path,
            open_blockers_state=open_blockers_state,
            actionable_statuses=actionable_statuses,
            trigger_results=trigger_results,
            t4_overlay_state=t4_overlay_state,
            issues_freshness=issues_freshness,
            milestones_freshness=milestones_freshness,
        )
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    if args.format == "markdown":
        write_stdout(render_markdown(payload))
    else:
        write_stdout(render_json(payload))

    return int(payload["exit_code"])


if __name__ == "__main__":
    raise SystemExit(main())
