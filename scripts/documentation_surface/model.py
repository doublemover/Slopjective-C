from __future__ import annotations

import sys
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class DocumentationSurfaceSource:
    path: Path
    root: Path
    required_tokens: tuple[str, ...] = ()
    forbidden_tokens: tuple[str, ...] = ()
    owner_id: str = ""
    owner_surface: str = ""

    @property
    def report_path(self) -> str:
        return self.path.relative_to(self.root).as_posix()

    def validate(self) -> tuple[str, ...]:
        text = self.path.read_text(encoding="utf-8")
        errors: list[str] = []
        for token in self.required_tokens:
            if token not in text:
                errors.append(f"{self.report_path}: missing required token {token!r}")
        for token in self.forbidden_tokens:
            if token in text:
                errors.append(f"{self.report_path}: forbidden token present {token!r}")
        return tuple(errors)


@dataclass(frozen=True)
class DocumentationSurfaceReport:
    checker_name: str
    errors: tuple[str, ...]
    owner_contract: dict[str, object]

    @property
    def passed(self) -> bool:
        return not self.errors


class DocumentationSurfaceReportWriter:
    def write(self, report: DocumentationSurfaceReport) -> int:
        if report.passed:
            print(f"{report.checker_name}: OK")
            return 0

        print(f"{report.checker_name}: FAIL", file=sys.stderr)
        for error in report.errors:
            print(f"- {error}", file=sys.stderr)
        return 1


@dataclass(frozen=True)
class DocumentationSurfaceModel:
    checker_name: str
    sources: tuple[DocumentationSurfaceSource, ...]
    owner_id: str
    owner_surface: str
    blocker_metadata: dict[str, object]

    def owner_contract(self) -> dict[str, object]:
        return {
            "owner_id": self.owner_id,
            "owner_surface": self.owner_surface,
            "checked_source_count": len(self.sources),
            "checked_sources": [source.report_path for source in self.sources],
            "blocker_metadata": dict(self.blocker_metadata),
        }

    def validate(self) -> DocumentationSurfaceReport:
        errors: list[str] = []
        for source in self.sources:
            errors.extend(source.validate())
        return DocumentationSurfaceReport(
            self.checker_name,
            tuple(errors),
            self.owner_contract(),
        )
