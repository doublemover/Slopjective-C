from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class DraftSyntaxSurfaceSources:
    ast_text: str
    parser_text: str
    builder_header_text: str
    builder_source_text: str
    parser_contract_text: str
    artifacts_text: str
    positive_text: str
    negative_text: str
    readme_text: str
    source_truth_paths: tuple[Path, ...]
