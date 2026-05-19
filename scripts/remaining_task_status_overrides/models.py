from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class OverrideEntry:
    task_id: str
    recommended_status: str
    evidence_refs: tuple[str, ...]
    rationale: str
    source_path: Path
