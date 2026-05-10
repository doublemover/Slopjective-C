#!/usr/bin/env python3
"""Apply lane audit status overrides to remaining_task_review_catalog.json."""

from __future__ import annotations

try:
    from remaining_task_status_overrides import (
        ALLOWED_STATUSES,
        OverrideEntry,
        apply_overrides,
        build_parser,
        load_catalog,
        load_overrides,
        main,
        normalize_status,
        normalize_text,
        parse_override_entry,
        read_json,
        render_summary,
        task_row_label,
        validate_catalog_status_invariants,
        write_catalog,
    )
except ModuleNotFoundError:
    from scripts.remaining_task_status_overrides import (
        ALLOWED_STATUSES,
        OverrideEntry,
        apply_overrides,
        build_parser,
        load_catalog,
        load_overrides,
        main,
        normalize_status,
        normalize_text,
        parse_override_entry,
        read_json,
        render_summary,
        task_row_label,
        validate_catalog_status_invariants,
        write_catalog,
    )


__all__ = [
    "ALLOWED_STATUSES",
    "OverrideEntry",
    "apply_overrides",
    "build_parser",
    "load_catalog",
    "load_overrides",
    "main",
    "normalize_status",
    "normalize_text",
    "parse_override_entry",
    "read_json",
    "render_summary",
    "task_row_label",
    "validate_catalog_status_invariants",
    "write_catalog",
]


if __name__ == "__main__":
    raise SystemExit(main())
