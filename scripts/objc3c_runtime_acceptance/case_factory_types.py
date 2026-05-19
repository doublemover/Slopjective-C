"""Shared runtime acceptance case factory types."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any, Callable

from objc3c_runtime_acceptance.case_result import CaseResult

CaseFactory = Callable[[], CaseResult]
LabeledCaseFactories = list[tuple[str, CaseFactory]]


@dataclass(frozen=True)
class CaseFactoryContext:
    domains: Any
    clangxx: str
    run_dir: Path


__all__ = ["CaseFactory", "CaseFactoryContext", "LabeledCaseFactories"]
