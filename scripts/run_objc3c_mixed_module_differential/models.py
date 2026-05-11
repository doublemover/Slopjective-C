"""Typed result and manifest models for mixed-module differential runs."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any, Callable

from .runtime_acceptance import CaseResult

NoClangCase = Callable[[Path], CaseResult]
ClangCase = Callable[[str, Path], CaseResult]
CaseRunner = Callable[[Path, str | None], CaseResult]
SurfaceBuilder = Callable[[list[CaseResult]], dict[str, Any]]


@dataclass(frozen=True)
class FixtureGroup:
    group_id: str
    provider: str
    consumer: str

    @classmethod
    def from_manifest(cls, root: Path, payload: Any) -> "FixtureGroup":
        if not isinstance(payload, dict):
            raise RuntimeError("mixed-module differential manifest fixture_groups entry was not an object")
        fields: dict[str, str] = {}
        for key in ("group_id", "provider", "consumer"):
            value = payload.get(key)
            if not isinstance(value, str) or not value:
                raise RuntimeError(f"mixed-module differential manifest fixture group missing {key}")
            if key != "group_id" and not (root / value).is_file():
                raise RuntimeError(f"mixed-module differential manifest references missing fixture {value}")
            fields[key] = value
        return cls(**fields)

    def to_json(self) -> dict[str, str]:
        return {
            "group_id": self.group_id,
            "provider": self.provider,
            "consumer": self.consumer,
        }


@dataclass(frozen=True)
class CaseSummary:
    case_id: str
    probe: str
    fixture: str | None
    claim_class: str
    summary: dict[str, Any]

    @classmethod
    def from_result(cls, result: CaseResult) -> "CaseSummary":
        return cls(
            case_id=result.case_id,
            probe=result.probe,
            fixture=result.fixture,
            claim_class=result.claim_class,
            summary=result.summary,
        )

    def to_json(self) -> dict[str, Any]:
        return {
            "case_id": self.case_id,
            "probe": self.probe,
            "fixture": self.fixture,
            "claim_class": self.claim_class,
            "summary": self.summary,
        }


__all__ = [
    "CaseResult",
    "CaseRunner",
    "CaseSummary",
    "ClangCase",
    "FixtureGroup",
    "NoClangCase",
    "SurfaceBuilder",
]
