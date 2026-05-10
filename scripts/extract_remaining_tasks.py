#!/usr/bin/env python3
"""Emit deterministic backlog views from remaining_task_review_catalog.json."""

from __future__ import annotations

from remaining_task_extraction.cli import *  # noqa: F403
from remaining_task_extraction.cli import __all__, main


if __name__ == "__main__":
    raise SystemExit(main())
