#!/usr/bin/env python3
"""Compute deterministic activation trigger state from offline snapshots."""

from __future__ import annotations

import sys
from pathlib import Path

SCRIPT_ROOT = Path(__file__).resolve().parent
ROOT = SCRIPT_ROOT.parent
if str(SCRIPT_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPT_ROOT))

from activation_triggers.cli import build_parser
from activation_triggers.cli import main
from activation_triggers.inputs import canonical_blocker_key
from activation_triggers.inputs import count_actionable_catalog_rows
from activation_triggers.inputs import count_items
from activation_triggers.inputs import load_json
from activation_triggers.inputs import normalize_actionable_statuses
from activation_triggers.inputs import parse_open_blockers
from activation_triggers.inputs import parse_t4_overlay
from activation_triggers.model import CONTRACT_ID
from activation_triggers.model import DEFAULT_ACTIONABLE_STATUSES
from activation_triggers.model import EXIT_GATE_CLOSED
from activation_triggers.model import EXIT_GATE_OPEN
from activation_triggers.model import EXIT_HARD_FAILURE
from activation_triggers.model import OPEN_BLOCKERS_TRIGGER_ID
from activation_triggers.model import TRIGGER_ORDER
from activation_triggers.model import utc_now_string
from activation_triggers.payload import build_freshness_entry
from activation_triggers.payload import build_payload
from activation_triggers.rendering import render_markdown

__all__ = [
    "CONTRACT_ID",
    "DEFAULT_ACTIONABLE_STATUSES",
    "EXIT_GATE_CLOSED",
    "EXIT_GATE_OPEN",
    "EXIT_HARD_FAILURE",
    "OPEN_BLOCKERS_TRIGGER_ID",
    "ROOT",
    "SCRIPT_ROOT",
    "TRIGGER_ORDER",
    "build_freshness_entry",
    "build_parser",
    "build_payload",
    "canonical_blocker_key",
    "count_actionable_catalog_rows",
    "count_items",
    "load_json",
    "main",
    "normalize_actionable_statuses",
    "parse_open_blockers",
    "parse_t4_overlay",
    "render_markdown",
    "utc_now_string",
]


if __name__ == "__main__":
    raise SystemExit(main())
