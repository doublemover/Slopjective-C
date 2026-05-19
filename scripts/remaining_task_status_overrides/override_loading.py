from __future__ import annotations

from pathlib import Path
from typing import Any, Sequence

from .constants import ALLOWED_STATUSES
from .json_files import read_json
from .models import OverrideEntry
from .text import normalize_text


def parse_override_entry(raw: Any, source_path: Path, index: int) -> OverrideEntry:
    if not isinstance(raw, dict):
        raise ValueError(
            f"{source_path}: override entry at index {index} must be an object"
        )

    task_id_raw = raw.get("task_id")
    if not isinstance(task_id_raw, str) or not normalize_text(task_id_raw):
        raise ValueError(f"{source_path}: override entry {index} has invalid 'task_id'")
    task_id = normalize_text(task_id_raw)

    status_raw = raw.get("recommended_status")
    if not isinstance(status_raw, str):
        raise ValueError(
            f"{source_path}: override entry {index} has invalid 'recommended_status'"
        )
    recommended_status = normalize_text(status_raw)
    if recommended_status not in ALLOWED_STATUSES:
        allowed = ", ".join(sorted(ALLOWED_STATUSES))
        raise ValueError(
            f"{source_path}: override entry {index} status '{recommended_status}' "
            f"is not allowed (allowed: {allowed})"
        )

    evidence_refs_raw = raw.get("evidence_refs", [])
    evidence_refs: list[str] = []
    if evidence_refs_raw is not None:
        if not isinstance(evidence_refs_raw, list):
            raise ValueError(
                f"{source_path}: override entry {index} has invalid 'evidence_refs'"
            )
        for item in evidence_refs_raw:
            if not isinstance(item, str):
                raise ValueError(
                    f"{source_path}: override entry {index} has non-string evidence ref"
                )
            cleaned = normalize_text(item)
            if cleaned:
                evidence_refs.append(cleaned)

    rationale_raw = raw.get("rationale", "")
    if not isinstance(rationale_raw, str):
        raise ValueError(f"{source_path}: override entry {index} has invalid 'rationale'")
    rationale = normalize_text(rationale_raw)

    return OverrideEntry(
        task_id=task_id,
        recommended_status=recommended_status,
        evidence_refs=tuple(evidence_refs),
        rationale=rationale,
        source_path=source_path,
    )


def load_overrides(paths: Sequence[Path]) -> dict[str, OverrideEntry]:
    mapping: dict[str, OverrideEntry] = {}
    for path in paths:
        payload = read_json(path)
        raw_entries: list[Any]
        if isinstance(payload, list):
            raw_entries = payload
        elif isinstance(payload, dict):
            overrides_raw = payload.get("overrides")
            if not isinstance(overrides_raw, list):
                raise ValueError(
                    f"{path}: override JSON object must contain an 'overrides' array"
                )
            raw_entries = overrides_raw
        else:
            raise ValueError(f"{path}: override JSON root must be an array or object")

        for index, raw in enumerate(raw_entries):
            entry = parse_override_entry(raw, path, index)
            previous = mapping.get(entry.task_id)
            if previous is not None:
                raise ValueError(
                    f"duplicate override for task_id '{entry.task_id}' in "
                    f"{previous.source_path} and {entry.source_path}"
                )
            mapping[entry.task_id] = entry
    return mapping
