"""Contract models for public claim drift summaries."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any, Mapping, Sequence

JsonObject = dict[str, Any]


@dataclass(frozen=True)
class PublicClaimDriftInputs:
    support_summary: Mapping[str, Any]
    support_summary_path: str
    support_summary_sha256: str
    surface_sha256: Mapping[str, str]
    surface_lines: Mapping[str, Sequence[str]]


__all__ = ["JsonObject", "PublicClaimDriftInputs"]
