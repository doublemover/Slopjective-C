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
class FrontendContractArtifact:
    name: str
    family: str
    artifact_path: str

    def as_payload(self) -> dict[str, str]:
        return {
            "name": self.name,
            "family": self.family,
            "artifact_path": self.artifact_path,
        }


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
    frontend_contract_artifacts: tuple[FrontendContractArtifact, ...]
    explicit_non_goals: tuple[str, ...]

    def expected_payload(self) -> dict[str, Any]:
        payload = {field.name: field.expected for field in self.fields}
        payload["frontend_contract_artifacts"] = [
            artifact.as_payload() for artifact in self.frontend_contract_artifacts
        ]
        payload["explicit_non_goals"] = list(self.explicit_non_goals)
        return payload

    def validate(self, payload: Mapping[str, Any]) -> SurfaceReport:
        errors: list[str] = []
        for field in self.fields:
            if payload.get(field.name) != field.expected:
                errors.append(field.drift_message)

        frontend_contract_artifacts = payload.get("frontend_contract_artifacts")
        expected_artifacts = [
            artifact.as_payload() for artifact in self.frontend_contract_artifacts
        ]
        if not (isinstance(frontend_contract_artifacts, list) and frontend_contract_artifacts):
            errors.append("frontend_contract_artifacts missing")
        elif frontend_contract_artifacts != expected_artifacts:
            errors.append("frontend contract artifact inventory drifted")

        if payload.get("explicit_non_goals") != list(self.explicit_non_goals):
            errors.append("explicit_non_goals drifted")

        return SurfaceReport(CHECKER_NAME, tuple(errors))


__all__ = [
    "CHECKER_NAME",
    "FrontendContractArtifact",
    "RepoSupercleanSurfaceModel",
    "SurfaceField",
    "SurfaceReport",
    "SurfaceReportWriter",
]
