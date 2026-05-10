from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Issue:
    number: int
    title: str
    labels: tuple[str, ...]
