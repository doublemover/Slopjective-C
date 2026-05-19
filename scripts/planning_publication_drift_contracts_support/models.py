from __future__ import annotations

from dataclasses import dataclass
from typing import Any, Mapping


JsonObject = dict[str, Any]


@dataclass(frozen=True)
class PlanningPublicationDriftPaths:
    payload_path: str
    publication_report_path: str
    markdown_report_path: str


@dataclass(frozen=True)
class PlanningPublicationDriftInputs:
    paths: PlanningPublicationDriftPaths
    payload: Mapping[str, Any]
    report: Mapping[str, Any]
    failures: list[JsonObject]
    live_summary: Mapping[str, Any]
