from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_library_cli_parity.subprocesses import CommandResult

MODE = "objc3c-library-cli-parity-v2"
DEFAULT_ARTIFACTS = (
    "module.diagnostics.json",
    "module.manifest.json",
    "module.ll",
    "module.o",
)


@dataclass(frozen=True)
class SourceExecution:
    work_key: str | None
    results: list[CommandResult]
    failures: list[str]
    routing: dict[str, Any] | None


@dataclass(frozen=True)
class ParityInputs:
    library_dir: Path
    cli_dir: Path
    default_artifacts: list[str]
    default_dimension_map: dict[str, str]
    execution: SourceExecution | None


@dataclass(frozen=True)
class ParityEvaluation:
    comparisons: list[dict[str, Any]]
    dimensions: list[dict[str, str]]
    failures: list[str]


__all__ = [
    "DEFAULT_ARTIFACTS",
    "MODE",
    "ParityEvaluation",
    "ParityInputs",
    "SourceExecution",
]
