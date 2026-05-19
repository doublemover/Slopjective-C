"""Built-in defaults for remaining spec task seeding."""

from __future__ import annotations

from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
SEED_CONFIG_SECTION = "seed_remaining_spec_tasks"

DEFAULT_SEED_CONFIG: dict[str, Any] = {
    "expected_task_count": 510,
    "repo_default": "doublemover/Slopjective-C",
    "catalog_md_default": "tmp/reports/remaining_task_review_catalog.md",
    "catalog_json_default": "tmp/reports/remaining_task_review_catalog.json",
    "sleep_seconds_default": 0.35,
    "lane_name": {
        "A": "Normative Closure",
        "B": "Implementation & Tooling",
        "C": "Governance & Ecosystem",
        "D": "Program Control & Release",
    },
    "conformance_milestone_by_tag": {
        "[CORE]": "Conformance: Core (E)",
        "[STRICT]": "Conformance: Strict (E)",
        "[CONC]": "Conformance: Strict Concurrency (E)",
        "[SYSTEM]": "Conformance: Strict System (E)",
        "[OPT-META]": "Conformance: Optional Metaprogramming (E)",
        "[OPT-CXX]": "Conformance: Optional Interop (E)",
        "[OPT-SWIFT]": "Conformance: Optional Interop (E)",
    },
    "lane_milestone_titles": {
        "A": "v0.12 Lane A - Normative Closure",
        "B": "v0.12 Lane B - Implementation & Tooling",
        "C": "v0.12 Lane C - Governance & Ecosystem",
        "D": "v0.12 Lane D - Program Control & Release",
    },
    "lane_milestone_due_on": {
        "A": "2026-04-15T00:00:00Z",
        "B": "2026-05-15T00:00:00Z",
        "C": "2026-05-01T00:00:00Z",
        "D": "2026-05-30T00:00:00Z",
    },
    "label_defs": {
        "lane:A": {"color": "0052CC", "description": "Parallel lane A: normative closure"},
        "lane:B": {"color": "1D76DB", "description": "Parallel lane B: implementation/tooling"},
        "lane:C": {"color": "0E8A16", "description": "Parallel lane C: governance/ecosystem"},
        "lane:D": {"color": "5319E7", "description": "Parallel lane D: program control/release"},
        "source:conformance-checklist": {
            "color": "D93F0B",
            "description": "Task sourced from conformance profile checklist",
        },
        "source:release-evidence": {
            "color": "FBCA04",
            "description": "Task sourced from release evidence checklist",
        },
        "source:planning-checklist": {
            "color": "BFDADC",
            "description": "Task sourced from planning package checklist",
        },
        "parallelizable": {"color": "C2E0C6", "description": "Can run in parallel lane scheduling"},
    },
    "planning_lane_by_issue": {
        "111": "D",
        "134": "A",
        "137": "A",
        "138": "C",
        "140": "B",
        "142": "D",
        "143": "A",
        "144": "A",
        "145": "A",
        "146": "C",
        "149": "A",
        "150": "B",
        "151": "B",
        "152": "C",
        "153": "C",
        "155": "B",
        "156": "B",
        "157": "B",
        "158": "B",
        "159": "D",
        "161": "B",
        "162": "B",
        "163": "B",
        "164": "C",
        "165": "D",
        "167": "B",
        "168": "C",
        "169": "C",
        "170": "C",
        "171": "D",
        "173": "B",
        "174": "C",
        "175": "C",
        "176": "C",
        "177": "D",
        "179": "B",
        "180": "C",
        "181": "C",
        "182": "C",
        "183": "D",
        "185": "D",
        "186": "D",
        "187": "D",
        "188": "D",
        "189": "D",
        "190": "D",
        "191": "D",
    },
}


__all__ = [
    "DEFAULT_SEED_CONFIG",
    "ROOT",
    "SEED_CONFIG_SECTION",
]
