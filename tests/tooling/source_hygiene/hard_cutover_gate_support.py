from __future__ import annotations

from pathlib import Path
from typing import Any


Report = dict[str, Any]


def write_fixture(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def active_pattern_ids(report: Report) -> list[str]:
    return [finding["pattern_id"] for finding in report["active_findings"]]


def active_path_patterns(report: Report) -> dict[str, str]:
    return {
        finding["path"]: finding["pattern_id"]
        for finding in report["active_findings"]
    }
