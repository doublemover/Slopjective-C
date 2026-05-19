from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path


@dataclass
class ContractCheckResult:
    errors: list[str]
    warnings: list[str]
    required_paths: list[Path]
    missing_fragments: list[str]
