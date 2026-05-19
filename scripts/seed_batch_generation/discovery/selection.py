"""Selection and filtering for seed matrix discovery candidates."""

from __future__ import annotations

from .candidates import MatrixSourceCandidate
from .constants import MATRIX_SOURCE_TABLES


def candidates_containing_required_tables(
    candidates: list[MatrixSourceCandidate],
) -> list[MatrixSourceCandidate]:
    selected: list[MatrixSourceCandidate] = []
    for candidate in candidates:
        source_text = candidate.path.read_text(encoding="utf-8")
        if _contains_required_table_headers(source_text):
            selected.append(candidate)
    return selected


def select_single_candidate(
    candidates: list[MatrixSourceCandidate],
) -> MatrixSourceCandidate:
    if not candidates:
        raise FileNotFoundError("no seed matrix source candidate found")
    if len(candidates) > 1:
        display_paths = ", ".join(candidate.display_path for candidate in candidates)
        raise ValueError(f"multiple seed matrix source candidates found: {display_paths}")
    return candidates[0]


def _contains_required_table_headers(source_text: str) -> bool:
    for columns in MATRIX_SOURCE_TABLES:
        header = "| " + " | ".join(columns) + " |"
        if header not in source_text:
            return False
    return True
