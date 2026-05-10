from __future__ import annotations

import json
from pathlib import Path

from .digests import compute_source_line_hash, normalize_source_line_hash
from .models import CatalogTask, SourceLineResult
from .paths import ROOT


def load_catalog(path: Path) -> list[CatalogTask]:
    if not path.exists():
        raise RuntimeError(f"catalog file not found: {path}")

    payload = json.loads(path.read_text(encoding="utf-8"))
    raw_tasks = payload.get("tasks")
    if not isinstance(raw_tasks, list):
        raise RuntimeError("catalog JSON missing 'tasks' array")

    tasks: list[CatalogTask] = []
    for raw in raw_tasks:
        if not isinstance(raw, dict):
            continue
        task_id = raw.get("task_id")
        raw_path = raw.get("path")
        raw_line = raw.get("line")
        title = raw.get("title")
        task_key = raw.get("task_key")
        source_line_hash = raw.get("source_line_hash")
        if not isinstance(task_id, str):
            continue
        if not isinstance(raw_path, str):
            continue
        if not isinstance(raw_line, int):
            continue
        if not isinstance(title, str):
            title = task_id
        if task_key is not None:
            if not isinstance(task_key, str) or not task_key:
                raise RuntimeError(
                    f"catalog task '{task_id}' has invalid task_key; expected non-empty string"
                )
        if source_line_hash is not None:
            if not isinstance(source_line_hash, str) or not source_line_hash:
                raise RuntimeError(
                    f"catalog task '{task_id}' has invalid source_line_hash; expected non-empty string"
                )
        tasks.append(
            CatalogTask(
                task_id=task_id,
                path=ROOT / Path(raw_path),
                line=raw_line,
                title=title,
                task_key=task_key,
                source_line_hash=source_line_hash,
            )
        )

    return tasks


def source_line_result(path: Path, line: int, expected_hash: str | None) -> SourceLineResult:
    if not path.exists():
        return SourceLineResult(
            raw_line=None,
            checked=False,
            display_line="<missing file>",
            stale_reason="missing-file",
        )

    lines = path.read_text(encoding="utf-8").splitlines()
    if line <= 0 or line > len(lines):
        return SourceLineResult(
            raw_line=None,
            checked=False,
            display_line="<line out of range>",
            stale_reason="line-out-of-range",
        )

    raw_line = lines[line - 1]
    display_line_text = raw_line.strip()
    checked = display_line_text.startswith("- [x]") or display_line_text.startswith("- [X]")
    if expected_hash is None:
        return SourceLineResult(
            raw_line=raw_line,
            checked=checked,
            display_line=display_line_text,
            stale_reason=None,
        )

    actual_hash = normalize_source_line_hash(compute_source_line_hash(raw_line))
    expected_hash_norm = normalize_source_line_hash(expected_hash)
    if actual_hash != expected_hash_norm:
        return SourceLineResult(
            raw_line=raw_line,
            checked=checked,
            display_line=display_line_text,
            stale_reason=f"hash-mismatch expected={expected_hash} actual=sha256:{actual_hash}",
        )

    return SourceLineResult(
        raw_line=raw_line,
        checked=checked,
        display_line=display_line_text,
        stale_reason=None,
    )


def checkbox_state(path: Path, line: int) -> tuple[bool, str]:
    result = source_line_result(path, line, expected_hash=None)
    return result.checked, result.display_line
