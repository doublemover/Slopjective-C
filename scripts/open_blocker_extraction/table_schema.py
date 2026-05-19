"""Markdown blocker table schema resolution."""

from __future__ import annotations

from typing import Sequence

from open_blocker_extraction.markdown_text import find_first_index, normalized_header
from open_blocker_extraction.model import BlockerTableSchema


def resolve_table_schema(header_cells: Sequence[str]) -> BlockerTableSchema | None:
    normalized_headers = [normalized_header(cell) for cell in header_cells]

    blocker_id_index = find_first_index(
        normalized_headers,
        lambda header: "blocker" in header and "id" in header,
    )
    if blocker_id_index is None:
        return None

    status_index = find_first_index(
        normalized_headers,
        lambda header: "status" in header or header == "state",
    )
    if status_index is None:
        return None

    owner_index = find_first_index(
        normalized_headers,
        lambda header: "responsible owner" in header,
    )
    if owner_index is None:
        owner_index = find_first_index(
            normalized_headers,
            lambda header: header == "owner",
        )
    if owner_index is None:
        owner_index = find_first_index(
            normalized_headers,
            lambda header: "owner" in header and "action" not in header,
        )

    due_date_index = find_first_index(
        normalized_headers,
        lambda header: "due date" in header,
    )

    excluded = {blocker_id_index, status_index}
    if owner_index is not None:
        excluded.add(owner_index)
    if due_date_index is not None:
        excluded.add(due_date_index)

    summary_index: int | None = None
    summary_priorities = (
        "blocking condition",
        "former blocking condition",
        "condition",
        "transition summary",
        "impacted scope",
        "blocks criteria",
        "blocks",
        "blocked",
        "summary",
    )
    for needle in summary_priorities:
        summary_index = find_first_index(
            normalized_headers,
            lambda header, needle=needle: needle in header,
        )
        if summary_index is not None and summary_index not in excluded:
            break
        summary_index = None
    if summary_index is None:
        for index in range(len(normalized_headers)):
            if index not in excluded:
                summary_index = index
                break

    owner_column_name = header_cells[owner_index] if owner_index is not None else None
    summary_column_name = header_cells[summary_index] if summary_index is not None else None
    return BlockerTableSchema(
        blocker_id_index=blocker_id_index,
        status_index=status_index,
        owner_index=owner_index,
        due_date_index=due_date_index,
        summary_index=summary_index,
        owner_column_name=owner_column_name,
        summary_column_name=summary_column_name,
    )


__all__ = ["resolve_table_schema"]
