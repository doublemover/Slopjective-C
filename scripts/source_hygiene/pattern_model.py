from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class ForbiddenPattern:
    pattern_id: str
    description: str
    regex: str
    severity: str = "error"
    include_paths: tuple[str, ...] = ()
    exclude_paths: tuple[str, ...] = ()
