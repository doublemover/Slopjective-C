"""Data model and writer for repo superclean surface reports."""

from __future__ import annotations

import sys
from dataclasses import dataclass
from typing import Any, Mapping

CHECKER_NAME = "repo-superclean-surface"


@dataclass(frozen=True)
class SurfaceField:
    name: str
    expected: Any
    drift_message: str


@dataclass(frozen=True)
class SurfaceReport:
    checker_name: str
    errors: tuple[str, ...]

    @property
    def passed(self) -> bool:
        return not self.errors


class SurfaceReportWriter:
    def write(self, report: SurfaceReport) -> int:
        if report.passed:
            print(f"{report.checker_name}: OK")
            return 0

        print(f"{report.checker_name}: FAIL", file=sys.stderr)
        for error in report.errors:
            print(f"- {error}", file=sys.stderr)
        return 1


@dataclass(frozen=True)
class RepoSupercleanSurfaceModel:
    fields: tuple[SurfaceField, ...]
    frontend_contract_artifact_names: tuple[str, ...]
    explicit_non_goals: tuple[str, ...]

    def validate(self, payload: Mapping[str, Any]) -> SurfaceReport:
        errors: list[str] = []
        for field in self.fields:
            if payload.get(field.name) != field.expected:
                errors.append(field.drift_message)

        frontend_contract_artifacts = payload.get("frontend_contract_artifacts", [])
        if not (isinstance(frontend_contract_artifacts, list) and frontend_contract_artifacts):
            errors.append("frontend_contract_artifacts missing")
        artifact_names = [
            entry.get("name")
            for entry in frontend_contract_artifacts
            if isinstance(entry, dict)
        ]
        if artifact_names != list(self.frontend_contract_artifact_names):
            errors.append("frontend contract artifact inventory drifted")

        if payload.get("explicit_non_goals") != list(self.explicit_non_goals):
            errors.append("explicit_non_goals drifted")

        return SurfaceReport(CHECKER_NAME, tuple(errors))


__all__ = [
    "CHECKER_NAME",
    "RepoSupercleanSurfaceModel",
    "SurfaceField",
    "SurfaceReport",
    "SurfaceReportWriter",
]
