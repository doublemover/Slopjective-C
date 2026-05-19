#!/usr/bin/env python3
"""Compute deterministic bootstrap readiness from offline snapshots."""

from __future__ import annotations

try:
    from bootstrap_readiness_checker import (
        EXIT_BLOCKED,
        EXIT_BOOTSTRAPPABLE,
        EXIT_HARD_FAILURE,
        ROOT,
        build_parser,
        build_payload,
        canonical_blocker_key,
        count_items,
        count_open_blockers,
        count_open_catalog_tasks,
        load_json,
        main,
        render_markdown,
    )
except ModuleNotFoundError:
    from scripts.bootstrap_readiness_checker import (
        EXIT_BLOCKED,
        EXIT_BOOTSTRAPPABLE,
        EXIT_HARD_FAILURE,
        ROOT,
        build_parser,
        build_payload,
        canonical_blocker_key,
        count_items,
        count_open_blockers,
        count_open_catalog_tasks,
        load_json,
        main,
        render_markdown,
    )


__all__ = [
    "EXIT_BLOCKED",
    "EXIT_BOOTSTRAPPABLE",
    "EXIT_HARD_FAILURE",
    "ROOT",
    "build_parser",
    "build_payload",
    "canonical_blocker_key",
    "count_items",
    "count_open_blockers",
    "count_open_catalog_tasks",
    "load_json",
    "main",
    "render_markdown",
]


if __name__ == "__main__":
    raise SystemExit(main())
